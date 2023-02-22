#pragma once
#include<iostream>
#include <fstream>
#include"rdt3.h"
#include"client.h"
using namespace std;
/*void readfile(char* name, int& length, int& index)
{//读入文件
	char content[1024];
	cout << "读入文件...\n";
	index = 0;
	length = 0;
	ifstream in(name, ifstream::binary);//以二进制方式读入文件
	if (!in)
	{
		cout << "文件无效\n";
		return;
	}
	char line=in.get();;
	while (in)//读入失败或完成退出循环
	{//读取文件内容存在content数组

		content[length % 1024] = line;//每行1024个字节，对应data成员长度为1024
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
	//发送文件名
	clock_t start = clock();
	struct packet a, b;
	//a.index = index;
	//a.filelength = length;
	int t = strlen(name);

	memset(a.data, 0, sizeof(a.data));
	memcpy(a.data, name, t);

	a.set_sf();
	a.msgseq = sendseq++;
	//发送ef包;
	if (!stopWaitsend(sock, addr, a, b))
	{
		cout << "[FILE,TRANS,ERROR] 文件传输失败\n";
		return 0;
	}
	cout << "[FILE,READ]  读入文件...\n";
	index = 0;
	length = 0;
	ifstream in(name, ifstream::binary);//以二进制方式读入文件
	if (!in)
	{
		cout << "[FILE,ERROR]  文件无效\n";
		return 0;
	}
	char line = in.get();;
	while (in)//读入失败或完成退出循环
	{//读取文件内容存在content数组

		content[length % 1024] = line;//每行1024个字节，对应data成员长度为1024
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
				cout << "[FILE,SEND,ERROR]  文件发送失败\n";
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
			cout << "[FILE,SEND,ERROR]  文件发送失败\n";
			return 0;
		}
	}
	in.close();
	//发送文件结束确认包
	packet p1, p2;
	p1.set_ef();
	cout << "[SEND,EF]发送ef包\n";
	if (!stopWaitsend(sock, addr, p1, p2))
	{
		cout << "[ERROR] 出错0\n";
		return 0;
	}
	//吞吐率计算
	clock_t end = clock();
	double dtime = (double)(end - start) / CLOCKS_PER_SEC;
	cout << "[TOTAL TIME]  总用时:" << dtime << endl;
	cout << "[RATE]  吞吐率:" << (double)(index + 1) * sizeof(packet) * 8 / dtime / 1024 / 1024 << "Mbps" << endl;
	return 1;
}
