/*
请注意，你的代码不能出现任何 int/short/char/float/double/auto 等局部变量/函数传参，我们仅允许使用 reg 定义的寄存器变量。
其中 reg 等价于一个 int。

你不能自己申请额外的内存，即不能使用 new/malloc，作为补偿我们传入了一段 buffer，大小为 BUFFER_SIZE = 64，你可以视情况使用。

我们的数组按照 A, B, C, buffer 的顺序在内存上连续紧密排列，且 &A = 0x30000000（这是模拟的设定，不是 A 的真实地址）

如果你需要以更自由的方式访问内存，你可以以相对 A 的方式访问，比如 A[100]，用 *(0x30000000) 是无法访问到的。

如果你有定义常量的需求（更严谨的说法是，你想定义的是汇编层面的立即数，不应该占用寄存器），请参考下面这种方式使用宏定义来完成。
*/

#include "cachelab.h"

#define m case0_m
#define n case0_n
#define p case0_p

// 我们用这个 2*2*2 的矩阵乘法来演示寄存器是怎么被分配的
void gemm_case0(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {  // allocate 0 1 2 3
    reg tmpa[2][2];
    reg tmpb[2][2];
    reg tmpc[2][2];
    tmpa[0][0] = A[0];
    tmpa[0][1] = A[1];
    tmpa[1][0] = A[2];
    tmpa[1][1] = A[3];
    tmpb[0][0] = B[0];
    tmpb[0][1] = B[1];
    tmpb[1][0] = B[2];
    tmpb[1][1] = B[3];
    tmpc[0][0] = tmpa[0][0]*tmpb[0][0]+tmpa[0][1]*tmpb[1][0];
    tmpc[0][1] = tmpa[0][0]*tmpb[0][1]+tmpa[0][1]*tmpb[1][1];
    tmpc[1][0] = tmpa[1][0]*tmpb[0][0]+tmpa[1][1]*tmpb[1][0];
    tmpc[1][1] = tmpa[1][0]*tmpb[0][1]+tmpa[1][1]*tmpb[1][1];
    C[0] = tmpc[0][0];
    C[1] = tmpc[0][1];
    C[2] = tmpc[1][0];
    C[3] = tmpc[1][1];
}

#undef m
#undef n
#undef p

#define m case1_m
#define n case1_n
#define p case1_p

#define BP 16

void gemm_case1(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {
    for (reg i = 0; i < m; ++i) {               // 遍历A的每一行
        for (reg j = 0; j < p; j += BP) {       // 按列块遍历C和B
            // 初始化累加寄存器
            reg c[BP];
            for (reg k = 0; k < BP; ++k) {
                c[k + 0] = 0;
            }
            for (reg k = 0; k < n; ++k) {       // 遍历A的列和B的行
                reg a = A[i * n + k];           // 加载A[i][k]到寄存器

                for (reg h = 0; h < BP; ++h) {
                    reg* b = new reg;
                    *b = B[k * p + j + h];
                    c[h + 0] += a * *b;
                    delete b;
                }
            }

            // 将累加结果写回C矩阵
            for (reg k = 0; k < BP; ++k) {
                C[i * p + j + k] = c[k + 0];
            }
        }
    }
}

#undef m
#undef n
#undef p

#define m case2_m
#define n case2_n
#define p case2_p

#define BP1 16
#define BP2 16

void gemm_case2(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {
    for (reg i = 0; i < m; ++i) {  // 遍历A的每一行
        for (reg j = 0; j < p - BP1; j += BP1) {       // 按列块遍历C和B
            // 初始化累加寄存器
            reg c[BP1];
            for (reg k = 0; k < BP1; ++k) {
                c[k + 0] = 0;
            }
            for (reg k = 0; k < n; ++k) {       // 遍历A的列和B的行
                reg a = A[i * n + k];           // 加载A[i][k]到寄存器

                for (reg h = 0; h < BP1; ++h) {
                    reg* b = new reg;
                    *b = B[k * p + j + h];
                    c[h + 0] += a * *b;
                    delete b;
                }
            }

            // 将累加结果写回C矩阵
            for (reg k = 0; k < BP1; ++k) {
                C[i * p + j + k] = c[k + 0];
            }
        }

        // 处理尾部列块
            reg c[BP2];
            for (reg k = 0; k < BP2; ++k) {
                c[k + 0] = 0;
            }
            for (reg k = 0; k < n; ++k) {       // 遍历A的列和B的行
                reg a = A[i * n + k];           // 加载A[i][k]到寄存器

                for (reg h = 0; h < BP2; ++h) {
                    reg* b = new reg;
                    *b = B[k * p + BP1 + h];
                    c[h + 0] += a * *b;
                    delete b;
                }
            }

            // 将累加结果写回C矩阵
            for (reg k = 0; k < BP2; ++k) {
                C[i * p + BP1 + k] = c[k + 0];
            }
        }
}

#undef m
#undef n
#undef p

#define m case3_m
#define n case3_n
#define p case3_p
#define BP1 17
#define BP2 31-BP1

void gemm_case3(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {
    for (reg i = 0; i < m; ++i) {  // 遍历A的每一行
        for (reg j = 0; j < p - BP1; j += BP1) {       // 按列块遍历C和B
            // 初始化累加寄存器
            reg c[BP1];
            for (reg k = 0; k < BP1; ++k) {
                c[k + 0] = 0;
            }
            for (reg k = 0; k < n; ++k) {       // 遍历A的列和B的行
                reg a = A[i * n + k];           // 加载A[i][k]到寄存器

                for (reg h = 0; h < BP1; ++h) {
                    reg* b = new reg;
                    *b = B[k * p + j + h];
                    c[h + 0] += a * *b;
                    delete b;
                }
            }

            // 将累加结果写回C矩阵
            for (reg k = 0; k < BP1; ++k) {
                C[i * p + j + k] = c[k + 0];
            }
        }

        // 处理尾部列块
            reg c[BP2];
            for (reg k = 0; k < BP2; ++k) {
                c[k + 0] = 0;
            }
            for (reg k = 0; k < n; ++k) {       // 遍历A的列和B的行
                reg a = A[i * n + k];           // 加载A[i][k]到寄存器

                for (reg h = 0; h < BP2; ++h) {
                    reg* b = new reg;
                    *b = B[k * p + BP1 + h];
                    c[h + 0] += a * *b;
                    delete b;
                }
            }

            // 将累加结果写回C矩阵
            for (reg k = 0; k < BP2; ++k) {
                C[i * p + BP1 + k] = c[k + 0];
            }
        }
}

#undef m
#undef n
#undef p