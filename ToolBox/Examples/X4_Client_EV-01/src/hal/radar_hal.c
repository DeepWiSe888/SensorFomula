#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <poll.h>
#include <signal.h>

#include "radar_hal.h"
#include "xep_hal.h"




#pragma message "PX30 platform defined, using GPIO3_C0 as ENABLE pin, GPIO3_C1 as INTERRUPT pin"
/******* PX30 ********/
// EANBLE ==> GPIO3_C0 = 32*3 + 8 + 8 + 0 = 112 || GPIO3_D2 96+8+8+8+2 = 122 ||
// INT    ==> GPIO3_C1 = 32*3 + 8 + 8 + 1 = 113 || GPIO3_D3 96+8+8+8+3 = 123 ||
// Enable ==> GPIO3_C3 = 32*3 + 8 + 8 + 3 = 115
// Int    ==> GPIO3_A5 = 32*3 + 5 = 101
#define X4_ENABLE_PIN 115
#define X4_INTR_PIN   101
#define X4_SPI_DEV    "/dev/spidev1.0"
#define X4_SPI_SPEED  48000000



static int _gpio_set(char const *fn, char *val, int len) {
    FILE* fp = fopen(fn, "wb");
    if (NULL == fp) {
        //fprintf(stderr, "Open file %s fail, error: %s", fn, strerror(errno));
        return -1;
    }
    fwrite(val, 1, len, fp);
    fflush(fp);
    fclose(fp);
    return 0;
}
static int gpio_export(int pin) {
    char val[8];
    int len = sprintf(val, "%d", pin);
    return _gpio_set("/sys/class/gpio/export", val, len);
}
static int gpio_unexport(int pin) {
    char val[8];
    int len = sprintf(val, "%d", pin);
    return _gpio_set("/sys/class/gpio/unexport", val, len);
}
static int gpio_set_direction(int pin, int direction) {
    char val[8] = {0};
    int len = 0;
    if (GPIO_IN == direction) {
        len = sprintf(val, "in");
    }
    else if (GPIO_OUT == direction) {
        len = sprintf(val, "out");
    }
    else {
        return -1;
    }
    char fn[256];
    sprintf(fn, "/sys/class/gpio/gpio%d/direction", pin);
    return _gpio_set(fn, val, len);
}
static int gpio_set_value(int pin, int _val) {
    char val[8] = {0};
    int len = 0;
    if (GPIO_LOW == _val) {
        len = sprintf(val, "0");
    }
    else if (GPIO_HIGH == _val) {
        len = sprintf(val, "1");
    }
    else {
        return -1;
    }
    char fn[256];
    sprintf(fn, "/sys/class/gpio/gpio%d/value", pin);
    return _gpio_set(fn, val, len);
}
static int gpio_set_edge(int pin, int trigger) {
    char const* _isr_cmds[] = { "none", "rising", "falling", "both" };
    if (trigger < 0 || trigger > 4) {
        //fprintf(stderr, "Unkown trigger type");
        return -1;
    }
    char val[8];
    strcpy(val,_isr_cmds[trigger]);
    int len = strlen(val);
    char fn[256];
    sprintf(fn, "/sys/class/gpio/gpio%d/edge", pin);
    return _gpio_set(fn, val, len);
}
static int gpio_set_activelow(int pin, int low) {
    char val[2];
    val[1] = '\0';
    if (low) {
        val[0] = '1';
    }
    else
        val[0] = '0';
    char fn[256];
    sprintf(fn, "/sys/class/gpio/gpio%d/active_low", pin);
    return _gpio_set(fn, val, 1);
}

/**
 * SPI operations
 * @param fd
 * @param speed
 * @return SPI node describe
 */
static int spi_set_speed(int fd, int speed) {
    if (-1 == fd) {
        return -1;
    }
    uint32_t speed_u32 = speed;
    /* set datarate */
    int ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed_u32);
    if (ret < 0)
    {
        printf("Error Set Speed\n");
        return -6;
    }

    // /* check datarate */
    int r_spd = 0;
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &r_spd);
    if (ret < 0 || speed != r_spd)
    {
        printf("Error verify Speed\n");
        return -7;
    }

    return 0;
}
static int spi_set_mode(int fd, uint8_t mode) {
    if (-1 == fd) {
        return -1;
    }
    /* set mode */
    int ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret < 0)
    {
        perror("Error Set SPI-Mode");
        return -2;
    }

    /* check mode
    int r_m = 0;
    ret = ioctl(fd, SPI_IOC_RD_MODE, &r_m);
    if (ret < 0 || mode != r_m)
    {
        perror("Error Get SPI-Mode");
        return -3;
    }*/

    return 0;
}
static int spi_set_bpw(int fd, int bpw) {
    if (-1 == fd) {
        return -1;
    }
    /* set word length*/
    int ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bpw);
    if (ret < 0)
    {
        perror("Error Set wordlength");
        return -4;
    }

    // /* check word length */
    int r_bpw = 0;
    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &r_bpw);
    if (ret < 0 || bpw != r_bpw)
    {
        perror("Error Get wordlength");
        return -5;
    }

    return 0;
}

static int spi_fd = -1;
int SPI_open()
{
    if (spi_fd >= 0) return spi_fd;

    int fd = open(X4_SPI_DEV, O_RDWR);
    if (fd < 0) {
        //fprintf(stderr, "Open %s, error: %s", X4_SPI_DEV, strerror(errno));
        return -1;
    }
    if (spi_set_mode(fd, SPI_MODE_0)) {
        //fprintf(stderr, "SPI set data mode, error: %s", strerror(errno));
        close(fd);
        return -1;
    }
    if (spi_set_speed(fd, X4_SPI_SPEED)) {
        printf("SPI set data rate, error: %s", strerror(errno));
        close(fd);
        return -1;
    }
    if (spi_set_bpw(fd, 8)) {
        //fprintf(stderr, "SPI set data bits per word, error: %s", strerror(errno));
        close(fd);
        return -1;
    }

    //fprintf(stderr, "open " X4_SPI_DEV " = %d", fd);

    return fd;
}

#include <x4driver.h>
int SPI_transfer(int spi_fd, uint8_t* wdata, uint32_t wlen, uint8_t* rdata) {
    if (spi_fd < 0)  {
        //fprintf(stderr, "SPI device is not open.");
        return -1;
    }

    //clear memory
    memset(rdata, 0, wlen);

    struct spi_ioc_transfer spi ;
    memset(&spi, 0, sizeof(spi));
    spi.tx_buf        = (unsigned long)wdata;
    spi.rx_buf        = (unsigned long)rdata;
    spi.len           = wlen;
    spi.delay_usecs   = 0;
    //spi.speed_hz      = X4_SPI_SPEED;
    //spi.bits_per_word = 8;
    spi.cs_change     = 0;

    // lets go
    //if(wlen>100)
    //    x4driver_tic();
    int ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi);
    //if(wlen>100)
    //    x4driver_toc();
    if (ret < 0)
    {
        printf("spi error[%d].\n", errno);
        switch (errno) {
            case EBADF:
                //fprintf(stderr, "ERROR: fd is not a valid file descriptor.");
                break;
            case EFAULT:
                //fprintf(stderr, "ERROR: argp references an inaccessible memory area");
                break;
            case EINVAL:
                //fprintf(stderr, "ERROR: request or argp is not valid");
                break;
            case ENOTTY:
                //fprintf(stderr, "ERROR: fd is not associated with a character special device");
                break;
            default: // Perhaps this should be default:
                //fprintf(stderr, "ERROR: The specified request does not apply to the kind of object that the file descriptor fd references.");
                break;
        }
        return -1;
    }

    return 0;
}

void SPI_close()
{
    close(spi_fd);
    spi_fd = -1;
}

int radar_hal_init(radar_handle_t* handler)
{
    if (NULL == handler) {
        return -1;
    }
    //fprintf(stderr, "radar_hal_init\n");
    int status = XT_SUCCESS;

    //Enable
    gpio_export(X4_ENABLE_PIN);
    gpio_export(X4_INTR_PIN);
    usleep(100000);
    gpio_set_direction(X4_ENABLE_PIN, GPIO_OUT);
    gpio_set_direction(X4_INTR_PIN, GPIO_IN);

    gpio_set_value(X4_ENABLE_PIN, 0);
    gpio_set_value(X4_ENABLE_PIN, 1);
    // 10ms，等待雷达就绪
    usleep(10000);

    handler->spi_handle = SPI_open();

    return status;
}

void radar_hal_deinit(radar_handle_t* radar_handle)
{
    close(radar_handle->spi_handle);
    radar_handle->spi_handle = -1;
    gpio_unexport(X4_INTR_PIN);
    gpio_set_direction(X4_INTR_PIN, GPIO_OUT);
}

int radar_hal_get_instance_size(void)
{
    int size = sizeof(radar_handle_t);
    return size;
}

int radar_hal_pin_set_enable(radar_handle_t* radar_handle, int value)
{
    (void)radar_handle;

    if (value == 1)
        gpio_set_value(X4_ENABLE_PIN, GPIO_HIGH);
    else if (value == 0)
        gpio_set_value(X4_ENABLE_PIN, GPIO_LOW);
    else
        return XT_ERROR;

    return XT_SUCCESS;
}


int radar_hal_spi_write_read(radar_handle_t* radar_handle, uint8_t* wdata, uint32_t wlen, uint8_t* rdata, uint32_t rlen)
{
    if ((0 == wlen) && (0 == rlen)) {
        return XT_SUCCESS;
    }

    uint8_t tx[2048];//tx[wlen+rlen];
    uint8_t rx[2048];//rx[wlen+rlen];

    //x4driver_tic();
    if (wdata) memcpy(tx, wdata, wlen);
    SPI_transfer(radar_handle->spi_handle, tx, wlen+rlen, rx);
    if (rdata) memcpy(rdata, rx+wlen, rlen);
    //x4driver_toc();

    return XT_SUCCESS;
}

int radar_hal_spi_write(radar_handle_t* radar_handle, uint8_t* data, uint32_t length)
{
    if (0 == length) {
        return XT_SUCCESS;
    }
    if (NULL == data) {
        return XT_ERROR;
    }

    uint8_t rx[length];
    SPI_transfer(radar_handle->spi_handle, data, length, rx);
    return XT_SUCCESS;
}

int radar_hal_spi_read(radar_handle_t* radar_handle, uint8_t* data, uint32_t length)
{
    if (0 == length) {
        return XT_SUCCESS;
    }
    if (NULL == data) {
        return XT_ERROR;
    }
    uint8_t tx[length];
    SPI_transfer(radar_handle->spi_handle, tx, length, data);

    return XT_SUCCESS;
}

void* _isr_trigger(void* args) {

    radar_isrcb* cbs = (radar_isrcb*)args;
    char fn[256];
    sprintf(fn, "/sys/class/gpio/gpio%d/value", X4_INTR_PIN);
    int fd = open(fn, O_RDONLY);
    uint32_t info = 1; /* unmask */
    read(fd, &info, sizeof(info));

    while (cbs->radar_handle->running) {

        if (fd < 0) {
            fd = open(fn, O_RDONLY);
        }
        struct pollfd fds;
        fds.fd = fd;
        fds.events = POLLPRI;
        int ret = poll(&fds, 1, -1);
        if (ret >= 1 && (fds.revents & POLLPRI)) {
            info = 0;
            lseek(fd, SEEK_SET, 0);
            int nb = read(fd, &info, sizeof(info));
            if (nb && info && cbs && cbs->data_ready_cb) {
                if (cbs->data_ready_cb(cbs->args)) {
                    break;
                }
            }
        }
        else {
            //fprintf(stderr, "poll error: %s", strerror(errno));
            close(fd);
            fd = -1;
        }
    }
    // 回收资源
    close(fd);
    radar_hal_deinit(cbs->radar_handle);
    if (cbs->resycle)
        cbs->resycle(cbs->args);

    return NULL;
}
pthread_t radar_hal_enable_isr(radar_handle_t* radar_handle, radar_isrcb* cbs)
{
    (void)radar_handle;
    radar_handle->running = 1;
    cbs->radar_handle = radar_handle;
    gpio_set_activelow(X4_INTR_PIN, 0);
    gpio_set_direction(X4_INTR_PIN, GPIO_IN);
    gpio_set_edge(X4_INTR_PIN, GPIO_ISR_RISING);

    // Signal
    sigset_t sigpipe_mask;
    sigemptyset(&sigpipe_mask);
    sigaddset(&sigpipe_mask, SIGPIPE);
    sigset_t saved_mask;
    pthread_sigmask(SIG_BLOCK, &sigpipe_mask, &saved_mask);
    // 设置优先级
    pthread_t pid;
    pthread_attr_t attr;
    struct sched_param sched;
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    sched.__sched_priority = sched_get_priority_max(SCHED_RR);
    pthread_attr_setschedparam(&attr, &sched);

    //pthread_create(&pid, &attr, _isr_trigger, cbs);
    pthread_create(&pid, 0, _isr_trigger, cbs);
    return pid;
}

int radar_hal_disable_isr(radar_handle_t* radar_handle, void* args)
{
    (void)radar_handle; (void) args;
    radar_handle->running = 0;

    return XT_SUCCESS;
}

