#ifndef __PAK_H__
#define __PAK_H__
#define setbit(x,y) x|=(1<<y);
#define clrbit(x,y) x&=!(1<<y);
#define ACK 0x0
#define SYN 0x1
#define EXIST 0x2
#define FIN 0x3
#define SF 0x4
#define TF 0x5
#define EF 0x6
#include<windows.h>
#pragma pack(1)
struct packet {
public:
	//���ݱ���ʽ
	DWORD SrcIP, DstIP;
	u_short SrcPort, DstPort;
	int msgseq;
	int acknum;
	unsigned short flag;//��־λ
	unsigned short len;//���ݳ���
	unsigned short checksum;//У���
	char data[1024];
public:
	packet();
	void set_flag(int setbit);
	bool get_flag(int getbit);
	void clear_flag(int setbit);
	void set_seq(int sendseq);
	void set_ack(int acknum);
	int get_seq();
	int get_ack();
	void cpdata(char* s,int len);
	void setchecksum(int packetlen);
	bool checkchecksum(int packetlen);
	void clrpak();
	void log(int typeno);
};
#pragma pack()
void sendpak(SOCKET& socket, SOCKADDR_IN& addr, packet& a);
void recvpak(SOCKET& socket, SOCKADDR_IN& addr, packet& a);
#endif
