#include"rdt3.h"
#include<ctime>
#include <stdio.h>
bool stopWaitsend(packet &a, packet &b)
{
	extern SOCKET sockCli;
	extern SOCKADDR_IN addrSer;
	sendpak(sockCli, addrSer, a);
	clock_t start = clock();//��ʱ
	int flag = 0;//
	while (1) {
		recvpak(sockCli, addrSer, b);
		if (b.get_flag(ACK)&&b.get_ack()==a.get_seq()+1&&b.checkchecksum(sizeof(b)))
		{//�յ�ȷ��
			//printf("[RECV,ACK]  �յ�ȷ�ϰ�(ack)\n");
			return 1;
		}
		clock_t end = clock();
		if (flag == MAX_SEND_TIMES)//�ط�10��ʧ��
			return 0;
		if ((end - start) / CLOCKS_PER_SEC >= MAX_WAIT_TIME)
		{
			flag++;
			start = clock();//���ü�ʱ��
			//cout << "��" << flag << "���ش�" << endl;
			printf("��%d���ش�\n",flag);
			sendpak(sockCli, addrSer, a);//�ش�
		}
	}
	return 0;
}