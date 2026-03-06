// ProducConsum.cpp : Defines the entry point for the console application.
//
// 生产者消费者问题
#include "stdafx.h"


#include "windows.h"
#include <conio.h>
#include <stdlib.h>
//#include <fstream.h>
#include <fstream>
#include <io.h>
#include <string.h>
#include <stdio.h>


#define INTE_PER_SEC 100 
#define MAX_THREAD_NUM  64
#define SEM_MAX_FULL    64
#define PRODUC 'P'
#define CONSUM 'C'

using namespace  std;

struct ThreadInfo//线程信息结构体
{
	int    serial;//线程序号
	char   entity;//实体类型
	double delay;//延迟时间
	double persist;//持续时间
};


int  buff_num;//缓冲区大小


CRITICAL_SECTION sem_mutex; //互斥锁

HANDLE sem_full; //满信号量
HANDLE sem_avail; //空信号量


 
void  ProducConsum(char *file);//生产者消费者函数
void  Thread_Producter(void *p);//生产者线程函数
void  Thread_Consumer(void *p);//消费者线程函数
 

int main(int argc, char* argv[])//主函数
{
	ProducConsum("pc_data.txt");
    

	return 0;
}

///////////////////////////////////////////
void ProducConsum(char *file)//生产者消费者函数
{
	DWORD n_thread = 0;  //线程数
	DWORD thread_ID ;	 //线程ID
 
	HANDLE		h_Thread[MAX_THREAD_NUM];//线程句柄数组
	ThreadInfo  thread_info[MAX_THREAD_NUM];//线程信息数组

 
	ifstream inFile;//文件流对象
	inFile.open(file);//打开文件
	puts("Read Data File \n");//读取数据文件

	inFile >> buff_num;//读取缓冲区大小
	while(inFile)
	{
		inFile >>  thread_info[n_thread].serial;//读取线程序号
		inFile >>  thread_info[n_thread].entity;//读取实体类型
		inFile >>  thread_info[n_thread].delay;//读取延迟时间
		inFile >>  thread_info[n_thread].persist;//读取持续时间
		n_thread++;//线程数增加
		inFile.get();//读取换行符
	}


	for(int i=0;i<(int)(n_thread);i++)//创建线程
	{
		if(thread_info[i].entity == PRODUC)
		 
			h_Thread[i] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)( Thread_Producter),//创建生产者线程
			&thread_info[i],0,&thread_ID);
		else
		{
			if(thread_info[i].entity == CONSUM)
		 
			h_Thread[i] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)( Thread_Consumer),//创建消费者线程
			&thread_info[i],0,&thread_ID);

			else
			{
				puts("Bad File\n");
				exit(0);
			}

		}
	}

    printf("Buff  %d\n",buff_num);

 	InitializeCriticalSection(&sem_mutex);//初始化互斥锁
 	sem_full  = CreateSemaphore(NULL,0       ,SEM_MAX_FULL, "sem_full");
 	sem_avail = CreateSemaphore(NULL,buff_num,SEM_MAX_FULL, "sem_avail");
	
  	WaitForMultipleObjects(n_thread,h_Thread,TRUE,-1);//等待所有线程结束

	printf("Task is Finished!\n");
	getch();
}


///////////////////////////////////////////

 
void  Thread_Producter(void *p)//生产者线程函数
{

	DWORD m_delay;			 
	DWORD m_persist;		 
	int		m_serial;	 

	//初始化线程参数
	m_serial = ((ThreadInfo*)(p))->serial;
	m_delay  = (DWORD)(((ThreadInfo*)(p))->delay*INTE_PER_SEC);
	m_persist  = (DWORD)(((ThreadInfo*)(p))->persist*INTE_PER_SEC);
    while (true)
	{
 	   Sleep(m_delay);//生产者线程延迟
 	   printf("P thread %d send the P require\n",m_serial);
 	   WaitForSingleObject(sem_avail,INFINITE); 
	   EnterCriticalSection(&sem_mutex);//进入临界区
 	   printf("P thread %d Begin to P\n",m_serial);
	   Sleep(m_persist);
	   printf("P thread %d Finish P.\n",m_serial);
 	   ReleaseSemaphore(sem_full,1,NULL);//释放满信号量
 	   LeaveCriticalSection(&sem_mutex);//离开临界区
	}
	
}


///////////////////////////////////////////
 
void  Thread_Consumer(void *p)//消费者线程函数
{

	DWORD	m_delay;		 
	DWORD	m_persist;		 
	int		m_serial;		 
 	//初始化线程参数
	m_serial = ((ThreadInfo*)(p))->serial;
	m_delay  = (DWORD)(((ThreadInfo*)(p))->delay*INTE_PER_SEC);
	m_persist  = (DWORD)(((ThreadInfo*)(p))->persist*INTE_PER_SEC);

    while (true)
	{ 
	  Sleep(m_delay);
 	  printf("C thread %d send the C require\n",m_serial);
 	  WaitForSingleObject(sem_full,INFINITE);//等待满信号量
	  EnterCriticalSection(&sem_mutex);//进入临界区
 	  printf("C thread %d Begin to C\n",m_serial);
	  Sleep(m_persist);
	  printf("C thread %d Finish C.\n",m_serial);
 	  ReleaseSemaphore(sem_avail,1,NULL);//释放空信号量
 	  LeaveCriticalSection(&sem_mutex);//离开临界区
    }
}




