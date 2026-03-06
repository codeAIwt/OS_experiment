#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define MAX_PAGES 1000
#define MAX_FRAMES 10

// 页面结构体
typedef struct {
    int page_number;
    int last_used;  // 用于LRU算法
} Page;

// 页框结构体
typedef struct {
    int page_number;
    int valid;      // 有效位，1表示页框中有页面，0表示空闲
    int loaded_time; // 装入时间，用于FIFO算法
} Frame;

// 系统状态结构体
typedef struct {
    Frame frames[MAX_FRAMES];
    int frame_count;
    int page_faults;
    int total_accesses;
} SystemState;

// 初始化系统状态
void init_system(SystemState *state, int frame_count) {
    state->frame_count = frame_count;
    state->page_faults = 0;
    state->total_accesses = 0;
    
    for (int i = 0; i < frame_count; i++) {
        state->frames[i].valid = 0;
        state->frames[i].page_number = -1;
        state->frames[i].loaded_time = 0;
    }
}

// 查找页面是否在内存中
int find_page(SystemState *state, int page_number) {
    for (int i = 0; i < state->frame_count; i++) {
        if (state->frames[i].valid && state->frames[i].page_number == page_number) {
            return i; // 返回页框索引
        }
    }
    return -1; // 页面不在内存中
}

// FIFO算法实现
int fifo_replacement(SystemState *state, int page_number, int current_time) {
    state->total_accesses++;
    
    // 检查页面是否已在内存中
    int frame_index = find_page(state, page_number);
    if (frame_index != -1) {
        // 页面命中，不需要置换
        return 0;
    }
    
    // 发生缺页中断
    state->page_faults++;
    
    // 查找空闲页框
    for (int i = 0; i < state->frame_count; i++) {
        if (!state->frames[i].valid) {
            // 找到空闲页框
            state->frames[i].valid = 1;
            state->frames[i].page_number = page_number;
            state->frames[i].loaded_time = current_time;
            return 1;
        }
    }
    
    // 没有空闲页框，需要置换
    // 找到最早装入的页面（FIFO）
    int oldest_frame = 0;
    for (int i = 1; i < state->frame_count; i++) {
        if (state->frames[i].loaded_time < state->frames[oldest_frame].loaded_time) {
            oldest_frame = i;
        }
    }
    
    // 置换页面
    state->frames[oldest_frame].page_number = page_number;
    state->frames[oldest_frame].loaded_time = current_time;
    
    return 1;
}

// LRU算法实现
int lru_replacement(SystemState *state, int page_number, int current_time) {
    state->total_accesses++;
    
    // 检查页面是否已在内存中
    int frame_index = find_page(state, page_number);
    if (frame_index != -1) {
        // 页面命中，更新最近使用时间
        state->frames[frame_index].loaded_time = current_time;
        return 0;
    }
    
    // 发生缺页中断
    state->page_faults++;
    
    // 查找空闲页框
    for (int i = 0; i < state->frame_count; i++) {
        if (!state->frames[i].valid) {
            // 找到空闲页框
            state->frames[i].valid = 1;
            state->frames[i].page_number = page_number;
            state->frames[i].loaded_time = current_time;
            return 1;
        }
    }
    
    // 没有空闲页框，需要置换
    // 找到最久未使用的页面（LRU）
    int lru_frame = 0;
    for (int i = 1; i < state->frame_count; i++) {
        if (state->frames[i].loaded_time < state->frames[lru_frame].loaded_time) {
            lru_frame = i;
        }
    }
    
    // 置换页面
    state->frames[lru_frame].page_number = page_number;
    state->frames[lru_frame].loaded_time = current_time;
    
    return 1;
}

// 打印当前内存状态
void print_memory_state(SystemState *state) {
    printf("当前内存中的页面: ");
    for (int i = 0; i < state->frame_count; i++) {
        if (state->frames[i].valid) {
            printf("%d ", state->frames[i].page_number);
        } else {
            printf("- ");
        }
    }
    printf("\n");
}

// 生成随机页面访问序列
void generate_random_sequence(int *sequence, int length, int max_page_number, unsigned int seed) {
    srand(seed);
    for (int i = 0; i < length; i++) {
        sequence[i] = rand() % (max_page_number + 1);
    }
}

// 运行页面置换算法
void run_algorithm(SystemState *state, int *sequence, int length, const char *algorithm_name, 
                  int (*replacement_func)(SystemState *, int, int)) {
    printf("\n=== %s 算法 ===\n", algorithm_name);
    init_system(state, state->frame_count);
    
    for (int i = 0; i < length; i++) {
        int page_fault = replacement_func(state, sequence[i], i);
        printf("访问页面 %d: ", sequence[i]);
        if (page_fault) {
            printf("缺页中断 ");
        } else {
            printf("页面命中 ");
        }
        print_memory_state(state);
    }
    
    double fault_rate = (double)state->page_faults / state->total_accesses * 100;
    printf("\n%s 算法统计:\n", algorithm_name);
    printf("总访问次数: %d\n", state->total_accesses);
    printf("缺页次数: %d\n", state->page_faults);
    printf("缺页率: %.2f%%\n", fault_rate);
}

int main() {
    //设置windows控制台编码为utf
    SetConsoleOutputCP(65001);
    
    int frame_count, max_page_number, sequence_length;
    unsigned int seed;
    int use_custom_sequence;
    
    printf("操作系统实验四 - 页面置换算法仿真\n");
    printf("=====================================\n");
    
    // 交互式参数设置
    printf("\n请设置仿真参数：\n");
    printf("1. 输入页框数量 (1-%d): ", MAX_FRAMES);
    scanf("%d", &frame_count);
    
    printf("2. 输入最大页面编号: ");
    scanf("%d", &max_page_number);
    
    printf("3. 输入页面序列长度: ");
    scanf("%d", &sequence_length);
    
    printf("4. 输入伪随机数种子 (0表示使用当前时间): ");
    scanf("%u", &seed);
    
    printf("5. 使用自定义序列还是随机序列？(0=随机序列, 1=自定义序列): ");
    scanf("%d", &use_custom_sequence);
    
    SystemState state;
    state.frame_count = frame_count;
    
    int *sequence = (int*)malloc(sequence_length * sizeof(int));
    
    if (use_custom_sequence) {
        printf("请输入%d个页面编号 (用空格分隔): ", sequence_length);
        for (int i = 0; i < sequence_length; i++) {
            scanf("%d", &sequence[i]);
        }
    } else {
        if (seed == 0) {
            seed = (unsigned int)time(NULL);
        }
        generate_random_sequence(sequence, sequence_length, max_page_number, seed);
    }
    
    printf("\n仿真参数设置完成：\n");
    printf("页框数量: %d\n", frame_count);
    printf("最大页面号: %d\n", max_page_number);
    printf("序列长度: %d\n", sequence_length);
    printf("随机种子: %u\n", seed);
    printf("页面访问序列: ");
    for (int i = 0; i < sequence_length; i++) {
        printf("%d ", sequence[i]);
    }
    printf("\n");
    
    // FIFO算法测试
    //run_algorithm(&state, sequence, sequence_length, "FIFO", fifo_replacement);
    
    // LRU算法测试
    run_algorithm(&state, sequence, sequence_length, "LRU", lru_replacement);
    
    free(sequence);
    
    return 0;
}