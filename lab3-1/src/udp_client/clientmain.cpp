#include<iostream>
#include<ctime>
#include"conf.h"
#include"rdt3.h"
#include"client.h"
#include "FILE.h"
using namespace std;
int buildConnectCli(SOCKET& sock, SOCKADDR_IN& addr) {
	/*
	�ͻ��˷�������
	*/
	packet a, b;
	a.set_syn();
	//���÷������к�
	a.msgseq = sendseq++;
	//cout << "�������к�.\n";
	if (stopWaitsend(sock,addr,a, b))
		return 1;//ͣ�ȷ��ͻ��ƣ�������Ϣa,������ʱ�¼�ʱ�ش�������յ��˶�Ӧ��ack��b����1,��������ش���������0
	else
		return 0;
};
bool stopWaitsend(SOCKET& sock, SOCKADDR_IN& addr,packet& a, packet b)
{
	//cout << "����" << endl;
	sendpak(sock,addr,a);
	cout << "[SEND]  ���Ͱ�(msgseq="<<a.msgseq<<"acknum="<<a.acknum<<")\n";
	clock_t start = clock();//��ʱ
	int flag = 0;//
	while (1) {
		recvpak(sock, addr, b);
		//cout << "recv\n";
		//cout << "b.acknum" << b.acknum << "a.msgseq" << a.msgseq << endl;
		
		if (b.get_ack()&&b.acknum == a.msgseq&&b.checkchecksum(sizeof(b)))
		{//�յ�ȷ��
			cout << "[RECV,ACK]  �յ�ȷ�ϰ�(ack)\n";
			return 1;
		}
		clock_t end = clock();
		if (flag == MAX_SEND_TIMES)//�ط�10��ʧ��
			return 0;
		if ((end - start) / CLOCKS_PER_SEC >= MAX_WAIT_TIME)
		{
			flag++;
			start = clock();//���ü�ʱ��
			cout << "��" << flag << "���ش�" << endl;
			sendpak(sock, addr, a);//�ش�
		}
	}
	return 0;
}
int disConnect(SOCKET& sock, SOCKADDR_IN& addr) {
//2�λ���
	packet a, b;
	a.set_fin();
	if (stopWaitsend(sock, addr, a, b))
		return 1;
	else
		return 0;
}
int main() {
	if(!WSAinit())
		printf("[WSA,SUCCESS]  WSAinit Success\n");
	SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);
	//�����׽��ַ�����
	u_long imode = 1;
	ioctlsocket(sockClient, FIONBIO, &imode);
	//string ADDRSER;
	//cout << "������������ĵ�ַ:";
	//getline(cin, ADDRSER);
	DstIP = inet_addr("127.0.0.1");
	HostIP = inet_addr("127.0.0.1");
	SOCKADDR_IN addrSer;
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(SerPORT);
	//addrSer.sin_addr.S_un.S_addr = inet_addr(ADDRSER.c_str());
	addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	SOCKADDR_IN addrCli;
	addrCli.sin_family = AF_INET;
	addrCli.sin_port = htons(CliPORT);
	//addrSer.sin_addr.S_un.S_addr = inet_addr(ADDRSER.c_str());
	addrCli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (bind(sockClient, (SOCKADDR*)&addrCli, sizeof(SOCKADDR)) == -1)
		cout << "client bind error\n";
	cout << "[BIND,SUCCESS]  client bind success\n";
	if (buildConnectCli(sockClient, addrSer))
	{
		cout << "[CONNECT,SUCESS]  �������ӳɹ�...\n";
	}
	else
		cout << "[CONNECT,ERROR]  ���ӽ���ʧ��\n";
	int op;
	cout << "[OP]  ���������:1.�����ļ�  2.exit\n";
	cin >> op;
	//sendpak(sockClient, addrSer, p);
	/*��Ϣ�������պͷ��ͣ�*/
	//sendto(sockClient,"hello", 6, 0, (sockaddr*)&addrSer, sizeof(addrSer));
	while (1)
	{
		if (op == 1) {
			//�����ļ���
			char name[50];
			cout << "[FILE,INPUT]  �������ļ���:";
			cin >> name;
			if (sendfile(sockClient, addrSer, name))
			{
				cout << "[FILE,SUCCESS]  �ļ����ͳɹ�\n" << "���������:1.�����ļ�  2.exit\n";
				cin >> op;
				if (op == 2)
					break;
			}
			else
				cout << "[FILE,ERROR]  �ļ�����ʧ��\n";
		}
		if (op == 2)
			break;
	}
	if (disConnect(sockClient, addrSer))
	{
		cout << "[DISCONNECT,SUCCESS]  �����ѶϿ�\n";
		sendseq = 0;
	}
	else
		cout << "[DISCONNECT,ERROR]  ���ӶϿ�ʧ��\n";
	sendseq = 0;
	system("pause");
	return 0;
}