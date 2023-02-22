#include<vector>
//#include"conf.h"
#include"pak.h"
#include <ctime>
char sendBuffer[65536][1024];
std::vector<packet> paks;
DWORD srcIP = inet_addr("127.0.0.1");
DWORD dstIP = inet_addr("127.0.0.3");
u_short srcPort = htons(11111);
u_short dstPort = htons(33333);
SOCKADDR_IN addrSer, addrCli;
SOCKET sockCli;
int sendseq = 0;
int status = 0;
int base = 0, nextseqnum = 0;//滑动窗口
clock_t start, end;//超时重传
int cnt = 0;
bool stopTimer = false;//停止计时


clock_t clockstart;
clock_t clockend;
int constatus = 0;
int ssthresh = 32;
int cwnd = 1;
bool overtime = false;
int pktnum = 0;