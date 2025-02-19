#include "fle.hpp"
#include <iomanip>
#include <iostream>

void FLE_nm(const FLEObject& obj)
{
    // TODO: 实现符号表显示工具
    // 1. 遍历所有符号
    //    - 跳过未定义符号 (section为空的情况)
    //    - 使用16进制格式显示符号地址
   for (const auto& symbol : obj.symbols) {
        if (symbol.section.empty()) {
            continue;
        }

        std::cout << std::setw(16) << std::setfill('0') << std::hex << symbol.offset << " ";

        char symbol_type = ' ';
        if (symbol.type == SymbolType::LOCAL) {
            if (symbol.section == ".text") {
                symbol_type = 't'; // 局部代码段符号
            } else if (symbol.section == ".data") {
                symbol_type = 'd'; // 局部数据段符号
            } else if (symbol.section == ".bss") {
                symbol_type = 'b'; // 局部BSS段符号
            } else if (symbol.section == ".rodata") {
                symbol_type = 'r'; // 局部只读数据段符号
            }
        } else if (symbol.type == SymbolType::WEAK) {
            if (symbol.section == ".text") {
                symbol_type = 'W'; // 弱全局代码段符号
            } else if (symbol.section == ".data") {
                symbol_type = 'V'; // 弱全局数据段符号
            } else if (symbol.section == ".bss") {
                symbol_type = 'V'; // 弱全局BSS段符号
            } else if (symbol.section == ".rodata") {
                symbol_type = 'V'; // 弱全局只读数据段符号
            }
        } else if (symbol.type == SymbolType::GLOBAL) {
            if (symbol.section == ".text") {
                symbol_type = 'T'; // 强全局代码段符号
            } else if (symbol.section == ".data") {
                symbol_type = 'D'; // 强全局数据段符号
            } else if (symbol.section == ".bss") {
                symbol_type = 'B'; // 强全局BSS段符号
            } else if (symbol.section == ".rodata") {
                symbol_type = 'R'; // 强全局只读数据段符号
            }
        } else if (symbol.type == SymbolType::UNDEFINED) {
            symbol_type = 'U'; // 未定义符号
        }

        std::cout << symbol_type << " ";

        std::cout << symbol.name << std::endl;
    }
    

    // 2. 确定符号类型字符
    //    - 处理弱符号: 代码段用'W'，其他段用'V'
    //    - 根据段类型(.text/.data/.bss/.rodata)和符号类型(GLOBAL/LOCAL)确定显示字符
    //    - 全局符号用大写字母，局部符号用小写字母

    // 3. 按格式输出
    //    - [地址] [类型] [符号名]
    //    - 地址使用16位十六进制，左侧补0

    throw std::runtime_error("Not implemented");
}