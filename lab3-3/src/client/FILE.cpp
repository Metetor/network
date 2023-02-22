#include"FILE.h"
#include"conf.h"
#include "pak.h"
#include"rdt3.h"
#include"GBN.h"
#include"RENO.h"
extern int cnt;
extern clock_t start;
extern clock_t end;
void readfile(char* name)
{
	extern int pktnum;
	int length=0;
	ifstream in(name, ifstream::binary);
	if (!in)
	{
		printf("文件读取失败\n");
		return;
	}
	char line = in.get();
	while (in)
	{
		sendBuffer[pktnum][length % 1024] = line;
		length++;
		if (length % 1024 == 0)
		{
			pktnum++;
			length = 0;
		}
		line = in.get();
	}
	printf("文件读取到缓冲区\n");
	in.close();
	for (int i = 0; i <=pktnum; i++)
	{
		packet tmp;
		if (i == pktnum)
		{
			printf("len，%d", length);
			tmp.cpdata(sendBuffer[i], length);
		}
		else
			tmp.cpdata(sendBuffer[i],1024);
		tmp.set_flag(TF);
		tmp.set_seq(i);
		paks.push_back(tmp);
	}
	printf("该文件预计将要发送%d个数据包\n", pktnum);
}
int sendfile(char* name)
{
	//读取文件
	printf("读取文件中...\n");
	readfile(name);
	clock_t tmstart = clock();
	//首先发送SF包
	printf("[SEND,SF] 发送SF包");
	packet startpak,rpak;
	startpak.set_flag(SF);
	startpak.cpdata(name,sizeof(name));
	startpak.set_seq(sendseq++);
	stopWaitsend(startpak,rpak);

	//传输文件内容
	pktnum = paks.size();
	printf("pktnum:%d", pktnum);
	printf("[SEND,TF] 发送TF包\n");
	DWORD dwThreadId1;
	HANDLE sendhand = CreateThread(nullptr, 0, sendhandler, LPVOID(&sockCli), 0, &dwThreadId1);
	DWORD dwThreadId2;
	HANDLE recvhand = CreateThread(nullptr, 0, recvhandler, LPVOID(&sockCli), 0, &dwThreadId2);
	while (1) {

		if (base == pktnum)
		{
			//发送EF包
			packet endpak, pkt;
			endpak.set_flag(EF);
			endpak.set_seq(nextseqnum);
			if (stopWaitsend(endpak, pkt))
				printf("[EF] 文件发送成功\n");
			CloseHandle(recvhand);
			break;
		}
	}
	//if (GBNFSM(pktNum));
	//DWORD dwThreadId;
	//HANDLE ackhandler = CreateThread(nullptr, 0, ACKHandler, LPVOID(&sockCli), 0, &dwThreadId);
	//while (1)
	//{
	//	if (base > pktNum)
	//		break;
	//	//滑动窗口有空闲，发送包
	//	if (base <= nextseqnum && nextseqnum < (base + N) && nextseqnum < pktNum)
	//	{
	//		sendpak(sockCli,addrSer,paks[nextseqnum]);
	//		if (base == nextseqnum)
	//		{
	//			start = clock();
	//			stopTimer = true;
	//			cnt = 0;
	//		}
	//		nextseqnum++;
	//	}
		//if (base == pktNum)
		//{
		//	//发送EF包
		//	packet endpak, pkt;
		//	endpak.set_flag(EF);
		//	endpak.set_seq(pktNum);
		//	if (stopWaitsend(endpak, pkt))
		//		printf("[EF] 文件发送成功\n");
		//	CloseHandle(ackhandler);
		//	break;
		//}

	//	//超时重传
	//	if (stopTimer && (clock() - start) / CLOCKS_PER_SEC > MAX_WAIT_TIME)
	//	{
	//		if (cnt >= MAX_SEND_TIMES)
	//		{
	//			printf("重传次数过多，文件传输失败\n");
	//			return 0;
	//		}
	//		printf("[TIMEOUT] 开始超时重传\n");
	//		start = clock();
	//		stopTimer = true;
	//		for (int j = base; j < nextseqnum; j++)
	//		{
	//			sendpak(sockCli, addrSer, paks[j]);
	//		}
	//		cnt++;
	//	}
	//}

	//计算吞吐率
	clock_t tmend = clock();
	double totaltime = (double)(tmend - tmstart) / CLOCKS_PER_SEC;
	printf("传输总时间:%f\n", totaltime);
	printf("吞吐率: %f Mbps\n", (double)(pktnum + 1) * sizeof(packet) * 8 / totaltime / 1024 / 1024);
	//printf("文件发送完毕!\n");
	base = 0;
	nextseqnum = 0;
	paks.clear();
	return 1;
}