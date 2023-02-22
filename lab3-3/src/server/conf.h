#ifndef __CONF_H__
#define __CONF_H__
#include<windows.h>
#include <stdio.h>
#define UDPSENDBFRSIZE 65536  //64 KB

extern char recvBuffer[UDPSENDBFRSIZE][1024];
extern DWORD srcIP;
extern DWORD dstIP;
extern u_short srcPort;
extern u_short dstPort;
extern SOCKADDR_IN addrSer, addrCli;
extern SOCKET sockSer;
extern char name[1024];
extern int sendseq;
extern int status;
#endif