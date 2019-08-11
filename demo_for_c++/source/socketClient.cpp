#include "socketClient.h"

SocketClient::SocketClient(unsigned long serverIp, unsigned short serverPort)
{

    socket_ = socket(AF_INET,SOCK_STREAM,0);

    memset(&addrSrv_, 0, sizeof(addrSrv_));
    addrSrv_.sin_addr.s_addr = serverIp;
    addrSrv_.sin_family = AF_INET;
    addrSrv_.sin_port = htons(serverPort);

}


SocketClient::~SocketClient(){

}

int SocketClient::Connect(){
    return connect(socket_,(sockaddr*)&addrSrv_,sizeof(sockaddr));
}

int SocketClient::Send(const char *__buf, size_t __n, int __flags){
    return send(socket_,__buf, __n, __flags);

}

int SocketClient::Recv(char *__buf, size_t __n, int __flags){
   return recv(socket_,__buf, __n, __flags);
}

void SocketClient::Close(){
#if defined(_MSC_VER)//win
    shutdown(socket_, SD_BOTH);
    closesocket(socket_);
    WSACleanup();
#else
    close(socket_);
#endif
}
