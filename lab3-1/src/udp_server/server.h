#pragma once
#include"rdt3.h"
//Ϊ�˼򻯴��룬����������ã����ｫ�������ݰ��ͽ��շ�װΪ����
void sendpak(SOCKET& sock, SOCKADDR_IN& addr, packet& a);
void recvpak(SOCKET& sock, SOCKADDR_IN& addr, packet& a);
void sendpak(SOCKET& sock, SOCKADDR_IN& addr, packet& a)
{
	a.setchecksum(sizeof(a));//����У���
	sendto(sock, (char*)&a, sizeof(packet), 0, (sockaddr*)&addr, sizeof(addr));
}
void recvpak(SOCKET& sock, SOCKADDR_IN& addr, packet& a)
{
	//memset(a->data, 0, sizeof(a->data));//������ݣ���ֹ�յ�֮ǰ��Ϣ��Ӱ��
	int addrlen = sizeof(addr);
	recvfrom(sock, (char*)&a, sizeof(packet), 0, (sockaddr*)&addr, &addrlen);
}

//��������:�����,packet pΪ�յ��Ĵ���syn��־λ����Ϣ
int buildConnectSer(SOCKET& sock, SOCKADDR_IN& addr,packet& p);
int disConnectSer(SOCKET& sock, SOCKADDR_IN& addr,packet&b);
bool stopWaitsend(SOCKET& sock, SOCKADDR_IN& addr, packet& a, packet b);//aд���������Ϣ������յ�b���ص�ack�����ȷ�ɹ�
bool stopWaitrecv(SOCKET& sock, SOCKADDR_IN& addr, packet& a, packet b);
int FSM(SOCKET& sock, SOCKADDR_IN& addr, packet& a);