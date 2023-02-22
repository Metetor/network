#ifndef __SERVER_H__
#define __SERVER_H__
#include"conf.h"
#include"pak.h"
#include "FILE.h"
extern SOCKET sockSer;
extern SOCKADDR_IN addrCli;
extern int sendseq;
int buildConnect(packet& b)
{

	packet pak;
	pak.set_flag(ACK);
	pak.set_ack(b.get_seq() + 1);
	pak.set_seq(sendseq++);
	sendpak(sockSer, addrCli, pak);
	return 1;
}
int disConnect(packet& b)
{
	packet pak;
	pak.set_flag(FIN);
	pak.set_flag(ACK);
	pak.set_ack(b.get_seq() + 1);
	sendpak(sockSer, addrCli, pak);
	return 1;
}
int FSM(packet& b)
{
	switch (status) 
	{
	case 0:
		if (b.get_flag(SYN) && b.checkchecksum(sizeof(b)))
		{
			printf("------------------------等待建立连接------------------------\n");
			if (buildConnect(b))
			{
				printf("[CONNECT,SUCCESS] 建立连接成功\n");
				status = 1;
			}
			else
			{
				printf("[ERROR] 建立连接失败");
				status = 0;
			}
		}
		break;
	case 1:
		if (b.get_flag(SF) && b.checkchecksum(sizeof(b)))
		{
			printf("接收到SF\n");
			memset(name, 0, sizeof(name));
			int l=0;
			b.cpdata(name,l);
			packet pak;
			pak.set_flag(ACK);
			pak.set_ack(b.get_seq() + 1);
			pak.set_seq(sendseq++);
			sendpak(sockSer, addrCli, pak);
			status = 2;
		}
		break;
	case 2:
		printf("------------------------等待接收文件内容------------------------\n");
		recvfile();
		status = 3;
		break;
	case 3:
		if (b.get_flag(EF) && b.checkchecksum(sizeof(b)))
		{
			if (disConnect(b))
			{
				printf("[DISCON,SUC] 断开连接成功\n");
				status = 0;
			}
			else
			{
				printf("[DISCON,ERROR] 断开连接失败\n");
				status = 3;
			}
		}
		else
			status = 1;
		break;
	}
	return 1;
}
#endif // !_SERVER_H