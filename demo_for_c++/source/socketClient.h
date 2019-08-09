#ifndef _SOCKET_CLIENT_H_
#define _SOCKET_CLIENT_H_

//common
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if defined(_MSC_VER)//win
#include <WinSock2.h>
#include "time.h"
#define snprintf sprintf_s
#define sleep Sleep
#else//linux
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#endif

class SocketClient{
public:

#if defined(_MSC_VER)//win
    typedef SOCKET NativeSocket;
#else
    typedef int NativeSocket;
#endif
    typedef sockaddr_in NativeAddress;

    SocketClient(unsigned long serverIp, unsigned short serverPort);
    ~SocketClient();

    int Connect();
    void Close();
    int Send(const char *__buf, size_t __n, int __flags);
    int Recv(char *__buf, size_t __n, int __flags);



   // variable

   NativeAddress addrSrv_;
   NativeSocket socket_;
   //unsigned long serverIp_ ;
   //unsigned short serverPort_;
};

#endif //_SOCKET_CLIENT_H_
