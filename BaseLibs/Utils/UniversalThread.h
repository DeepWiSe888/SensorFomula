#ifndef SENSORFOMULA_UNIVERSALTHREAD_H
#define SENSORFOMULA_UNIVERSALTHREAD_H

class UniversalThread
{
private:
    void *  pthread_t_ptr;
    static char stop_flag;
public:
    UniversalThread(void* pid);
    ~UniversalThread();
public:
    static int StopFlag();
    static void join(UniversalThread* ptr);
    static int StopAllThreads();
};


class ULock
{
protected:
    void* mutex;
public:
    ULock();
    ~ULock();

public:
    int lock();
    void unLock();
};

#endif