#pragma once
//#include<iostream>
#include <fstream>
#include"rdt3.h"
#include"server.h"
using namespace std;
void outfile(char* name, char content[1024], int length);
int recvfile(SOCKET sock, SOCKADDR_IN& addr, packet& a)
{
	char content[1024];
	packet p;
	p.set_ack(a);
	a.msgseq = sendseq++;
	//cout << "����pak...\n";
	sendpak(sock,addr,p);
	//��ȡ�ļ���Ϣ
	int index = a.index;
	int length = a.filelength;
	char name[100];
	memset(name, 0, 100);
	memcpy(name, a.data, 100);
	while (1) {
		packet tmp,b;
		stopWaitrecv(sock, addr, tmp,b);
		if (tmp.get_tf())
		{
			//cout << "��ʼ����...\n";
			memset(content, 0, sizeof(content));
			//cout << "size" << sizeof(tmp.data) << "len" << tmp.filelength << endl;
			memcpy(content, tmp.data, tmp.filelength);
			//cout << "before outfile\n";
			outfile(name, content, tmp.filelength);
		}
		if (tmp.get_ef())
		{
			cout << "�յ�ef��" << endl;
			break;
		}
	}
	cout << name << "�ļ����ճɹ�\n";
	return 1;
}void outfile(char* name, char content[1024], int length)
{
	//cout << "this is outfile\n";
	ofstream out;
	out.open(name,ios::binary|ios::app | ios::out);
	for (int i = 0; i < length; i++)
	{
		out << content[i];
	}
	out.close();
}