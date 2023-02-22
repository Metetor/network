#include<iostream>
#include<winsock.h>
#pragma comment(lib,"ws2_32.lib")
int main() {
	WSAStartup(wVersionRequested,&wsaData);
    SOCKET sockClient=socket(AF_INET,SOCK_STREAM,0);

    connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

    recv(sockClient,recvBuf,50,0);

    send(sockClient,"hello",sendlen,0);

    closesocket(sockClient);
    WSACleanup();
}