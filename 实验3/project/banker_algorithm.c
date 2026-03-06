#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

#define PROCESS 5
#define RESOURCE 3

// 全局变量
int available[RESOURCE]; // 可用资源向量
int max[PROCESS][RESOURCE]; // 最大需求矩阵
int allocation[PROCESS][RESOURCE]; // 分配矩阵
int need[PROCESS][RESOURCE]; // 需要矩阵
bool finish[PROCESS]; // 进程完成标志

//初始化系统资源和进程数据
void initialze(){
    int i, j;

    //输入总资源数
    printf("请输入总资源数：");
    int total[RESOURCE];
    for (i = 0; i < RESOURCE; i++){  //输入每个资源的总数量
        scanf("%d", &total[i]);
    }

    //输入每个进程的最大需求矩阵
    printf("\n输入每个进程对三种资源的最大需求量\n");
    for(i = 0; i < PROCESS; i++){
        printf("进程P%d:", i);
        for(j = 0; j < RESOURCE; j++){
            scanf("%d", &max[i][j]);
        }
    }

    //输入每个进程的当前已分配的资源
    printf("\n输入每个进程对三种资源的当前分配量(allocation矩阵)\n");
    int allocated_total[RESOURCE] = {0};
    for(i = 0; i < PROCESS; i++){
        printf("进程P%d:", i);
        for( j = 0; j < RESOURCE; j++)
        {
            scanf("%d", &allocation[i][j]);
            allocated_total[j] += allocation[i][j];
        }
    }

    // 计算Need矩阵和Available向量
    for(i = 0; i < PROCESS; i++){
        for(j = 0; j < RESOURCE; j++){
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }
    for(j = 0; j < RESOURCE; j++){
        available[j] = total[j] - allocated_total[j];
    }

    // 初始化进程完成标志
    for(i = 0; i < PROCESS; i++){
        finish[i] = false;
    }
}

// 安全检查算法
bool isSafe(int safe_sequence[]){
    int work[RESOURCE];
    int i, j, k;
    int count = 0;

    //初始化工作向量
    for(i = 0; i<RESOURCE; i++){
        work[i] = available[i];
    }

    //重置完成状态
    for(i = 0; i<PROCESS; i++){
        finish[i] = false;
    }

    while (count < PROCESS)
    {
        bool found = false;
         
        for(i = 0; i<PROCESS; i++){
            if(!finish[i]){
                
                //检查是否所有资源需求量都小于等于工作向量
                for(j = 0; j<RESOURCE; j++){
                    if(need[i][j] > work[j]){
                        break;
                    }
                }

                //若找到可执行进程
                if(j == RESOURCE){
                    for(k = 0; k<RESOURCE; k++){
                        work[k] += allocation[i][k];
                    }
                    finish[i] = true; 
                    safe_sequence[count++] = i;
                    found = true;
                }
            }
        }

        if (!found)
        {
            break;
        }
        
    }
    for ( i = 0; i < PROCESS; i++)
    {
        if (!finish[i])
        {
            return false;
        }
        
    }
    
    return true;
}

//资源请求处理
void requestResources(){
    int process;
    int request[RESOURCE];
    int i;

    printf("\n请输入请求资源的进程编号(0-%d): ",PROCESS-1);
    scanf("%d", &process);

    if (process < 0 || process >= PROCESS)
    {
        printf("\n编号无效\n");
        return;
    }

    printf("\n请输入请求的资源数量(A,B,C)\n");
    for(i = 0; i<RESOURCE; i++){
        scanf("%d", &request[i]);
    }
    
    //检查请求是否超过需求
    for(i = 0; i<RESOURCE; i++){
        if (request[i] > need[process][i])
        {
            printf("请求资源超过进程需求！\n");
            return;
        }
        
    }

    //检查是否超过可用资源
    for(i = 0; i<RESOURCE; i++){
        if(request[i] > available[i]){
            printf("可用资源不足，进程需等待\n");
            return;
        }
    }

    //预分配资源
    for(i = 0; i<RESOURCE; i++){
        available[i] -= request[i];
        allocation[process][i] += request[i];
        need[process][i] -= request[i];
    }

    //检查系统是否安全
    int safe_seq[PROCESS];
    if(isSafe(safe_seq)){
        printf("资源分配成4，系统处于安全状态\n");

    }else{
        //恢复资源分配
        for(i = 0; i<RESOURCE; i++){
            available[i] += request[i];
            allocation[process][i] -= request[i];
            need[process][i] += request[i];
        }
        printf("资源分配后系统不安全，分配失败，进程需等待\n");
    }
}

//显示系统状态
void displayStatus(){
    int i, j;
    printf("\n=======================系统状态=======================\n");
    printf("进程\tMax(A,B,C)\tAllocation(A,B,C)\tNeed(A,B,C)\n");
    for ( i = 0; i < PROCESS; i++)
    {
        printf("P%d\t", i);
        for(j = 0; j<RESOURCE; j++){
            printf("%d ", max[i][j]);
        }
        printf("\t\t");
        for ( j = 0; j < RESOURCE; j++)
        {
            printf("%d ", allocation[i][j]);
        }
        printf("\t\t");
        
        for ( j = 0; j < RESOURCE; j++)
        {
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }
    printf("\n可用资源(A,B,C):");
    for(j = 0; j<RESOURCE; j++){
        printf("%d", available[j]);
    }
    printf("\n");

    //检查安全状态
    int safe_seq[PROCESS];
    if (isSafe(safe_seq))
    {
        printf("出于安全状态，安全序列：");
        for(i = 0; i<PROCESS; i++){
            printf("P%d", safe_seq[i]);
        }
        printf("\n");
    }else{
        printf("系统处于不安全状态\n");
    }
    printf("===========================================================\n");    
}

void menu(){
    int chioce;

    while (1)
    {
        printf("\n============银行家算法模拟系统=============\n");
        printf("1.初始化系统数据\n");
        printf("2.检查系统的安全状态\n");
        printf("3.进程请求资源\n");
        printf("4.显示系统状态\n");
        printf("0.退出程序\n");
        printf("===========================================\n");
        printf("请输入操作选项:\n");
        scanf("%d", &chioce);

        switch(chioce){
            case 1:
                initialze();
                break;
            case 2:
            {
                int safe_seq[PROCESS];
                printf("检查中。。。\n");
                if (isSafe(safe_seq))
                {
                    printf("系统处于安全状态\n");
                    printf("安全序列：");
                    for(int i = 0; i<PROCESS; i++){
                        printf("P%d ", safe_seq[i]);
                    }
                    printf("\n");
                }else{
                    printf("系统不处于安全状态\n");
                }
                break;
            }
            case 3:
                requestResources();
                break;
            case 4:
                displayStatus();
                break;
            case 0:
                printf("程序已退出\n");
                return;
            default:
                printf("选项无效\n");
                
        }       
    }
    
}

int main(){
    SetConsoleOutputCP(65001);
    printf("===银行家算法模拟程序===\n");
    printf("支持的资源类型：A,B,C\n");
    printf("进程总数：%d\n", PROCESS);
    menu();
    return 0;
}