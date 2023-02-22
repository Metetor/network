#pragma once
#include"conf.h"
#include <time.h>
#include "rdt3.h"
extern clock_t clockstart;
extern clock_t clockend;
extern int constatus;
extern int ssthresh;
extern int cwnd;
extern bool overtime;
extern int pktnum;
void printwindow() {
	printf("窗口大小为:%d\n", cwnd);
}
DWORD WINAPI recvhandler(LPVOID lparam)
{
	printwindow();
	SOCKET* clientSock = (SOCKET*)lparam;
	//cout << "接收" << i << endl;
	clock_t finalovertime;
	int t = 0;
	while (base < pktnum)
	{
		if (nextseqnum > base + cwnd || nextseqnum == base)//窗口已满（对方还没有发送ack）或者窗口为空
			Sleep(2);
		packet pak;
		recvpak(*clientSock,addrSer,pak);
		if (pak.get_flag(ACK)&&pak.checkchecksum(sizeof(pak)))//收到确认消息且序号正确
		{
			clockstart = clock();//重置计时器
			if (pak.get_ack() > base)//收到正确的消息序号
			{
				if (constatus == 0)
				{
					cwnd += pak.acknum - base;//累积确认
					if (cwnd >= ssthresh)
						constatus = 1;//拥塞避免

				}
				else if (constatus == 1)
				{
					cwnd += (pak.acknum- base) / cwnd;//线性增长
				}
				printwindow();
				base = pak.get_ack();
				overtime = 0;
				t = 0;
			}
			else if (pak.get_ack() <= base)
			{
				t++;
				printf("收到第%d个冗余包\n", t);
				printwindow();
				if (constatus == 2) {//快速恢复
					cwnd += 1;
					if (cwnd >= ssthresh) constatus = 1;//进入拥塞避免阶段
				}
				if (t >= 3)//大于等于三个冗余ack
				{
					constatus = 2; //快速重传
					overtime = 1;
					t = 0;
				}
			}
		}
		clockend = clock();
	}
	printf("exitpoint2\n");
	return 1;
}
DWORD WINAPI sendhandler(LPVOID lparam)//发线程
{
	int flag = 0;
	clock_t s = clock();
	SOCKET* clientSock = (SOCKET*)lparam;
	while (base < pktnum)
	{
		if (!overtime)
		{
			int temp = base;
			if (base + cwnd == nextseqnum)
				Sleep(40);
			for (; nextseqnum < base + cwnd && nextseqnum < pktnum; nextseqnum++)
			{
				flag = 0;//正常发送

				if (!overtime)
				{
					sendpak(*clientSock, addrSer, paks[nextseqnum]);
					s = clock();
					//Sleep(20);
				}

				else {
					break;
				}
				sendpak(*clientSock, addrSer, paks[nextseqnum]);
				if (temp == base)
				{
					clock_t e = clock();
					if ((e - s) / CLOCKS_PER_SEC >= MAX_WAIT_TIME)//超时重发
					{
						overtime = 1;
						nextseqnum++;
						break;
					}
				}
			}
		}
		if (overtime == 1)//重新发送
		{
			if (constatus == 1 || constatus == 0)
			{
				ssthresh = cwnd / 2;
				cwnd = 1;
				constatus = 0;//重新进入慢启动阶段
				printwindow();
			}
			else if (constatus == 2)
			{//快速重传
				printf("快速重传\n");
				ssthresh = cwnd / 2;
				cwnd = ssthresh + 3;
			}
			if (flag)
				Sleep(10);//减少重传次数
			for (int i = base; i <nextseqnum; i++)
			{
				//重新发送
				if (overtime)
					sendpak(*clientSock, addrSer, paks[i]);
				else break;
				flag++;
			}
			overtime = 0;//重传标识归0
			s = clock();//重置计时器
		}
		clock_t e = clock();
		if ((e - s) / CLOCKS_PER_SEC >=MAX_WAIT_TIME)//超时重发
			overtime = 1;
	}
	printf("exitpoint1\n");
	return 1;
}