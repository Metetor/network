#include"rdt3.h"
#include<ctime>
#include <stdio.h>
bool stopWaitsend(packet &a, packet &b)
{
	extern SOCKET sockCli;
	extern SOCKADDR_IN addrSer;
	sendpak(sockCli, addrSer, a);
	clock_t start = clock();//计时
	int flag = 0;//
	while (1) {
		recvpak(sockCli, addrSer, b);
		if (b.get_flag(ACK)&&b.get_ack()==a.get_seq()+1&&b.checkchecksum(sizeof(b)))
		{//收到确认
			//printf("[RECV,ACK]  收到确认包(ack)\n");
			return 1;
		}
		clock_t end = clock();
		if (flag == MAX_SEND_TIMES)//重发10次失败
			return 0;
		if ((end - start) / CLOCKS_PER_SEC >= MAX_WAIT_TIME)
		{
			flag++;
			start = clock();//重置计时器
			//cout << "第" << flag << "次重传" << endl;
			printf("第%d次重传\n",flag);
			sendpak(sockCli, addrSer, a);//重传
		}
	}
	return 0;
}