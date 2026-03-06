// scheduling.h
struct pcb
{
	char name[10];
	char state;
	int nice;
	int atime; // arrival time
	int ntime;
	int rtime;
	struct pcb * link;//link为下一个进程的指针
};
typedef struct pcb PCB;