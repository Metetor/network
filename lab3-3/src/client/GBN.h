#pragma once
#include"conf.h"
#include <time.h>
#include "rdt3.h"
extern int cnt;
extern clock_t start;
extern clock_t end;
DWORD WINAPI ACKHandler(LPVOID param)
{
	SOCKET* clientSock = (SOCKET*)param;
	while (1)
	{
		packet tmp;
		recvpak(*clientSock, addrSer, tmp);
		if (!tmp.get_flag(EXIST))
			continue;
		if (tmp.checkchecksum(sizeof(tmp)))
		{
			if (tmp.get_ack() >= base && tmp.get_ack() <= base + N)
			{
				base = tmp.get_ack();
				if (base == nextseqnum)
				{
					stopTimer = false;
				}
				else
				{
					start = clock();
					stopTimer = true;
					cnt = 0;
				}
			}
			else
			{
				printf("[INVALID] 无效的ACK序列号\n");
				printf("base:%d ack:%d", base, tmp.get_ack());
			}
		}
		else
			printf("[ERROR] 校验和错误\n");
	}
	return 0;
}
int GBNFSM(int pktNum)
{
	DWORD dwThreadId;
	HANDLE ackhandler = CreateThread(nullptr, 0, ACKHandler, LPVOID(&sockCli), 0, &dwThreadId);
	while (1)
	{
		if (base > pktNum)
			break;
		//滑动窗口有空闲，发送包
		if (base <= nextseqnum && nextseqnum < (base + N) && nextseqnum < pktNum)
		{
			sendpak(sockCli, addrSer, paks[nextseqnum]);
			if (base == nextseqnum)
			{
				start = clock();
				stopTimer = true;
				cnt = 0;
			}
			nextseqnum++;
		}
		if (base == pktNum)
		{
			//发送EF包
			packet endpak, pkt;
			endpak.set_flag(EF);
			endpak.set_seq(pktNum);
			if (stopWaitsend(endpak, pkt))
				printf("[EF] 文件发送成功\n");
			CloseHandle(ackhandler);
			return 1;
		}

		//超时重传
		if (stopTimer && (clock() - start) / CLOCKS_PER_SEC > MAX_WAIT_TIME)
		{
			if (cnt >= MAX_SEND_TIMES)
			{
				printf("重传次数过多，文件传输失败\n");
				return 0;
			}
			printf("[TIMEOUT] 开始超时重传\n");
			start = clock();
			stopTimer = true;
			for (int j = base; j < nextseqnum; j++)
			{
				sendpak(sockCli, addrSer, paks[j]);
			}
			cnt++;
		}
	}
	return 0;
}