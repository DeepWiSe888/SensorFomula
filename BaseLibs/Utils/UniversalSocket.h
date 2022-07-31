//
// Created by link on 2022/6/29.
//

#ifndef SENSORFOMULA_UNIVERSALSOCKET_H
#define SENSORFOMULA_UNIVERSALSOCKET_H

#include <stdint.h>

#define USOCKET_CONNECTED    (1)
#define USOCKET_BINDED       (2)
#define USOCKET_LISTENED     (4)

enum USOCKET_RUNMODE
{
    USOCKET_EMPTY = 0,
    USOCKET_TCP = 1,
    USOCKET_UDP = 2,
    USOCKET_ICMP = 3,
};

class USocket
{
    friend class TCPServer;
    friend class TCPClient;
private:
    int socket_id;
    int socket_status;
    USOCKET_RUNMODE socket_mode;
public:
    USocket();
    USocket(USOCKET_RUNMODE mode);
    USocket(const USocket& copy);
    ~USocket();

public:
    USocket& operator=(const USocket& copy);
    // only TCP mode
    USocket& operator=(int sockfd);

public:
    //int socketID(){return socket_id;}

public:
    bool isValid();
    bool isConnected();

    // socket fd will be released automaticly if join
    void join();

    int closeSocket();

public:
    // --- as server --- //
    int bindAndListen(uint32_t port);
    USocket acceptSocket(uint32_t timeout_ms);

public:
    // --- as client --- //
    int connectServer(char* ip, uint32_t port);

public:
    // --- tcp r/w --- //
    int tcpSend(char* buf, int buflen);
    int tcpRecv(char* buf, int maxbuf, int timeout_ms = 2000);

public:
    // --- udp r/w --- //
    int udpSend(char* ip, uint32_t port, char* buf, int buflen);
    int udpRecv(uint32_t port, char* buf, int maxbuf);

private:
    int reInit(USOCKET_RUNMODE mode);

public:
    static int getSelfIP(char* out);
};


#endif //SENSORFOMULA_UNIVERSALSOCKET_H
