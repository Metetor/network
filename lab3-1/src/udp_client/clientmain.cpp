#include<iostream>
#include<ctime>
#include"conf.h"
#include"rdt3.h"
#include"client.h"
#include "FILE.h"
using namespace std;
int buildConnectCli(SOCKET& sock, SOCKADDR_IN& addr) {
	/*
	客户端发起连接
	*/
	packet a, b;
	a.set_syn();
	//设置发送序列号
	a.msgseq = sendseq++;
	//cout << "设置序列号.\n";
	if (stopWaitsend(sock,addr,a, b))
		return 1;//停等发送机制，发送消息a,发生超时事件时重传，如果收到了对应的ack则b返回1,超过最大重传次数返回0
	else
		return 0;
};
bool stopWaitsend(SOCKET& sock, SOCKADDR_IN& addr,packet& a, packet b)
{
	//cout << "发送" << endl;
	sendpak(sock,addr,a);
	cout << "[SEND]  发送包(msgseq="<<a.msgseq<<"acknum="<<a.acknum<<")\n";
	clock_t start = clock();//计时
	int flag = 0;//
	while (1) {
		recvpak(sock, addr, b);
		//cout << "recv\n";
		//cout << "b.acknum" << b.acknum << "a.msgseq" << a.msgseq << endl;
		
		if (b.get_ack()&&b.acknum == a.msgseq&&b.checkchecksum(sizeof(b)))
		{//收到确认
			cout << "[RECV,ACK]  收到确认包(ack)\n";
			return 1;
		}
		clock_t end = clock();
		if (flag == MAX_SEND_TIMES)//重发10次失败
			return 0;
		if ((end - start) / CLOCKS_PER_SEC >= MAX_WAIT_TIME)
		{
			flag++;
			start = clock();//重置计时器
			cout << "第" << flag << "次重传" << endl;
			sendpak(sock, addr, a);//重传
		}
	}
	return 0;
}
int disConnect(SOCKET& sock, SOCKADDR_IN& addr) {
//2次挥手
	packet a, b;
	a.set_fin();
	if (stopWaitsend(sock, addr, a, b))
		return 1;
	else
		return 0;
}
int main() {
	if(!WSAinit())
		printf("[WSA,SUCCESS]  WSAinit Success\n");
	SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);
	//设置套接字非阻塞
	u_long imode = 1;
	ioctlsocket(sockClient, FIONBIO, &imode);
	//string ADDRSER;
	//cout << "请输入服务器的地址:";
	//getline(cin, ADDRSER);
	DstIP = inet_addr("127.0.0.1");
	HostIP = inet_addr("127.0.0.1");
	SOCKADDR_IN addrSer;
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(SerPORT);
	//addrSer.sin_addr.S_un.S_addr = inet_addr(ADDRSER.c_str());
	addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	SOCKADDR_IN addrCli;
	addrCli.sin_family = AF_INET;
	addrCli.sin_port = htons(CliPORT);
	//addrSer.sin_addr.S_un.S_addr = inet_addr(ADDRSER.c_str());
	addrCli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (bind(sockClient, (SOCKADDR*)&addrCli, sizeof(SOCKADDR)) == -1)
		cout << "client bind error\n";
	cout << "[BIND,SUCCESS]  client bind success\n";
	if (buildConnectCli(sockClient, addrSer))
	{
		cout << "[CONNECT,SUCESS]  建立连接成功...\n";
	}
	else
		cout << "[CONNECT,ERROR]  连接建立失败\n";
	int op;
	cout << "[OP]  请输入操作:1.发送文件  2.exit\n";
	cin >> op;
	//sendpak(sockClient, addrSer, p);
	/*消息处理（接收和发送）*/
	//sendto(sockClient,"hello", 6, 0, (sockaddr*)&addrSer, sizeof(addrSer));
	while (1)
	{
		if (op == 1) {
			//输入文件名
			char name[50];
			cout << "[FILE,INPUT]  请输入文件名:";
			cin >> name;
			if (sendfile(sockClient, addrSer, name))
			{
				cout << "[FILE,SUCCESS]  文件发送成功\n" << "请输入操作:1.发送文件  2.exit\n";
				cin >> op;
				if (op == 2)
					break;
			}
			else
				cout << "[FILE,ERROR]  文件发送失败\n";
		}
		if (op == 2)
			break;
	}
	if (disConnect(sockClient, addrSer))
	{
		cout << "[DISCONNECT,SUCCESS]  连接已断开\n";
		sendseq = 0;
	}
	else
		cout << "[DISCONNECT,ERROR]  连接断开失败\n";
	sendseq = 0;
	system("pause");
	return 0;
}