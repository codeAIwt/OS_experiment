// scheduling.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include "scheduling.h"
#include "stdlib.h"


#define getstruct(type) (type *) malloc(sizeof(type));//申请PCB空间
PCB * ready = NULL, *p;//就绪队列头指针，p为当前进程
PCB * incoming = NULL; // 按到达时间排序的未到达进程链表
int current_time = 0;  // 仿真时间


// 按照短进程优先（SJF）将新进程插入就绪队列：ntime 小的放在前面
//进程的动态到达：按到达时间排序，到达时加入就绪队列，按需要时间排序，需要时间短的优先
bool sort()
{
	if (ready == NULL)
	{
		p->link = NULL;
		ready = p;
		return true;
	}

	// 如果新进程的需要时间比队头小，则插入到队头
	if (p->ntime < ready->ntime)
	{
		p->link = ready;
		ready = p;
		return true;
	}

	PCB *first = ready;
	PCB *second = first->link;
	// 在链表中找到第一个比 p->ntime 大的位置，将 p 插入到其前面
	while (second != NULL)
	{
		if (p->ntime < second->ntime)
		{
			p->link = second;
			first->link = p;
			return true;
		}
		first = second;
		second = second->link;
	}

	// 到达链表尾，插入到末尾
	first->link = p;
	p->link = NULL;
	return true;
}
bool input()//输入进程信息
{
	int num;
	printf("\nSumNumber:");
	scanf("%d", &num);
	for (int i = 0; i < num; i++)
	{
		printf("\n num:%d", i);
		p = getstruct(PCB);
		printf("\n name:");
		scanf("%9s", p->name);
		printf("\n nice:");
		int temp;
		//scanf_s("%d", &temp, sizeof(int));
		//p->nice = temp;
		scanf("%d", &(p->nice));
		printf("\n ntime:");
		scanf("%d", &(p->ntime));
		printf("\n atime:");
		scanf("%d", &(p->atime));
		printf("\n");
		p->rtime = 0;
		p->state = 'W';//W为等待状态
		p->link = NULL;//link为NULL表示下一个进程为空
		// 将新进程插入到 incoming 列表（按到达时间升序）
		if (incoming == NULL || p->atime < incoming->atime)
		{
			p->link = incoming;
			incoming = p;
		}
		else
		{
			PCB *a = incoming;
			while (a->link != NULL && a->link->atime <= p->atime)
				a = a->link;
			p->link = a->link;
			a->link = p;
		}
	}
	return( true);
}

// 把所有到达时间 <= current_time 的进程从 incoming 移到 ready
void move_arrivals()
{
	while (incoming != NULL && incoming->atime <= current_time)
	{
		PCB *node = incoming;
		incoming = incoming->link;
		node->link = NULL;
		p = node;
		sort();
	}
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
	printf("\n ****now %s (t=%d)", p->name, current_time);
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
	printf("jinchen[%s],\n", p->name);
	free(p);
	return(true);
}
// 非抢占式：一旦运行，执行到完成（SJF 非抢占）
bool running()
{
	p->state = 'R';
	// 非抢占：运行到完成，同时在运行期间处理新到达的进程（只加入 ready，不抢占当前）
	while (p->rtime < p->ntime)
	{
		p->rtime++;
		current_time++;
		// 每个时间单位检查是否有新到达进程
		move_arrivals();
	}
	// 运行完成，释放进程
	destory();
	return true;
}

int _tmain(int argc, _TCHAR* argv[])//主函数
{
	int len, h = 0;//h为已执行进程数，len为就绪队列长度
	char ch;//ch为用户输入的字符
	input();
	// 仿真主循环：直到 incoming 与 ready 都为空
	while (incoming != NULL || ready != NULL)
	{
		// 将当前时刻或之前到达的进程移动到 ready
		move_arrivals();

		// 若就绪队列为空但还有未来到达的进程，则跳到下一个到达时间
		if (ready == NULL && incoming != NULL)
		{
			current_time = incoming->atime;
			move_arrivals();
		}

		if (ready != NULL)
		{
			h++;
			printf("\n The execute number:%d (t=%d)\n", h, current_time);
			p = ready;
			ready = p->link;
			p->link = NULL;
			p->state = 'R';
			check();
			running();
			printf("\n.......\n");
		}
	}
	printf("\n\n 所有进程已经运行完成！successful\n");
	ch = getchar();
	return 0;
}
