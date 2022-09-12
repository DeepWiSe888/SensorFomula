#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <pthread.h>
#include <unistd.h>
#include "../../BaseLibs/Utils/UniversalTcp.h"
#include "../../BaseLibs/UniversalData/UniversalData.h"
#include "../../BaseLibs/version.h"


TCPServer tcpServer;
static int s_run_flag = 0;
static int s_listen_port = 0;

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
        printf("py call back fun has been set.\n");
    }
    return result;
}


int onTCPData(UMatC& m)
{
    int py_size = 0;
    char* py_buf = 0;
    m.Dump(&py_buf, &py_size);
    //printf("recv tcp data:%d", m.getLable()->MatSize());

    PyObject* arglist;
    PyObject* result;

    /// y# -> b'xxxxxxx'
    arglist=Py_BuildValue("(y#)", py_buf, py_size);
    result=PyEval_CallObject(py_callback_ondata,arglist);
    Py_DECREF(arglist);

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

    /*
    /// test ///

    if(py_callback_ondata==0)
        return;

    printf("test call back function.\n");
    int arg;
    PyObject* arglist;
    PyObject* result;

    char* sss = "hello";

    arglist=Py_BuildValue("(y#)", sss, 5);
    result=PyEval_CallObject(py_callback_ondata,arglist);
    Py_DECREF(arglist);
     */
}


static PyObject * py_tcp_server(PyObject *self, PyObject *args){
    int port;
    if (!PyArg_ParseTuple(args, "i", &port)) return NULL;
    return PyLong_FromLong(tcp_server(port));
}

static PyObject * py_version(PyObject *self, PyObject *args){
    version();
    return Py_None;
}

static PyMethodDef Methods[] = {
        {"set_ondata", py_set_callback, METH_VARARGS},
        {"tcp_server", py_tcp_server, METH_VARARGS},
        {"version", py_version, METH_VARARGS},
        {NULL, NULL}
};

static struct PyModuleDef cModule = {
        PyModuleDef_HEAD_INIT,
        "sfpy",
        "",
        -1,
        Methods
};


PyMODINIT_FUNC PyInit_sfpy(void){ return PyModule_Create(&cModule);}
//module初始化