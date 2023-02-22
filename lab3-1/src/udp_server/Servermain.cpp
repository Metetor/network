#include<iostream>
#include"server.h"
#include"rdt3.h"
#include"conf.h"
#include"FILE.h"
using namespace std;
int status = 0;
int buildConnectSer(SOCKET& sock, SOCKADDR_IN& addr, packet& p)
{
	struct packet a, b;
	a.set_ack(p);
	a.msgseq = sendseq++;
	sendpak(sock,addr,a);
	//cout << "���ӳɹ�\n";
	return 1;
};
int disConnectSer(SOCKET& sock, SOCKADDR_IN& addr,packet &b) {
	//2�λ���
	struct packet a;
	a.set_fin();
	a.set_ack(b);
	sendpak(sock, addr, a);
	return 1;
}
bool stopWaitsend(SOCKET& sock, SOCKADDR_IN& addr, packet& a, packet b)
{
	sendpak(sock, addr,a);
	clock_t start = clock();//��ʱ
	int flag = 0;//
	while (1) {
		recvpak(sock, addr, b);
		if (b.get_ack() && b.acknum == a.msgseq)
		{//�յ�ȷ��
			return 1;
		}
		clock_t end = clock();
		if (flag == MAX_SEND_TIMES)//�ط�10��ʧ��
			return 0;
		if ((end - start) / CLOCKS_PER_SEC >= MAX_WAIT_TIME)
		{
			flag++;
			start = clock();//���ü�ʱ��
			cout << "[RESEND]  ��" << flag << "���ش�" << endl;
			sendpak(sock, addr, a);//�ش�
		}
	}
	return 0;
}
bool stopWaitrecv(SOCKET& sock, SOCKADDR_IN& addr, packet& a, packet b)
{
	int flag = 0;
	while (1)
	{
		recvpak(sock,addr,a);//�յ��Է���������Ϣa
		if (a.get_exist())//��Ϊ��recv��������˷�������������Ҫ����յ�����Ϣ�Ƿ�Ϊ��
		{
			int check = a.checkchecksum(sizeof(a));//���У���
			if (check)//���ɹ�
			{
				cout << "[RECV]  �յ����ݰ�(checked,checksum="<<a.checksum<<")" << endl;
				b.set_ack(a);//�ظ������յ���Ϣa��ack��Ϣb
				b.msgseq = sendseq++;
				cout << "[SEND,ACK]  ����ȷ�ϰ�(msgseq=" << b.msgseq << "acknum=" << b.acknum << ")\n";
				sendpak(sock,addr,b);
				memset((char*)&b, 0, sizeof(packet));//��ֹ������һ����Ϣ����
				return 1;
			}
			else
			{
				packet b;
				sendpak(sock, addr, b);
				continue;
			}
		}
	}
	cout << "[RECV,ERROR]  ����ʧ��" << endl;
	return 0;
}
int main() {
	if (!WSAinit())
		printf("[WSA,SUCCESS]  WSAinit Success\n");
	SOCKET sockSer = socket(AF_INET, SOCK_DGRAM, 0);
	u_long imode = 1;
	ioctlsocket(sockSer, FIONBIO, &imode);
	SOCKADDR_IN addrCli;
	addrCli.sin_family = AF_INET;
	addrCli.sin_port = htons(CliPORT);
	addrCli.sin_addr.S_un.S_addr = inet_addr(ADDRSER);
	//���׽���
	SOCKADDR_IN addrSer;
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(SerPORT);
	addrSer.sin_addr.S_un.S_addr = inet_addr(ADDRSER);
	bind(sockSer, (SOCKADDR*)&addrSer, sizeof(SOCKADDR));
	cout << "[BIND,SUCCESS] ���׽��ֳɹ�\n";
	while (1)
	{
		packet b;
		recvpak(sockSer,addrCli,b);
		if (!FSM(sockSer,addrCli,b))
			break;
	}
	cout << 1 << endl;
	Sleep(20);
}
int FSM(SOCKET& sock, SOCKADDR_IN& addr, packet& b) {
	if (b.get_syn())
	{
		if (buildConnectSer(sock, addr, b))
		{
			status = 1;//����״̬
			cout << "[CONNECT,SUCCESS]  �������ӳɹ�" << endl;
		}
		else
			cout << "[CONNECT,ERROR]  ���ӽ���ʧ��\n";
	}
	else if (b.get_sf())
	{
		if (status)
		{
			cout << "[FILE,RECV]  ��ʼ�����ļ�...\n";
			recvfile(sock, addr, b);
		}
		else
			cout << "[ERROR]  ���Ƚ�������\n";
	}
	else if (b.get_fin())
	{
		if (disConnectSer(sock, addr,b))
		{
			cout << "[DISCONNECT,SUCCESS]  �Ͽ�����" << endl;
			status = 0;
			sendseq = 0;
		}
	}
	return 1;
}