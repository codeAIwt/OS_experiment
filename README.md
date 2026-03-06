# 操作系统实验一：进程同步与互斥

本项目包含两个经典的进程同步问题的实现：生产者-消费者问题和读者-写者问题。

## 项目结构

1. `producer_consumer.c`：实现生产者-消费者问题的代码。
2. `reader_writer.c`：实现读者-写者问题的代码。

## 编译与运行

### 生产者-消费者问题
1.	运行结果：
![生产者-消费者问题运行结果](./pic/生产者消费者.png)
 
2.	函数意义及与理论知识的联系：
关键函数与系统调用（与含义）
- CreateThread(...)：在 Windows API 中创建线程。传入线程入口函数和参数。理论上等同于创建一个执行单元并发运行。注意：建议使用 _beginthreadex 而非 CreateThread 当线程会使用 C/C++ 运行时库（printf、new、fstream 等），以避免 CRT 资源初始化/清理问题。
- 线程入口（目前为 void Thread_Producter(void *p) / void Thread_Consumer(void *p)）：程序把它们强制转换为 LPTHREAD_START_ROUTINE 使用，但标准签名应为 DWORD WINAPI ThreadFunc(LPVOID lpParam)。正确签名可保证与 Windows 线程 API ABI/返回值一致并利于调用 ExitThread 返回状态。
- CreateSemaphore(NULL, initialCount, maxCount, name)：创建计数信号量。信号量用于跟踪可用资源数量（这里用来表示 buffer 的“空格/已满”）。若定义了 UNICODE，应传入宽字符名（或使用 NULL 作为未命名信号量）——此前的错误就是因为传入了 narrow 字面量。
- InitializeCriticalSection, EnterCriticalSection, LeaveCriticalSection：实现互斥（临界区保护）。理论上互斥 + 信号量的组合可同时保证“互斥访问缓冲区”和“生产/消费计数约束”。
- WaitForSingleObject(sem, INFINITE), ReleaseSemaphore(...)：等待/释放信号量的操作，分别对应 P (wait) 和 V (signal) 操作（Dijkstra 术语）。
- WaitForMultipleObjects(n_thread, h_Thread, TRUE, -1)：等待所有线程退出。若线程是无限循环的，这会永远阻塞。
- I/O & timing: ifstream 读取配置，Sleep(m_delay) 模拟任务延迟/持续时间，printf/puts/getch 用于控制台输出与用户交互。
与并发理论的对应关系
- 典型 Dijkstra 方案（有界缓冲）：
    - 计数信号量 empty（这里 sem_avail）初始值 = 缓冲区容量（表示可写槽位），生产者在放入前执行 wait(empty)，消费后 signal(empty)。
    - 计数信号量 full（这里 sem_full）初始值 = 0（表示已占用槽位数），生产后 signal(full)，消费前 wait(full)。
    - 一个互斥量（mutex / 临界区）保护对缓冲区的互斥访问。
- 代码实现对应：生产者：Wait(sem_avail) -> EnterCriticalSection -> produce -> ReleaseSemaphore(sem_full) -> LeaveCriticalSection；消费者：相反。这样保证了资源计数与互斥两方面约束（计数保证不会超过边界、互斥保证单次访问的一致性）。


### 读者-写者问题
1.	运行结果：
- 读写互斥：
![读者-写者问题运行结果](./pic/读写互斥.png)
- 写写互斥：
![读者-写者问题运行结果](./pic/写写互斥.png)

2.	函数意义及与理论知识的联系：
关键函数与系统调用（与含义）
- CreateSemaphore(NULL, initialCount, maxCount, name)：创建计数信号量。信号量用于跟踪可用资源数量（这里用来表示 buffer 的“空格/已满”）。若定义了 UNICODE，应传入宽字符名（或使用 NULL 作为未命名信号量）——此前的错误就是因为传入了 narrow 字面量。
- InitializeCriticalSection, EnterCriticalSection, LeaveCriticalSection：实现互斥（临界区保护）。理论上互斥 + 信号量的组合可同时保证“互斥访问缓冲区”和“生产/消费计数约束”。
- WaitForSingleObject(sem, INFINITE), ReleaseSemaphore(...)：等待/释放信号量的操作，分别对应 P (wait) 和 V (signal) 操作（Dijkstra 术语）。

- 核心同步机制
    - `read_count`: 当前读者数量
    - `sem_mutex`: 保护 read_count 变量的互斥锁
    - `sem_w`: 控制写操作的互斥信号量
- 同步规则
    - 当有读者在读时，其他读者可以并发读
    - 当有写者在写时，其他任何线程都不能访问资源
    - 当有读者在读时，写者必须等待
    - 当有写者在写时，新的读者也必须等待
