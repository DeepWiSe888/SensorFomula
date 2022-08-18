#include "UniversalThread.h"

#include <pthread.h>
#include <unistd.h>

char UniversalThread::stop_flag = 0;

UniversalThread::UniversalThread(void* pid_)
{
    pthread_t_ptr = static_cast<pthread_t *>(pid_);
}
UniversalThread::~UniversalThread()
{

}


int UniversalThread::StopFlag()
{
    return stop_flag;
}

void UniversalThread::join(UniversalThread *ptr)
{

}


int UniversalThread::StopAllThreads()
{
    stop_flag = 1;
    sleep(3);
    return stop_flag;
}



#define   MUTEX_PTR     ((pthread_mutex_t*)mutex)
ULock::ULock()
{
    pthread_mutex_t *p = new pthread_mutex_t;
    pthread_mutex_init(p, 0);
    mutex = p;
}
ULock::~ULock()
{
    pthread_mutex_destroy(MUTEX_PTR);
    mutex = 0;
}

int ULock::lock()
{
    pthread_mutex_lock(MUTEX_PTR);
    return 0;
}
void ULock::unLock()
{
    pthread_mutex_unlock(MUTEX_PTR);
}