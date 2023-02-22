#pragma once
#include<iostream>
#include <fstream>
#include"rdt3.h"
#include"client.h"
using namespace std;
/*void readfile(char* name, int& length, int& index)
{//�����ļ�
	char content[1024];
	cout << "�����ļ�...\n";
	index = 0;
	length = 0;
	ifstream in(name, ifstream::binary);//�Զ����Ʒ�ʽ�����ļ�
	if (!in)
	{
		cout << "�ļ���Ч\n";
		return;
	}
	char line=in.get();;
	while (in)//����ʧ�ܻ�����˳�ѭ��
	{//��ȡ�ļ����ݴ���content����

		content[length % 1024] = line;//ÿ��1024���ֽڣ���Ӧdata��Ա����Ϊ1024
		length++;
		if (length % 1024 == 0)
		{
			index++;
			length = 0;
			memset(content, 0, sizeof(content));
			//cout << index << endl;
		}
		line = in.get();
	}
	//cout << index << endl;
	in.close();
	return;
}*/
int sendfile(SOCKET& sock, SOCKADDR_IN& addr, char* name)
{
	char content[1024];
	int length = 0;
	int index = 0;
	//�����ļ���
	clock_t start = clock();
	struct packet a, b;
	//a.index = index;
	//a.filelength = length;
	int t = strlen(name);

	memset(a.data, 0, sizeof(a.data));
	memcpy(a.data, name, t);

	a.set_sf();
	a.msgseq = sendseq++;
	//����ef��;
	if (!stopWaitsend(sock, addr, a, b))
	{
		cout << "[FILE,TRANS,ERROR] �ļ�����ʧ��\n";
		return 0;
	}
	cout << "[FILE,READ]  �����ļ�...\n";
	index = 0;
	length = 0;
	ifstream in(name, ifstream::binary);//�Զ����Ʒ�ʽ�����ļ�
	if (!in)
	{
		cout << "[FILE,ERROR]  �ļ���Ч\n";
		return 0;
	}
	char line = in.get();;
	while (in)//����ʧ�ܻ�����˳�ѭ��
	{//��ȡ�ļ����ݴ���content����

		content[length % 1024] = line;//ÿ��1024���ֽڣ���Ӧdata��Ա����Ϊ1024
		length++;
		if (length % 1024 == 0)
		{
			packet tmp;
			tmp.set_tf();
			tmp.index = index;
			tmp.filelength = length;
			memset(tmp.data, 0, sizeof(tmp.data));
			memcpy(tmp.data, content, sizeof(content));
			memset(content, 0, sizeof(content));
			tmp.msgseq = sendseq++;
			//cout << tmp.filelength << endl;
			if (!stopWaitsend(sock, addr, tmp, b))
			{
				cout << "[FILE,SEND,ERROR]  �ļ�����ʧ��\n";
				return 0;
			}
			index++;
			length = 0;
			//cout << index << endl;
		}
		line = in.get();
	}
	if (length != 0) {
		packet tmp;
		tmp.set_tf();
		tmp.index = index;
		tmp.filelength = length;
		memset(tmp.data, 0, sizeof(tmp.data));
		memcpy(tmp.data, content, sizeof(content));
		memset(content, 0, sizeof(content));
		tmp.msgseq = sendseq++;
		if (!stopWaitsend(sock, addr, tmp, b))
		{
			cout << "[FILE,SEND,ERROR]  �ļ�����ʧ��\n";
			return 0;
		}
	}
	in.close();
	//�����ļ�����ȷ�ϰ�
	packet p1, p2;
	p1.set_ef();
	cout << "[SEND,EF]����ef��\n";
	if (!stopWaitsend(sock, addr, p1, p2))
	{
		cout << "[ERROR] ����0\n";
		return 0;
	}
	//�����ʼ���
	clock_t end = clock();
	double dtime = (double)(end - start) / CLOCKS_PER_SEC;
	cout << "[TOTAL TIME]  ����ʱ:" << dtime << endl;
	cout << "[RATE]  ������:" << (double)(index + 1) * sizeof(packet) * 8 / dtime / 1024 / 1024 << "Mbps" << endl;
	return 1;
}
