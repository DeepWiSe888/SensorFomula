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