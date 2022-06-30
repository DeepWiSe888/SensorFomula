//
// Created by link on 2022/6/29.
//

#ifndef SENSORFOMULA_UNIVERSALSOCKET_H
#define SENSORFOMULA_UNIVERSALSOCKET_H

#include <stdint.h>

class USocket
{
private:
    int socket_id;
public:
    USocket();
    ~USocket();

public:
    bool isValid();
    bool isConnected();

    void join();
    int close();

public:
    // --- as server --- //
    int bindAndListen(uint32_t port);
    USocket accept(uint32_t timeout_ms);

public:
    // --- as client --- //
    int connect(char* ip, uint32_t port);

};


#endif //SENSORFOMULA_UNIVERSALSOCKET_H
