// schedulingSJF.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include "scheduling.h"
#include "stdlib.h"
#include "string.h"
#include <windows.h>

#define getstruct(type) (type *) malloc(sizeof(type));//申请PCB空间
PCB * ready = NULL, *p;//就绪队列头指针，p为当前进程
int current_time = 0; // 当前系统时间
int total_turnaround_time = 0; // 总周转时间
int total_weighted_turnaround_time = 0; // 总带权周转时间

// 作业信息结构体，用于存储作业的完整调度信息
struct JobInfo {
	int id;
	char name[10];
	int arrive_time;
	int service_time;
	int start_time;
	int finish_time;
	int waiting_time;
	int turnaround_time;
	float weighted_turnaround_time;
};

JobInfo job_infos[5]; // 存储5个作业的调度信息
int job_count = 0; // 作业计数器

// 将新进程按提交时间顺序加入就绪队列（SJF调度时会重新排序可运行的作业）SJF：非抢占式
bool sort()
{
	// 先按提交时间排序，SJF调度时会根据当前可运行的作业重新选择最短作业
	if (ready == NULL)
	{
		p->link = NULL;
		ready = p;
	}
	else
	{
		PCB *prev = NULL;
		PCB *cur = ready;
		// 找到合适的插入位置（按提交时间从小到大）
		while (cur != NULL && cur->atime < p->atime)
		{
			prev = cur;
			cur = cur->link;
		}
		// 插入到链表中
		if (prev == NULL)
		{
			// 插入到队头
			p->link = ready;
			ready = p;
		}
		else
		{
			// 插入到中间或队尾
			p->link = cur;
			prev->link = p;
		}
	}
	return true;
}

bool input()//输入进程信息
{
	// 根据附录1，直接初始化5个作业的信息
	int job_data[5][3] = {
		{1, 160, 20}, // JA: 作业号1, 提交时间02:40=160分钟, 运行时间20分钟
		{2, 170, 30}, // JB: 作业号2, 提交时间02:50=170分钟, 运行时间30分钟
		{3, 175, 10}, // JC: 作业号3, 提交时间02:55=175分钟, 运行时间10分钟
		{4, 180, 24}, // JD: 作业号4, 提交时间03:00=180分钟, 运行时间24分钟
		{5, 185, 6}   // JE: 作业号5, 提交时间03:05=185分钟, 运行时间6分钟
	};
	char job_names[5][3] = {"JA", "JB", "JC", "JD", "JE"};
	
	for (int i = 0; i < 5; i++)
	{
		p = getstruct(PCB);
		int job_id = job_data[i][0];
		int arrive_time = job_data[i][1];
		int service_time = job_data[i][2];
		
		// 保存作业基本信息到job_infos数组
		job_infos[i].id = job_id;
		strcpy(job_infos[i].name, job_names[i]);
		job_infos[i].arrive_time = arrive_time;
		job_infos[i].service_time = service_time;
		
		// 设置PCB信息
		strcpy(p->name, job_names[i]);
		p->atime = arrive_time;
		p->ntime = service_time;
		p->rtime = 0;
		p->nice = 0;
		p->state = 'W';//W为等待状态
		p->link = NULL;//link为NULL表示下一个进程为空
		sort();
	}
	job_count = 5;
	return true;
}

int space()
{
	int i = 0;
	PCB * pr = ready;
	while (pr != NULL)
	{
		i++;
		pr = pr->link;
	}
	return(i);
}

bool disp(PCB *pr)
{
	printf("\n name \t state \t nice \t needtime \t runtime \n");
	printf(" %s\t", pr->name);
	printf(" %c\t", pr->state);
	printf(" %d\t", pr->nice);
	printf(" %d\t", pr->ntime);
	printf("         %d\t", pr->rtime);
	return(true);
}

bool check()
{
	PCB * pr;
	printf("\n ****now%s",p->name);
	disp(p);
	pr = ready;
	if (pr != NULL)
	{
		printf("\n****就绪队列state");
	}
	else
	{
		printf("\n****就绪队列null\n");
	}
	while (pr != NULL)
	{
		disp(pr);
		pr = pr->link;
	}
	return(true);
}

bool destory()
{
	free(p);
	return(true);
}

bool running()
{
	// 计算开始时间
	int start_time = current_time > p->atime ? current_time : p->atime;
	
	// 计算完成时间
	int finish_time = start_time + p->ntime;
	
	// 计算周转时间 = 完成时间 - 提交时间
	int turnaround_time = finish_time - p->atime;
	
	// 计算等待时间 = 开始时间 - 提交时间
	int waiting_time = start_time - p->atime;
	
	// 计算带权周转时间 = 周转时间 / 运行时间
	float weighted_turnaround_time = (float)turnaround_time / p->ntime;
	
	// 更新当前系统时间
	current_time = finish_time;
	
	// 保存调度信息到job_infos数组
	for (int i = 0; i < job_count; i++)
	{
		if (strcmp(job_infos[i].name, p->name) == 0)
		{
			job_infos[i].start_time = start_time;
			job_infos[i].finish_time = finish_time;
			job_infos[i].waiting_time = waiting_time;
			job_infos[i].turnaround_time = turnaround_time;
			job_infos[i].weighted_turnaround_time = weighted_turnaround_time;
			break;
		}
	}
	
	// 更新统计信息
	total_turnaround_time += turnaround_time;
	total_weighted_turnaround_time += weighted_turnaround_time;
	
	// 运行完毕，释放进程
	destory();
	return true;
}

// 将分钟转换为时间格式（HH:MM）
void printTime(int minutes)
{
	int hour = minutes / 60 + 2; // 从02:00开始
	int min = minutes % 60;
	printf("%02d:%02d", hour, min);
}

// SJF调度：选择当前已到达且运行时间最短的作业
PCB* select_shortest_job()
{
	if (ready == NULL) return NULL;
	
	PCB *shortest = NULL;
	PCB *prev_shortest = NULL;
	PCB *prev = NULL;
	PCB *cur = ready;
	
	// 遍历就绪队列，找到已到达且运行时间最短的作业
	while (cur != NULL)
	{
		if (cur->atime <= current_time) // 作业已到达
		{
			if (shortest == NULL || cur->ntime < shortest->ntime)
			{
				shortest = cur;
				prev_shortest = prev;
			}
		}
		prev = cur;
		cur = cur->link;
	}
	
	// 如果没有找到已到达的作业（理论上不应该发生）
	if (shortest == NULL) return NULL;
	
	// 从就绪队列中移除选中的作业
	if (prev_shortest == NULL)
	{
		// 移除队头
		ready = shortest->link;
	}
	else
	{
		// 移除中间或队尾
		prev_shortest->link = shortest->link;
	}
	
	shortest->link = NULL;
	return shortest;
}

int _tmain(int argc, _TCHAR* argv[])//主函数
{	
	SetConsoleOutputCP(65001);
	// 初始化当前时间为第一个作业的提交时间（02:40 = 160分钟）
	current_time = 160;
	
	input();
	
	// SJF调度执行所有作业
	while (ready != NULL)
	{
		// 选择当前已到达且运行时间最短的作业
		p = select_shortest_job();
		if (p == NULL) break;
		
		p->state = 'R';
		running();
	}
	
	// 输出调度结果
	printf("\n\n===== 作业调度信息 =====\n");
	printf("作业编号\t作业名称\t提交时间\t要求服务运行时间\t开始时间\t完成时间\t等待时间\t周转时间\n");
	for (int i = 0; i < job_count; i++)
	{
		printf("%d\t\t%s\t\t", job_infos[i].id, job_infos[i].name);
		printTime(job_infos[i].arrive_time);
		printf("\t\t%d\t\t\t", job_infos[i].service_time);
		printTime(job_infos[i].start_time);
		printf("\t\t");
		printTime(job_infos[i].finish_time);
		printf("\t\t%d\t\t%d\n", job_infos[i].waiting_time, job_infos[i].turnaround_time);
	}
	
	// 输出统计信息
	float avg_turnaround_time = (float)total_turnaround_time / job_count;
	float avg_weighted_turnaround_time = (float)total_weighted_turnaround_time / job_count;
	
	printf("\n===== 统计信息 =====\n");
	printf("平均周转时间: %.2f\n", avg_turnaround_time);
	printf("平均带权周转时间: %.2f\n", avg_weighted_turnaround_time);
	
	// 等待用户按键
	char ch = getchar();
	return 0;
}