#ifndef SENSORFOMULA_UNIVERSALTHREAD_H
#define SENSORFOMULA_UNIVERSALTHREAD_H

class UniversalThread
{
private:
    void *  pthread_t_ptr;
public:
    UniversalThread(void* pid);
    ~UniversalThread();
public:
    static int StopFlag();
    static void join(UniversalThread* ptr);
};


class ULock
{
public:
    ULock();
    ~ULock();

public:
    int lock();
    void unLock();
};

#endif