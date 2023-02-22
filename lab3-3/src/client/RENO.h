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
	printf("���ڴ�СΪ:%d\n", cwnd);
}
DWORD WINAPI recvhandler(LPVOID lparam)
{
	printwindow();
	SOCKET* clientSock = (SOCKET*)lparam;
	//cout << "����" << i << endl;
	clock_t finalovertime;
	int t = 0;
	while (base < pktnum)
	{
		if (nextseqnum > base + cwnd || nextseqnum == base)//�����������Է���û�з���ack�����ߴ���Ϊ��
			Sleep(2);
		packet pak;
		recvpak(*clientSock,addrSer,pak);
		if (pak.get_flag(ACK)&&pak.checkchecksum(sizeof(pak)))//�յ�ȷ����Ϣ�������ȷ
		{
			clockstart = clock();//���ü�ʱ��
			if (pak.get_ack() > base)//�յ���ȷ����Ϣ���
			{
				if (constatus == 0)
				{
					cwnd += pak.acknum - base;//�ۻ�ȷ��
					if (cwnd >= ssthresh)
						constatus = 1;//ӵ������

				}
				else if (constatus == 1)
				{
					cwnd += (pak.acknum- base) / cwnd;//��������
				}
				printwindow();
				base = pak.get_ack();
				overtime = 0;
				t = 0;
			}
			else if (pak.get_ack() <= base)
			{
				t++;
				printf("�յ���%d�������\n", t);
				printwindow();
				if (constatus == 2) {//���ٻָ�
					cwnd += 1;
					if (cwnd >= ssthresh) constatus = 1;//����ӵ������׶�
				}
				if (t >= 3)//���ڵ�����������ack
				{
					constatus = 2; //�����ش�
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
DWORD WINAPI sendhandler(LPVOID lparam)//���߳�
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
				flag = 0;//��������

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
					if ((e - s) / CLOCKS_PER_SEC >= MAX_WAIT_TIME)//��ʱ�ط�
					{
						overtime = 1;
						nextseqnum++;
						break;
					}
				}
			}
		}
		if (overtime == 1)//���·���
		{
			if (constatus == 1 || constatus == 0)
			{
				ssthresh = cwnd / 2;
				cwnd = 1;
				constatus = 0;//���½����������׶�
				printwindow();
			}
			else if (constatus == 2)
			{//�����ش�
				printf("�����ش�\n");
				ssthresh = cwnd / 2;
				cwnd = ssthresh + 3;
			}
			if (flag)
				Sleep(10);//�����ش�����
			for (int i = base; i <nextseqnum; i++)
			{
				//���·���
				if (overtime)
					sendpak(*clientSock, addrSer, paks[i]);
				else break;
				flag++;
			}
			overtime = 0;//�ش���ʶ��0
			s = clock();//���ü�ʱ��
		}
		clock_t e = clock();
		if ((e - s) / CLOCKS_PER_SEC >=MAX_WAIT_TIME)//��ʱ�ط�
			overtime = 1;
	}
	printf("exitpoint1\n");
	return 1;
}