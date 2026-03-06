// scheduling.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include "scheduling.h"
#include "stdlib.h"
#include <windows.h>

#define getstruct(type) (type *) malloc(sizeof(type));//申请PCB空间
PCB * ready = NULL, *p;//就绪队列头指针，p为当前进程


// 按照短进程优先（SJF）将新进程插入就绪队列：ntime 小的放在前面
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
		printf("\n");
		p->rtime = 0;
		p->state = 'W';//W为等待状态
		p->link = NULL;//link为NULL表示下一个进程为空
		sort();
	}
	return( true);
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
	printf("jinchen[%s],\n", p->name);
	free(p);
	return(true);
}
// 非抢占式：一旦运行，执行到完成（SJF 非抢占）
bool running()
{
	p->state = 'R';
	// 直接运行至完成
	while (p->rtime < p->ntime)
	{
		p->rtime++;
		// 若需可视化/步进，这里可以打印或等待
	}
	// 运行完成，释放进程
	destory();
	return true;
}

int _tmain(int argc, _TCHAR* argv[])//主函数
{	
	SetConsoleOutputCP(65001);
	int len, h = 0;//h为已执行进程数，len为就绪队列长度
	char ch;//ch为用户输入的字符
	input();
	len = space();
	while ((len != 0) && (ready!=NULL))
	{
		ch = getchar();
		h++;
		printf("\n The execute number:%d \n",h);
		p = ready;
		ready = p->link;
		p->link = NULL;
		p->state = 'R';
		check();
		running();
		printf("\n.......");
		ch = getchar();
	}
	printf("\n\n 所有进程已经运行完成！successful\n");
	ch = getchar();
	return 0;
}
