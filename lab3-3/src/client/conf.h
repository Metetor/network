#ifndef __CONF_H__
#define __CONF_H__
#include<windows.h>
#include <stdio.h>
#include<vector>
#include"pak.h"
#define UDPSENDBFRSIZE 65536  //64 KB
#define N 10
using namespace std;
extern vector<packet> paks;
extern char sendBuffer[UDPSENDBFRSIZE][1024];
extern DWORD srcIP;
extern DWORD dstIP;
extern u_short srcPort;
extern u_short dstPort;
extern SOCKADDR_IN addrSer, addrCli;
extern SOCKET sockCli;
extern int sendseq;
extern int status;
extern int base;
extern int nextseqnum;
extern bool stopTimer;
#endif