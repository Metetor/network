#pragma once
#include"rdt3.h"
#include<iostream>
using namespace std;
//#include"init.h"
//为了简化代码，方便后续调用，这里将发送数据包和接收封装为函数
DWORD HostIP, DstIP;
void sendpak(SOCKET &sock,SOCKADDR_IN &addr,packet& a)
{
	a.set_exist();
	a.setchecksum(sizeof(a));//设置校验和
	sendto(sock, (char*)&a, sizeof(packet), 0, (sockaddr*)&addr, sizeof(addr));
}
void recvpak(SOCKET &sock,SOCKADDR_IN &addr,packet& a)
{
	memset(a.data, 0, sizeof(a.data));//清空数据，防止收到之前消息的影响
	int addrlen = sizeof(addr);
	recvfrom(sock, (char*)&a, sizeof(packet), 0, (sockaddr*)&addr,&addrlen);
}
//建立连接
int buildConnectCli(SOCKET& sock, SOCKADDR_IN& addr);
//断开连接
int disConnect(SOCKET& sock, SOCKADDR_IN& addr);
//确认重传
bool stopWaitsend(SOCKET& sock, SOCKADDR_IN& addr,packet& a, packet b);//a写入待发送消息，如果收到b返回的ack检查正确成功

