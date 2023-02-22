#include<iostream>
#include<winsock.h>
#include"GlobalVar.h"
#pragma comment(lib,"ws2_32.lib")
void WSAinit();//初始化套接字库

DWORD WINAPI handlerRequest(LPVOID lparam);
using namespace std;
int main() {
	//定义发送缓冲区和接受缓冲区
	SOCKADDR_IN addrSer;//服务端地址
	SOCKADDR_IN addrClient;
	WSAinit();
	//初始化addrSer
	int len = sizeof(SOCKADDR);
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(6666);
	addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	SOCKET sockSer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(sockSer, (SOCKADDR*)&addrSer, sizeof(SOCKADDR));
	listen(sockSer, 5);
	while (1) {
		SOCKET sockConn = accept(sockSer, (SOCKADDR*)&addrClient, &len);
		DWORD dwThreadId;
		HANDLE hThread = CreateThread(NULL, NULL, handlerRequest, LPVOID(sockConn), 0, &dwThreadId);
		CloseHandle(hThread);
	}
	closesocket(sockSer);
	WSACleanup();
}
void WSAinit() {
	WORD version = MAKEWORD(2, 2);
	WSAData wsadata;
	int error;
	error = WSAStartup(version, &wsadata);
	if (error != 0)
	{
		switch (error)
		{
		case WSASYSNOTREADY:
			cout << "WSASYSNOTREADY";
			break;
		case WSAVERNOTSUPPORTED:
			cout << "WSAVERNOTSUPPORTED";
			break;
		case WSAEINPROGRESS:
			cout << "WSAEINPROGRESS";
			break;
		case WSAEPROCLIM:
			cout << "WSAEPROCLIM";
			break;
		}
		return;
	}
	return;
}
DWORD WINAPI handlerRequest(LPVOID lparam) {
	SOCKET CilentSocket = (SOCKET)(LPVOID)lparam;
	send(CilentSocket, sendBuf, strlen(sendBuf), 0);
	recv(CilentSocket, recvBuf, 50, 0);
	closesocket(CilentSocket);
	return 0;
}