#ifndef _LOCKER_H_  
#define _LOCKER_H_  
  
#include <pthread.h>  
#include <stdio.h>  
#include <semaphore.h>  //信号量的使用主要是用来保护共享资源，使得资源在一个时刻只有一个进程（线程）所拥有。
  
/*信号量的类*/  
class sem_locker  
{  
private:  
    sem_t m_sem;  //信号量的数据类型为结构sem_t，它本质上是一个长整型的数。函数sem_init()用来初始化一个信号量。
  
public:  
    //初始化信号量  
    sem_locker()  
    {  
    if(sem_init(&m_sem, 0, 0) != 0)  
        printf("sem init error\n");  
    }  
    //销毁信号量  
    ~sem_locker()  
    {  
    sem_destroy(&m_sem);  //函数sem_destroy(sem_t *sem)用来释放信号量sem，属于无名信号量。
    }  
  
    //等待信号量  
    bool wait()  
    {  
    return sem_wait(&m_sem) == 0;  //函数sem_wait( sem_t *sem )被用来阻塞当前线程直到信号量sem的值大于0，解除阻塞后将sem的值减一，表明公共资源经使用后减少。
    }  
    //添加信号量  
    bool add()  
    {  
    return sem_post(&m_sem) == 0;  //函数sem_post( sem_t *sem )用来增加信号量的值。当有线程阻塞在这个信号量上时，调用这个函数会使其中的一个线程不在阻塞，选择机制同样是由线程的调度策略决定的。
    }  
};  
  
  
/*互斥 locker*/  
class mutex_locker  
{  
private:  
    pthread_mutex_t m_mutex;  //在多线程应用程序中，当多个线程共享相同的内存时，如同时访问一个变量时，需要确保每个线程看到一致的数据视图，即保证所有线程对数据的修改是一致的。

                               //如下两种情况不存在不一致的问题：

                                //每个线程使用的变量都是其他线程不会读取和修改的
                                //变量是只读的
                                //当一个线程在修改变量的值时，其他线程在读取这个变量时可能会得到一个不一致的值。
                                //一个典型的例子是，在一个多线程程序中，两个及以上个线程对同一个变量i执行i++操作，结果得到的值并不如顺序执行所预期的那样。这就是线程间不同步的一个例子。

                                 //可以用程序修改变量值时所经历的三个步骤解释这个现象：

                                    //从内存单元读入寄存器
                                    //在寄存器中对变量操作（加/减1）
                                    //把新值写回到内存单元
                                 //不能预期以上三步骤在一个总线周期内完成，所以也就不能指望多线程程序如预期那样运行。
								 /*
								 多线程程序中可能会存在数据不一致的情况，那么如何保证数据一致呢？可以考虑同一时间只有一个线程访问数据。互斥量(mutex)就是一把锁。

多个线程只有一把锁一个钥匙，谁上的锁就只有谁能开锁。当一个线程要访问一个共享变量时，先用锁把变量锁住，然后再操作，操作完了之后再释放掉锁，完成。

当另一个线程也要访问这个变量时，发现这个变量被锁住了，无法访问，它就会一直等待，直到锁没了，它再给这个变量上个锁，然后使用，使用完了释放锁，以此进行。

这个即使有多个线程同时访问这个变量，也好象是对这个变量的操作是顺序进行的。*/
public:  
    mutex_locker()  
    {  
        if(pthread_mutex_init(&m_mutex, NULL) != 0)  
        printf("mutex init error!");  
    }  
    ~mutex_locker()  
    {  
    pthread_mutex_destroy(&m_mutex);  
    }  
  
    bool mutex_lock()  //lock mutex  
    {  
    return pthread_mutex_lock(&m_mutex) == 0;  
    }  
    bool mutex_unlock()   //unlock  
    {  
    return pthread_mutex_unlock(&m_mutex) == 0;  
    }  
};  
  
/*条件变量 locker*/  
class cond_locker  
{  
private:  
    pthread_mutex_t m_mutex;  
    pthread_cond_t m_cond;  
  
public:  
    // 初始化 m_mutex and m_cond  
    cond_locker()  
    {  
    if(pthread_mutex_init(&m_mutex, NULL) != 0)  
        printf("mutex init error");  
    if(pthread_cond_init(&m_cond, NULL) != 0)  
    {   //条件变量初始化是被，释放初始化成功的mutex  
        pthread_mutex_destroy(&m_mutex);  
        printf("cond init error");  
    }  
    }  
    // destroy mutex and cond  
    ~cond_locker()  
    {  
    pthread_mutex_destroy(&m_mutex);  
    pthread_cond_destroy(&m_cond);  
    }  
    //等待条件变量  
    bool wait()  
    {  
    int ans = 0;  
    pthread_mutex_lock(&m_mutex);  
    ans = pthread_cond_wait(&m_cond, &m_mutex);  
    pthread_mutex_unlock(&m_mutex);  
    return ans == 0;  
    }  
    //唤醒等待条件变量的线程  
    bool signal()  
    {  
    return pthread_cond_signal(&m_cond) == 0;  
    }  
  
    //唤醒all等待条件变量的线程  
    bool broadcast()  
    {  
            return pthread_cond_broadcast(&m_cond) == 0;  
    }  
};  
  
#endif 