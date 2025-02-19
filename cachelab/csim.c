// student id: 2023202292
// please change the above line to your student id

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

typedef struct cacheline {
    int valid;
    long tag;
    long LRU;
} cacheline;

cacheline all[32][4];

int h_flag = 0, v_flag = 0;
char line[256];
int hits = 0, misses = 0, evictions = 0;
long s = -1, E = -1, b = -1;

void printSummary(int hits, int misses, int evictions) {
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}

void printHelp(const char* name) {
    printf(
        "Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n"
        "Options:\n"
        "  -h         Print this help message.\n"
        "  -v         Optional verbose flag.\n"
        "  -s <num>   Number of set index bits.\n"
        "  -E <num>   Number of lines per set.\n"
        "  -b <num>   Number of block offset bits.\n"
        "  -t <file>  Trace file.\n\n"
        "Examples:\n"
        "  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n"
        "  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n",
        name, name, name);
}

void L_opera(long addr, long size) {
    long b_bit = addr & ((1 << b) - 1);
    addr >>= b;
    long s_bit = addr & ((1 << s) - 1);
    long tag = addr >> s;

    for (long i = 0; i < E; i++) {
        if (all[s_bit][i].tag == tag && all[s_bit][i].valid == 1) {
            hits++;
            all[s_bit][i].LRU = 0;
            for (long j = 0; j < E; j++) {
                if (j != i && all[s_bit][j].valid == 1) {
                    all[s_bit][j].LRU++;
                }
            }
            return;
        }
    }
    misses++;
    for (long i = 0; i < E; i++) {
        if (all[s_bit][i].valid == 0) {
            all[s_bit][i].valid = 1;
            all[s_bit][i].LRU = 0;
            for (long j = 0; j < E; j++) {
                if (j != i && all[s_bit][j].valid == 1) {
                    all[s_bit][j].LRU++;
                }
            }
            all[s_bit][i].tag = tag;
            return;
        }
    }
    long maxLRU = 0;
    for (long i = 0; i < E; i++) {
        if (all[s_bit][i].LRU > maxLRU) {
            maxLRU = all[s_bit][i].LRU;
        }
    }
    for (long i = 0; i < E; i++) {
        if (all[s_bit][i].LRU == maxLRU) {
            all[s_bit][i].LRU = 0;
            for (long j = 0; j < E; j++) {
                if (j != i) {
                    all[s_bit][j].LRU++;
                }
            }
            all[s_bit][i].tag = tag;
            evictions++;
            return;
        }
    }
}

void S_opera(long addr, long size) {
    long b_bit = addr & ((1 << b) - 1);
    addr >>= b;
    long s_bit = addr & ((1 << s) - 1);
    long tag = addr >> s;

    for (long i = 0; i < E; i++) {
        if (all[s_bit][i].tag == tag && all[s_bit][i].valid == 1) {
            hits++;
            all[s_bit][i].LRU = 0;
            for (long j = 0; j < E; j++) {
                if (j != i && all[s_bit][j].valid == 1) {
                    all[s_bit][j].LRU++;
                }
            }
            return;
        }
    }
    misses++;
    for (long i = 0; i < E; i++) {
        if (all[s_bit][i].valid == 0) {
            all[s_bit][i].valid = 1;
            all[s_bit][i].LRU = 0;
            for (long j = 0; j < E; j++) {
                if (j != i && all[s_bit][j].valid == 1) {
                    all[s_bit][j].LRU++;
                }
            }
            all[s_bit][i].tag = tag;
            return;
        }
    }
    long maxLRU = 0;
    for (long i = 0; i < E; i++) {
        if (all[s_bit][i].LRU > maxLRU) {
            maxLRU = all[s_bit][i].LRU;
        }
    }
    for (long i = 0; i < E; i++) {
        if (all[s_bit][i].LRU == maxLRU) {
            all[s_bit][i].LRU = 0;
            for (long j = 0; j < E; j++) {
                if (j != i) {
                    all[s_bit][j].LRU++;
                }
            }
            all[s_bit][i].tag = tag;
            evictions++;
            return;
        }
    }
}

int main(int argc, char* argv[]) {
    // 数据初始化
    hits = 0, misses = 0, evictions = 0;
    h_flag = 0, v_flag = 0;
    long opt = -1;
    s = -1, E = -1, b = -1;
    char* trace = NULL;
    memset(all, 0, sizeof(all));

    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 'h':
                h_flag = 1;
                printHelp(argv[0]);
                break;
            case 'v':
                v_flag = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                trace = optarg;
                break;
        }
    }
    FILE* fil = fopen(trace, "r");
    while ((fgets(line, 100, fil)) != NULL) {
        char op;
        long addr, size;
        sscanf(line, " %c %lx,%ld", &op, &addr, &size);
        if (op == 'L') {
            L_opera(addr, size);
        } else if (op == 'S') {
            S_opera(addr, size);
        }
    }
    printSummary(hits, misses, evictions);
    return 0;
}
