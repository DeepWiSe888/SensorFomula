#include "vs.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "alg.h"



static int BINCNT = 0;
static int FPS = DEFAULT_FPS;


static int MAX_WINSECS  = 30;
static int MAX_BUFCNT  = 100*DEFAULT_FPS*30;

static Complex* radar_queue = 0;
static int radar_queue_cnt = 0;

static int vs_win_secs = 20;

#define radarQueue_lock_define  pthread_mutex_t _mutex;
#define radarQueue_lock_init()  pthread_mutex_init(&_mutex, NULL)
#define radarQueue_lock()       pthread_mutex_lock(&_mutex)
#define radarQueue_unlock()     pthread_mutex_unlock(&_mutex)
#define radarQueue_lock_free()  pthread_mutex_destroy(&_mutex)

radarQueue_lock_define


int radarqueue_add(float* fdata, int cnt)
{
    if(BINCNT==0)
        BINCNT = cnt/2;

    radarQueue_lock();
    if( (radar_queue_cnt + BINCNT) > MAX_BUFCNT)
    {

        static int savetick = 1;
        if(savetick==0)
        {
            savetick = 1;
            FILE* fp = fopen("/tmp/link_move_1.dat", "wb");
            if(fp==0)
                printf("fopen error\n");
            fwrite(radar_queue, sizeof(Complex), radar_queue_cnt, fp);
            fclose(fp);
            printf("==================\n");
            printf("==================\n");
            printf("==================\n");
            printf("==================\n");
            printf("==================\n");
            printf("==================\n");
            printf("save done.\n");
            printf("==================\n");
            printf("==================\n");
            printf("==================\n");
            printf("==================\n");
            printf("==================\n");
            fflush(stdout);
        }

        //
        int keep_size = BINCNT*FPS*vs_win_secs;
        if((keep_size+BINCNT)>MAX_BUFCNT)
        {
            radarQueue_unlock();
            printf(" radar queue params invalid! \n");
            return -1;
        }
        memmove(radar_queue, radar_queue+radar_queue_cnt-keep_size, keep_size*sizeof(Complex));
        radar_queue_cnt = keep_size;
    }

    int i;
    for(i=0;i<BINCNT;i++)
    {
        Complex* cur = radar_queue + radar_queue_cnt + i;
        cur->real = fdata[i];
        cur->imag = fdata[i+BINCNT];
    }
    radar_queue_cnt += BINCNT;

    radarQueue_unlock();

    return 0;
}

int radarqueue_getlast(Complex* out, int win_secs)
{
    int get_size = win_secs*BINCNT*FPS;
    if(get_size==0)
    {
        //printf("calc param error.\n");
        return 0;
    }
    if(radar_queue_cnt<get_size)
        return (0);
    radarQueue_lock();
    memcpy(out, radar_queue+radar_queue_cnt-get_size, sizeof(Complex)*get_size);
    radarQueue_unlock();

    return get_size;
}

void vs_calc(Complex* data, int win_secs)
{
    x4result_t ret;
    memset(&ret, 0, sizeof(ret));
    findbreath(data, win_secs*FPS, FPS, BINCNT, &ret );
    printf("exist: %d, move: %d, distance: %.2f \n", ret.exist, ret.fastmove, ret.distance);
    fflush(stdout);
}

void* vs_thread_fun(void* param)
{
    sleep(2);
    time_t last_time = time(0);
    int win_secs = vs_win_secs;
    Complex* buf = (Complex*)malloc(sizeof(Complex)*200*FPS*win_secs);
    while (1)
    {
        time_t cur_time = time(0);
        if(cur_time>last_time)
        {
            fflush(stdout);
            last_time = cur_time;
            //calc vs
            int n = radarqueue_getlast(buf, win_secs);
            if(n<=0)
            {
                usleep(10*1000);
                continue;
            }
            vs_calc(buf, win_secs);
        }
        else
        {
            usleep(10*1000);
        }
        if(cur_time==0)
            break;
    }
    return 0;
}


void vs_init(int fps)
{
    FPS = fps;
    radarQueue_lock_init();

    MAX_BUFCNT  = 100*FPS*MAX_WINSECS; //bincnt * fps * winsecs;
    radar_queue = (Complex*)malloc(sizeof(Complex) * MAX_BUFCNT);
    radar_queue_cnt = 0;

    pthread_t ntid = 0;
    pthread_create(&ntid, 0, vs_thread_fun, 0);

}

void vs_add_frame(float* fdata, int cnt)
{
    radarqueue_add(fdata, cnt);
}
