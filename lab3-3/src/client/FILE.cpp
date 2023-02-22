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
		printf("�ļ���ȡʧ��\n");
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
	printf("�ļ���ȡ��������\n");
	in.close();
	for (int i = 0; i <=pktnum; i++)
	{
		packet tmp;
		if (i == pktnum)
		{
			printf("len��%d", length);
			tmp.cpdata(sendBuffer[i], length);
		}
		else
			tmp.cpdata(sendBuffer[i],1024);
		tmp.set_flag(TF);
		tmp.set_seq(i);
		paks.push_back(tmp);
	}
	printf("���ļ�Ԥ�ƽ�Ҫ����%d�����ݰ�\n", pktnum);
}
int sendfile(char* name)
{
	//��ȡ�ļ�
	printf("��ȡ�ļ���...\n");
	readfile(name);
	clock_t tmstart = clock();
	//���ȷ���SF��
	printf("[SEND,SF] ����SF��");
	packet startpak,rpak;
	startpak.set_flag(SF);
	startpak.cpdata(name,sizeof(name));
	startpak.set_seq(sendseq++);
	stopWaitsend(startpak,rpak);

	//�����ļ�����
	pktnum = paks.size();
	printf("pktnum:%d", pktnum);
	printf("[SEND,TF] ����TF��\n");
	DWORD dwThreadId1;
	HANDLE sendhand = CreateThread(nullptr, 0, sendhandler, LPVOID(&sockCli), 0, &dwThreadId1);
	DWORD dwThreadId2;
	HANDLE recvhand = CreateThread(nullptr, 0, recvhandler, LPVOID(&sockCli), 0, &dwThreadId2);
	while (1) {

		if (base == pktnum)
		{
			//����EF��
			packet endpak, pkt;
			endpak.set_flag(EF);
			endpak.set_seq(nextseqnum);
			if (stopWaitsend(endpak, pkt))
				printf("[EF] �ļ����ͳɹ�\n");
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
	//	//���������п��У����Ͱ�
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
		//	//����EF��
		//	packet endpak, pkt;
		//	endpak.set_flag(EF);
		//	endpak.set_seq(pktNum);
		//	if (stopWaitsend(endpak, pkt))
		//		printf("[EF] �ļ����ͳɹ�\n");
		//	CloseHandle(ackhandler);
		//	break;
		//}

	//	//��ʱ�ش�
	//	if (stopTimer && (clock() - start) / CLOCKS_PER_SEC > MAX_WAIT_TIME)
	//	{
	//		if (cnt >= MAX_SEND_TIMES)
	//		{
	//			printf("�ش��������࣬�ļ�����ʧ��\n");
	//			return 0;
	//		}
	//		printf("[TIMEOUT] ��ʼ��ʱ�ش�\n");
	//		start = clock();
	//		stopTimer = true;
	//		for (int j = base; j < nextseqnum; j++)
	//		{
	//			sendpak(sockCli, addrSer, paks[j]);
	//		}
	//		cnt++;
	//	}
	//}

	//����������
	clock_t tmend = clock();
	double totaltime = (double)(tmend - tmstart) / CLOCKS_PER_SEC;
	printf("������ʱ��:%f\n", totaltime);
	printf("������: %f Mbps\n", (double)(pktnum + 1) * sizeof(packet) * 8 / totaltime / 1024 / 1024);
	//printf("�ļ��������!\n");
	base = 0;
	nextseqnum = 0;
	paks.clear();
	return 1;
}