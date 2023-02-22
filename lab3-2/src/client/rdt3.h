#pragma once
#include <WinSock2.h>
#include <fstream>
using namespace std;


#define SYN 0x01
#define ACK 0x02
#define FIN 0x04
#define SF 0x08              //开始传输文件

#define BUF_SIZE 1024
#define MAX_TIMES 5             //超时重传次数
#define TIME_OUT 1000           //超时重传的时间，毫秒

#define N 10                    //滑动窗口的大小

char filenames[][30] = { "1.jpg","2.jpg","3.txt","helloworld.txt" };
char FILE_PATH[] = "C:\\Users\\cultu\\source\\repos\\udp2_client\\udp2_client";


WSADATA wsaData;                //初始化变量
SOCKET clientSocket;

time_t tt;                      //日志使用的时间戳
char t[32] = { 0 };

struct sockaddr_in local;       //本地IP和端口信息

struct sockaddr_in servAddr;    //服务器IP和端口信息

struct sockaddr fromAddr;       //接收到的包的来源地址信息
int addrLen = sizeof(fromAddr);

clock_t start, end;             //超时重传

char buffer[20000][BUF_SIZE];   //文件缓冲区

int changeStateSeq = 0;                //用来切换状态时用的序列号（比如START，SYN，FIN），从0开始

int base = 0, nextseqnum = 0;   //滑动窗口


void print2(int a);
#pragma pack(1)//按1byte对齐
class packet {
public:
    int flag;//标志位
    int seq;//序列号
    int ackseq;//应答序列号
    int filenum;//文件的包个数
    int datalen;
    u_short checksum;
    char data[BUF_SIZE];
    packet() {
        memset(this, 0, sizeof(packet));
    }
    void setSyn() {
        this->flag += SYN;
    }
    int getSyn() {
        if (this->flag & SYN) {
            return 1;
        }
        else {
            return 0;
        }
    }
    int getAck() {
        if (this->flag & ACK) {
            return 1;
        }
        else {
            return 0;
        }
    }
    void setAck() {
        this->flag += ACK;
    }
    void setSF(int pktNum) {
        this->flag += SF;
        this->filenum = pktNum;
    }
    int getSF() {
        if (this->flag & SF) {
            return 1;
        }
        else {
            return 0;
        }
    }
    void setFin() {
        this->flag = FIN;
    }
    int getFin() {
        if (this->flag & FIN) {
            return 1;
        }
        else {
            return 0;
        }
    }
    void setChecksum()
    {
        int sum = 0;
        u_char temp2[1046];
        memset(temp2, 0, sizeof(temp2));
        memcpy(temp2, this, sizeof(packet));
        u_short* temp = (u_short*)temp2;
        for (int i = 0; i < 523; i++)
        {
            //sum += temp[2 * i] + temp[2 * i + 1] << 8;
            sum += temp[i];
            while (sum >= 65536)
            {
                int t = sum >> 16;
                //printf("2 ");
                //print2(sum);
                sum = sum & 0x0000ffff;
                //printf("3 ");
                //print2(sum);
                sum += t;
                //printf("4 ");
                //print2(sum);
            }
        }
        //Sleep(100000);
        this->checksum = ~(u_short)sum;
    }
    bool checkChecksum()
    {
        int sum = 0;
        u_char temp2[1046];
        memset(temp2, 0, sizeof(temp2));
        memcpy(temp2, this, sizeof(packet));
        u_short* temp = (u_short*)temp2;
        for (int i = 0; i < 523; i++)
        {
            //暂时不计算checksum字段的值
            if (i == 10)
                continue;
            sum += temp[i];
            while (sum >= 65536)
            {
                int t = sum >> 16;
                sum = sum & 0x0000ffff;
                sum += t;
            }
        }
        //print2(sum);
        //print2(checksum);
        if (checksum + (u_short)sum == 65535)
            return true;
        return false;
    }
};
#pragma pack()


