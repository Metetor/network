#include<iostream>
#include"server.h"
#include"rdt3.h"
#include"conf.h"
#include"FILE.h"
using namespace std;
int status = 0;
int buildConnectSer(SOCKET& sock, SOCKADDR_IN& addr, packet& p)
{
	struct packet a, b;
	a.set_ack(p);
	a.msgseq = sendseq++;
	sendpak(sock,addr,a);
	//cout << "连接成功\n";
	return 1;
};
int disConnectSer(SOCKET& sock, SOCKADDR_IN& addr,packet &b) {
	//2次挥手
	struct packet a;
	a.set_fin();
	a.set_ack(b);
	sendpak(sock, addr, a);
	return 1;
}
bool stopWaitsend(SOCKET& sock, SOCKADDR_IN& addr, packet& a, packet b)
{
	sendpak(sock, addr,a);
	clock_t start = clock();//计时
	int flag = 0;//
	while (1) {
		recvpak(sock, addr, b);
		if (b.get_ack() && b.acknum == a.msgseq)
		{//收到确认
			return 1;
		}
		clock_t end = clock();
		if (flag == MAX_SEND_TIMES)//重发10次失败
			return 0;
		if ((end - start) / CLOCKS_PER_SEC >= MAX_WAIT_TIME)
		{
			flag++;
			start = clock();//重置计时器
			cout << "[RESEND]  第" << flag << "次重传" << endl;
			sendpak(sock, addr, a);//重传
		}
	}
	return 0;
}
bool stopWaitrecv(SOCKET& sock, SOCKADDR_IN& addr, packet& a, packet b)
{
	int flag = 0;
	while (1)
	{
		recvpak(sock,addr,a);//收到对方发来的消息a
		if (a.get_exist())//因为将recv函数设成了非阻塞，所以需要检测收到的消息是否为空
		{
			int check = a.checkchecksum(sizeof(a));//检测校验和
			if (check)//检测成功
			{
				cout << "[RECV]  收到数据包(checked,checksum="<<a.checksum<<")" << endl;
				b.set_ack(a);//回复对于收到消息a的ack消息b
				b.msgseq = sendseq++;
				cout << "[SEND,ACK]  发送确认包(msgseq=" << b.msgseq << "acknum=" << b.acknum << ")\n";
				sendpak(sock,addr,b);
				memset((char*)&b, 0, sizeof(packet));//防止干扰下一次消息接收
				return 1;
			}
			else
			{
				packet b;
				sendpak(sock, addr, b);
				continue;
			}
		}
	}
	cout << "[RECV,ERROR]  接收失败" << endl;
	return 0;
}
int main() {
	if (!WSAinit())
		printf("[WSA,SUCCESS]  WSAinit Success\n");
	SOCKET sockSer = socket(AF_INET, SOCK_DGRAM, 0);
	u_long imode = 1;
	ioctlsocket(sockSer, FIONBIO, &imode);
	SOCKADDR_IN addrCli;
	addrCli.sin_family = AF_INET;
	addrCli.sin_port = htons(CliPORT);
	addrCli.sin_addr.S_un.S_addr = inet_addr(ADDRSER);
	//绑定套接字
	SOCKADDR_IN addrSer;
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(SerPORT);
	addrSer.sin_addr.S_un.S_addr = inet_addr(ADDRSER);
	bind(sockSer, (SOCKADDR*)&addrSer, sizeof(SOCKADDR));
	cout << "[BIND,SUCCESS] 绑定套接字成功\n";
	while (1)
	{
		packet b;
		recvpak(sockSer,addrCli,b);
		if (!FSM(sockSer,addrCli,b))
			break;
	}
	cout << 1 << endl;
	Sleep(20);
}
int FSM(SOCKET& sock, SOCKADDR_IN& addr, packet& b) {
	if (b.get_syn())
	{
		if (buildConnectSer(sock, addr, b))
		{
			status = 1;//设置状态
			cout << "[CONNECT,SUCCESS]  建立连接成功" << endl;
		}
		else
			cout << "[CONNECT,ERROR]  连接建立失败\n";
	}
	else if (b.get_sf())
	{
		if (status)
		{
			cout << "[FILE,RECV]  开始接收文件...\n";
			recvfile(sock, addr, b);
		}
		else
			cout << "[ERROR]  请先建立连接\n";
	}
	else if (b.get_fin())
	{
		if (disConnectSer(sock, addr,b))
		{
			cout << "[DISCONNECT,SUCCESS]  断开连接" << endl;
			status = 0;
			sendseq = 0;
		}
	}
	return 1;
}