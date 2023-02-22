#include <stdio.h>
#include<winsock.h>
#include"pak.h"
#include"init.h"
#pragma comment(lib,"Ws2_32.lib")
int WSAinit() 
{
	WORD version = MAKEWORD(2, 2);
	WSAData wsadata;
	int error;
	error = WSAStartup(version, &wsadata);
	if (error != 0)
	{
		switch (error)
		{
		case WSASYSNOTREADY:
			printf("WSASYSNOTREADY");
			break;
		case WSAVERNOTSUPPORTED:
			printf("WSAVERNOTSUPPORTED");
			break;
		case WSAEINPROGRESS:
			printf("WSAEINPROGRESS");
			break;
		case WSAEPROCLIM:
			printf("WSAEPROCLIM");
			break;
		}
		return -1;
	}
	return 0;
}

void sock_init()
{
	extern SOCKET sockCli;
	extern SOCKADDR_IN addrCli, addrSer;
	sockCli = socket(AF_INET, SOCK_DGRAM, 0);
	//设置套接字非阻塞
	u_long imode = 1;
	ioctlsocket(sockCli, FIONBIO, &imode);
	printf("请选择发送目标: [1]server端  [2]路由器\n");
	int x = 0;
	scanf_s("%d", &x);
	if (x == 1)
	{
		addrSer.sin_family = AF_INET;
		addrSer.sin_port = htons(33333);
		addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.3");
	}
	else if (x == 2)
	{
		addrSer.sin_family = AF_INET;
		addrSer.sin_port = htons(22222);
		addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.2");
	}
	addrCli.sin_family = AF_INET;
	addrCli.sin_port = htons(11111);
	addrCli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (bind(sockCli, (SOCKADDR*)&addrCli, sizeof(SOCKADDR)) == -1)
		printf( "client bind error\n");
	printf("[BIND,SUCCESS]  client bind success\n");
}

void init() {
	if (!WSAinit())
		printf("[WSA,SUCCESS]  WSAinit Success\n");

	sock_init();
}