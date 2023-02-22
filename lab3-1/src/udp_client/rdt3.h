#pragma once 
#include<winsock.h>
#include<string>
#include<iostream>
//#include "init.h"
#pragma comment(lib,"Ws2_32.lib")
using namespace std;
#define MAX_SEND_TIMES 10//最大重传次数
#define MAX_WAIT_TIME 1
#define ACK 0x0
#define SYN 0x1
#define EXIST 0x2
#define FIN 0x3
#define SF 0x4
#define TF 0x5
#define EF 0x6
#pragma pack(1)//一下内容按1byte对齐，如果没有的话会以4bytes对齐
struct packet {
	//定义发送数据包格式
	DWORD SrcIP, DstIP;//发送端ip和接收端ip
	u_short SrcPort, DstPort;//发送端端口号和接收端端口号
	int msgseq;//消息序列号
	int acknum;//acknowledgement number
	unsigned short flag;//标志位
	int index;//用于描述文件大小
	int filelength;
	unsigned short len;//数据长度
	unsigned short checksum;//校验和
	char data[1024];//传输的数据
	packet() {
		this->SrcIP = NULL;
		this->DstIP = NULL;
		this->SrcPort = 6666;
		this->DstPort = 5678;
		this->flag = 0;
		this->msgseq = 0;
		this->acknum = 0;
		this->index = 0;
		this->filelength = 0;
		this->len = 0;
		this->checksum = 0;
	};
	void set_ack(packet b);
	void set_syn();
	void set_exist();
	void set_fin();
	void set_sf();
	void set_tf();
	void set_ef();
	bool get_ack() {
		return ((this->flag >> ACK) & 1ul);
	}
	bool get_syn() {
		return ((this->flag >> SYN) & 1ul);
	}
	bool get_exist() {
		return ((this->flag >> EXIST) & 1ul);
	}
	bool get_fin() {
		return ((this->flag >> FIN) & 1ul);
	}
	bool get_sf() {
		return ((this->flag >> SF) & 1ul);
	}
	bool get_tf() {
		return ((this->flag >> TF) & 1ul);
	}
	bool get_ef() {
		return ((this->flag >> EF) & 1ul);
	}
	void setchecksum(int packetlen);
	bool checkchecksum(int packetlen);
};
#pragma pack()//恢复
void packet::set_ack(packet b)
{
	this->flag=this->flag | (1u<<ACK);
	this->acknum = b.msgseq;
}
void packet::set_syn()
{
	this->flag= this->flag | (1u << SYN);
}
void packet::set_exist()
{
	this->flag = this->flag | (1u << EXIST);
}
void packet::set_fin() {
	this->flag = this->flag | (1u << FIN);
}
void packet::set_sf() {
	this->flag = this->flag | (1u << SF);
}
void packet::set_tf() {
	this->flag = this->flag | (1u << TF);
}
void packet::set_ef() {
	this->flag = this->flag | (1u << EF);
}
void packet::setchecksum(int packetlen)
{//计算校验和
	//将数据以1byte进行处理
	u_long sum = 0;
	int count = (packetlen + 1) / 2;
	u_short* tmp = (u_short*)this;// 这里需要设置unsigned类型，否则会有正负号
	while (count--) {
		sum += *(tmp++);
	}
	sum = (sum >> 16) + (sum&0xffff);
	sum += (sum >> 16);
	this->checksum = ~sum;
	//cout << "sum" << sum << "checksum"<< this->checksum << endl;
	return;
}
bool packet::checkchecksum(int packetlen) {
	//cout << "flag:checksum" << this->checksum << endl;
	u_long sum = 0;
	int count = (packetlen + 1) / 2;
	u_short* tmp = (u_short*)this;// 这里需要设置unsigned类型，否则会有正负号
	while (count--) {
		sum += *(tmp++);
		if (sum & 0xffff0000)
		{
			sum &= 0xffff;
			sum++;
		}
	}
	//cout << "sum" << sum << endl;
	//cout << (u_short)sum << endl;
	if ((u_short)sum == 65535)
	{
		return true;
	}
	return false;
}
