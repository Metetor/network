#include"conf.h"
char recvBuffer[UDPSENDBFRSIZE][1024];
DWORD srcIP = inet_addr("127.0.0.3");
DWORD dstIP = inet_addr("127.0.0.1");
u_short srcPort = htons(33333);
u_short dstPort = htons(11111);
SOCKADDR_IN addrSer, addrCli;
SOCKET sockSer;
char name[1024];
int sendseq = 0;
int status = 0;