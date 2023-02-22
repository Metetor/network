#include"conf.h"
#include"pak.h"
#include <time.h>
packet::packet() {
	extern DWORD srcIP;
	extern DWORD dstIP;
	extern u_short srcPort;
	extern u_short dstPort;
	this->SrcIP = srcIP;
	this->DstIP = dstIP;
	this->SrcPort = srcPort;
	this->DstPort = dstPort;
	this->msgseq = 0;
	this->acknum = 0;
	this->flag = 0;
	this->len = 0;
	this->checksum = 0;
}
void packet::set_flag(int setbit) {
	setbit(this->flag, setbit);
	return;
}
bool packet::get_flag(int getbit) {
	return ((this->flag >> getbit) & 1ul);
}
void packet::clear_flag(int setbit) {
	clrbit(this->flag, setbit);
	return;
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
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	this->checksum = ~sum;
	return;
}
bool packet::checkchecksum(int packetlen) {
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
	if ((u_short)sum == 65535)
	{
		return true;
	}
	return false;
}
void packet::set_seq(int sendseq)
{
	this->msgseq = sendseq;
}

void packet::set_ack(int acknum)
{
	this->acknum = acknum;
}

int packet::get_seq()
{
	return this->msgseq;
}

int packet::get_ack()
{
	return this->acknum;
}
void packet::cpdata(char *s,int &l)
{
	l = this->len;
	memcpy(s, this->data,this->len);
}
void packet::clrpak()
{
	memset(this->data, 0, sizeof(this->data));
}

void packet::log(int typeno)
{
	char t[32] = { 0 };
	time_t now = time(0);
	strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&now));
	char type[8];
	if (typeno == 1)
	{
		strcpy_s(type, "SEND");
	}
	else if (typeno == 2)
	{
		strcpy_s(type, "RECV");
	}
	else
	{
		strcpy_s(type, "UNKNOWN");
	}
	printf("[%s %s] flag:%d SYN:%d ACK:%d msgseq:%d acknum:%d \n", t, type,this->flag, this->get_flag(SYN), this->get_flag(ACK), this->msgseq, this->acknum);
}

//将发送数据包和接收数据包封装为函数
void sendpak(SOCKET& socket, SOCKADDR_IN& addr, packet& a)
{
	//设置flag
	a.set_flag(EXIST);
	a.setchecksum(sizeof(a));
	sendto(socket, (char*)&a, sizeof(packet), 0, (sockaddr*)&addr, sizeof(addr));
	a.log(1);
}
void recvpak(SOCKET& socket, SOCKADDR_IN& addr, packet& a)
{
	a.clrpak();
	int addrlen = sizeof(addr);
	recvfrom(socket, (char*)&a, sizeof(packet), 0, (sockaddr*)&addr, &addrlen);
	if (a.get_flag(EXIST))
		a.log(2);
}