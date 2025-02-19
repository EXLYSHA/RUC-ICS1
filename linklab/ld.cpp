#include "fle.hpp"
#include <cassert>
#include <fstream> // 引入文件流库
#include <iostream>
#include <map>
#include <stdexcept>
#include <unordered_set>
#include <vector>
FLEObject FLE_ld(const std::vector<FLEObject>& objects)
{
    // TODO: 实现链接器
    // 1. 收集和合并段
    //    - 遍历所有输入对象的段
    //    - 按段名分组并计算偏移量
    //    - 设置段的属性（读/写/执行）

    FLEObject output;

    std::unordered_set<std::string> section_names; // 用于存储所有的section name
    for (const auto& obj : objects) {
        for (const auto& sec : obj.sections) {
            section_names.insert(sec.first);
        }
    }

    std::map<std::string, Symbol> global_symbols; // 数据表示：symbol name
    std::map<std::string, Symbol> local_symbols; // 数据分别表示：symbol name + "&" + object name
    struct Info {
        std::string section_name;
        int reloc_index;
        SymbolType type;
        int object_index;
    };
    std::vector<Info> reloc_info; // 数据分别表示：section name, reloc index, symbol type, object index

    int section_index = 0;
    for (auto section_name : section_names) {
        FLESection load_section;
        int offset = 0;
        for (int i = 0; i < (int)objects.size(); i++) {
            auto obj = objects[i];
            auto find = obj.sections.find(section_name);
            if (find != obj.sections.end()) {
                auto sec = find->second;
                load_section.data.insert(load_section.data.end(), sec.data.begin(), sec.data.end());
                for (auto reloc : sec.relocs) {
                    reloc.offset += offset;
                    load_section.relocs.push_back(reloc);
                    auto find = std::find_if(obj.symbols.begin(), obj.symbols.end(), [&reloc](const Symbol& sym) {
                        return sym.name == reloc.symbol;
                    });
                    if (find != obj.symbols.end()) {
                        Info info;
                        info.section_name = section_name;
                        info.reloc_index = load_section.relocs.size() - 1;
                        info.type = find->type;
                        info.object_index = i;
                        reloc_info.push_back(info);
                    }
                }

                // 2. 处理符号
                //    - 收集所有全局符号和局部符号
                //    - 处理符号冲突（强符号/弱符号）

                for (auto& sym : obj.symbols) {
                    if (sym.section != section_name) {
                        continue;
                    }
                    sym.offset += offset + 0x2000 * section_index;

                    switch (sym.type) {
                    case SymbolType::LOCAL:
                        local_symbols[sym.name + "&" + obj.name] = sym;
                        break;

                    case SymbolType::GLOBAL: {
                        auto it = global_symbols.find(sym.name);
                        if (it == global_symbols.end()) {
                            global_symbols[sym.name] = sym;
                        } else {
                            auto& existing = it->second;
                            if (existing.type == SymbolType::WEAK || existing.type == SymbolType::UNDEFINED) {
                                global_symbols[sym.name] = sym;
                            } else if (existing.type == SymbolType::GLOBAL) {
                                throw std::runtime_error("multiple definition of strong symbol: " + sym.name);
                            }
                        }
                        break;
                    }

                    default: {
                        auto it = global_symbols.find(sym.name);
                        if (it == global_symbols.end() || it->second.type == SymbolType::UNDEFINED) {
                            global_symbols[sym.name] = sym;
                        }
                        break;
                    }
                    }
                }
                auto find = std::find_if(obj.shdrs.begin(), obj.shdrs.end(), [&section_name](const SectionHeader& shdr) {
                    return shdr.name == section_name;
                });
                if (find == obj.shdrs.end()) {
                    throw std::runtime_error("Section header not found.");
                }
                offset += find->size;
            }
        }
        load_section.name = section_name;

        ProgramHeader phdr;
        phdr.name = section_name;
        phdr.size = offset;
        phdr.vaddr = 0x400000 + 0x2000 * section_index;
        if (section_name.substr(0, 5) == ".text") {
            phdr.flags = 5;
        } else if (section_name.substr(0, 7) == ".rodata") {
            phdr.flags = 4;
        } else if (section_name.substr(0, 5) == ".data") {
            phdr.flags = 6;
        } else if (section_name.substr(0, 4) == ".bss") {
            phdr.flags = 6;
        }
        output.phdrs.push_back(phdr);
        output.sections[section_name] = load_section;
        section_index++;
    }
    for (auto& [sym_name, sym] : global_symbols) {
        if (sym.type == SymbolType::UNDEFINED) {
            throw std::runtime_error("Undefined symbol: " + sym_name);
        }
    }
    for (auto [sym_name, sym] : local_symbols) {
        if (sym.type == SymbolType::UNDEFINED) {
            throw std::runtime_error("Undefined symbol: " + sym_name);
        }
    }

    // 3. 重定位
    //    - 遍历所有重定位项
    //    - 计算并填充重定位值
    //    - 注意不同重定位类型的处理

    // 4. 生成可执行文件
    //    - 设置程序入口点（_start）
    //    - 确保所有必要的段都已正确设置

    for (auto& [section_name, sec] : output.sections) {
        uint64_t offset = 0;
        for (auto phdr : output.phdrs) {
            if (phdr.name == section_name) {
                offset = phdr.vaddr;
            }
        }
        for (int i = 0; i < (int)sec.relocs.size(); i++) {
            auto reloc = sec.relocs[i];
            auto find = std::find_if(reloc_info.begin(), reloc_info.end(), [&section_name, i](const Info& info) {
                return info.section_name == section_name && info.reloc_index == i;
            });
            if (find == reloc_info.end()) {
                throw std::runtime_error("Relocation info not found.");
            }
            auto sym_type = find->type;
            auto obj_index = find->object_index;
            auto rel_type = reloc.type;
            std::map<std::string, Symbol>::iterator find1;
            if (sym_type == SymbolType::LOCAL) {
                find1 = local_symbols.find(reloc.symbol + "&" + objects[obj_index].name);
                if (find1 == local_symbols.end()) {
                    throw std::runtime_error("Undefined symbol: " + reloc.symbol);
                }
            } else {
                find1 = global_symbols.find(reloc.symbol);
                if (find1 == global_symbols.end()) {
                    throw std::runtime_error("Undefined symbol: " + reloc.symbol);
                }
            }
            uint64_t addr = find1->second.offset + 0x400000 + (reloc.addend - (reloc.offset + offset)) * (rel_type == RelocationType::R_X86_64_PC32);
            int bytes = 4 + (rel_type == RelocationType::R_X86_64_64) * 4;
            if (reloc.type == RelocationType::R_X86_64_32) {
                unsigned long test = addr & 0xFFFFFFFF;
                if (test != addr) {
                    throw std::runtime_error("Relocation R_X86_64_32 Failed.");
                }
            } else if (reloc.type == RelocationType::R_X86_64_32S) {
                unsigned long test = addr & 0xFFFFFFFF;
                long sign = static_cast<long>(static_cast<int>(test));
                if (sign != addr) {
                    throw std::runtime_error("Relocation R_X86_64_32S Failed.");
                }
            }
            for (int j = 0; j < bytes; j++) {
                sec.data[reloc.offset + j] = uint8_t((addr >> (j * 8)) & 0xff);
            }
        }
    }

    auto find = global_symbols.find("_start");
    if (find == global_symbols.end()) {
        throw std::runtime_error("No _start symbol found.");
    }
    output.entry = find->second.offset + 0x400000;

    for (auto& [section_name, sec] : output.sections) {
        sec.relocs.clear();
    }


    output.name = "a.out";
    output.type = ".exe";
    return output;
}