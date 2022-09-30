#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include "../../BaseLibs/Utils/UniversalTcp.h"
#include "../../BaseLibs/UniversalData/UniversalData.h"
#include "../../BaseLibs/version.h"


TCPServer tcpServer;
static int s_run_flag = 0;
static int s_listen_port = 0;
static int s_data_channel_id = 0;
static int tmp_fps = 0;
static int tmp_bins = 0;

static PyObject *py_callback_ondata = NULL;

static PyObject *
py_set_callback(PyObject *dummy, PyObject *args)
{
    PyObject *result = NULL;
    PyObject *temp;

    if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
        if (!PyCallable_Check(temp)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(temp);         /* Add a reference to new callback */
        Py_XDECREF(py_callback_ondata);  /* Dispose of previous callback */
        py_callback_ondata = temp;       /* Remember new callback */
        /* Boilerplate to return "None" */
        Py_INCREF(Py_None);
        result = Py_None;
        printf("py call back fun has been set. %x\n", py_callback_ondata);
    }
    return result;
}


int onTCPData(UMatC& m, int channel_id)
{
    int py_size = 0;
    char* py_buf = 0;
    m.Dump(&py_buf, &py_size);
    //printf("recv tcp data:%d", m.getLable()->MatSize());


    // While using callback python in multi-thread, gil state and thread flags is required to be set.
    // This is stupid, and hard to write graceful codes, let's look forwad to Python 4 maybe better.
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    Py_BEGIN_ALLOW_THREADS;
    Py_BLOCK_THREADS;

    PyObject* arglist;
    PyObject* result;
    /// y# -> b'xxxxxxx'
    arglist=Py_BuildValue("(iy#)", channel_id, py_buf, py_size);
    result=PyEval_CallObject(py_callback_ondata,arglist);
    Py_DECREF(arglist);

    Py_UNBLOCK_THREADS;
    Py_END_ALLOW_THREADS;
    PyGILState_Release(gstate);


    delete[] py_buf;
    return 0;
}

void* tcp_server_thread_fun(void* param)
{
    printf("py_tcp_server start, listen port %d\n", s_listen_port);
    tcpServer.Instance(onTCPData, s_listen_port);
    while (s_run_flag)
    {
        sleep(1);
    }
    return 0;
}

int tcp_server(int listen_port)
{
    s_run_flag = 1;
    pthread_t pid;
    pthread_create(&pid, 0, tcp_server_thread_fun, 0);
    return 0;
}

void version()
{
    printf("%s\n", sf_version());
}

void* radar_data_fun(void* param)
{
    int chid = s_data_channel_id ++;
    int fps = tmp_fps;
    int bins = tmp_bins;
    UF rpm_hz = 0.2; // 12/min
    UF bpm_hz = 1.2; // 72/min
    if(bins>138) bins = 138;
    printf("creating simulation data channel, channel id=%d, fps=%d, bins=%d\n",
           chid, fps, bins);

    int sleep_us = 1000*1000/fps;
    UMatC m(DataLabel(RADAR, bins));

    srand(time(0));
    int data_bin_inx = rand()%bins;

    while (s_run_flag)
    {
        for(int x=0;x<bins;x++)
        {
            m.At(x)->i = (1e-5)*(UF)((rand()%1000)-1000);
            m.At(x)->q = (1e-5)*(UF)((rand()%1000)-1000);
            if(x==data_bin_inx)
            {
                m.At(x)->i += (0.05)*sin(2*3.14*rpm_hz*x/fps); // add rpm
                m.At(x)->i += (0.01)*cos(2*3.14*bpm_hz*x/fps); // add bpm
                m.At(x)->q = m.At(x)->q;
            }

        }
        m.updateTimeStamp();
        onTCPData(m, chid);
        usleep(sleep_us);
    }

    return 0;
}

int create_simulate_data_channel(int fps, int bins)
{
    s_run_flag = 1;
    tmp_fps = fps;
    tmp_bins = bins;
    pthread_t pid_radar;
    pthread_create(&pid_radar, 0, radar_data_fun, 0);

    return 0;
}


static PyObject * py_tcp_server(PyObject *self, PyObject *args){
    int port;
    if (!PyArg_ParseTuple(args, "i", &port)) return NULL;
    return PyLong_FromLong(tcp_server(port));
}

static PyObject * py_create_simulate_data_channel(PyObject *self, PyObject *args){
    int fps, bins;
    if (!PyArg_ParseTuple(args, "ii", &fps, &bins)) return NULL;
    return PyLong_FromLong(create_simulate_data_channel(fps, bins));
}


static PyObject * py_version(PyObject *self, PyObject *args){
    version();
    return Py_None;
}

static PyMethodDef Methods[] = {
        {"set_ondata", py_set_callback, METH_VARARGS},
        {"tcp_server", py_tcp_server, METH_VARARGS},
        {"version", py_version, METH_VARARGS},
        {"create_simulate_data_channel", py_create_simulate_data_channel, METH_VARARGS},
        {NULL, NULL}
};

static struct PyModuleDef cModule = {
        PyModuleDef_HEAD_INIT,
        "utils",
        "",
        -1,
        Methods
};


PyMODINIT_FUNC PyInit_sfpy(void){ return PyModule_Create(&cModule);}
//module初始化