#pragma once 
#include<winsock.h>
#include<string>
#include<iostream>
//#include "init.h"
#pragma comment(lib,"Ws2_32.lib")
using namespace std;
#define MAX_SEND_TIMES 10//����ش�����
#define MAX_WAIT_TIME 1
#define ACK 0x0
#define SYN 0x1
#define EXIST 0x2
#define FIN 0x3
#define SF 0x4
#define TF 0x5
#define EF 0x6
#pragma pack(1)//һ�����ݰ�1byte���룬���û�еĻ�����4bytes����
struct packet {
	//���巢�����ݰ���ʽ
	DWORD SrcIP, DstIP;//���Ͷ�ip�ͽ��ն�ip
	u_short SrcPort, DstPort;//���Ͷ˶˿ںźͽ��ն˶˿ں�
	int msgseq;//��Ϣ���к�
	int acknum;//acknowledgement number
	unsigned short flag;//��־λ
	int index;//���������ļ���С
	int filelength;
	unsigned short len;//���ݳ���
	unsigned short checksum;//У���
	char data[1024];//���������
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
#pragma pack()//�ָ�
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
{//����У���
	//��������1byte���д���
	u_long sum = 0;
	int count = (packetlen + 1) / 2;
	u_short* tmp = (u_short*)this;// ������Ҫ����unsigned���ͣ��������������
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
	u_short* tmp = (u_short*)this;// ������Ҫ����unsigned���ͣ��������������
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
