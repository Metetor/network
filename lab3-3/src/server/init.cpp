#include <stdio.h>
#include<winsock.h>
#include"pak.h"
#include"init.h"
#include"server.h"
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
	extern SOCKET sockSer;
	extern SOCKADDR_IN addrSer;
	//ÉèÖÃÌ×½Ó×Ö·Ç×èÈû
	u_long imode = 1;
	sockSer = socket(AF_INET, SOCK_DGRAM, 0);
	ioctlsocket(sockSer, FIONBIO, &imode);


	addrCli.sin_family = AF_INET;
	addrCli.sin_port = htons(11111);
	addrCli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(33333);
	addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.3");

	if (bind(sockSer, (SOCKADDR*)&addrSer, sizeof(SOCKADDR)) == -1)
		printf("server bind error\n");
	else
		printf("[BIND,SUCCESS]  server bind success\n");
	return;
}

void init() {
	if (!WSAinit())
		printf("[WSA,SUCCESS]  WSAinit Success\n");

	sock_init();
}