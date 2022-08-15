#include "UniversalThread.h"

#include <pthread.h>

UniversalThread::UniversalThread(void* pid_)
{
    pthread_t_ptr = static_cast<pthread_t *>(pid_);
}
UniversalThread::~UniversalThread()
{

}




int UniversalThread::StopFlag()
{
    return 0;
}

void UniversalThread::join(UniversalThread *ptr)
{

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