#pragma once
#include"rdt3.h"
#include<iostream>
using namespace std;
//#include"init.h"
//Ϊ�˼򻯴��룬����������ã����ｫ�������ݰ��ͽ��շ�װΪ����
DWORD HostIP, DstIP;
void sendpak(SOCKET &sock,SOCKADDR_IN &addr,packet& a)
{
	a.set_exist();
	a.setchecksum(sizeof(a));//����У���
	sendto(sock, (char*)&a, sizeof(packet), 0, (sockaddr*)&addr, sizeof(addr));
}
void recvpak(SOCKET &sock,SOCKADDR_IN &addr,packet& a)
{
	memset(a.data, 0, sizeof(a.data));//������ݣ���ֹ�յ�֮ǰ��Ϣ��Ӱ��
	int addrlen = sizeof(addr);
	recvfrom(sock, (char*)&a, sizeof(packet), 0, (sockaddr*)&addr,&addrlen);
}
//��������
int buildConnectCli(SOCKET& sock, SOCKADDR_IN& addr);
//�Ͽ�����
int disConnect(SOCKET& sock, SOCKADDR_IN& addr);
//ȷ���ش�
bool stopWaitsend(SOCKET& sock, SOCKADDR_IN& addr,packet& a, packet b);//aд���������Ϣ������յ�b���ص�ack�����ȷ�ɹ�

