#pragma once

#define SYN 0x01
#define ACK 0x02
#define FIN 0x04
#define START 0x08 //开始传输文件

#define BUF_SIZE 1024 //1024 + 10
#define WAIT_TIME 5
#define MAX_TIMES 10

char FILE_PATH[] = "C:\\Users\\cultu\\source\\repos\\udp2_client\\udp2_client\\";
char OUT_PATH[] = "C:\\Users\\cultu\\source\\repos\\udp2_server\\udp2_server\\";
struct sockaddr fromAddr;
int addrLen = sizeof(fromAddr);
int changeStateSeq = 0;

//接收客户端请求
SOCKADDR clntAddr;  //客户端地址信息
int nSize = sizeof(SOCKADDR);

time_t tt;
char t[32] = { 0 };

//初始化变量
WSADATA wsaData;
SOCKET serverSocket;
struct sockaddr_in local;

int state = 0;//描述客户端的状态
clock_t start, end;

char buffer[20000][1024];
int pktNum = 0;
char name[1024];

int lose = 0;

void print2(int a);

#pragma pack(1)//按1byte对齐
class packet {
public:
    int flag;//标志位
    int seq;//序列号
    int acknum;//应答序列号
    int filenum;//当前文件传输完毕剩余的包个数
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
        this->flag = ACK;
        //this->ackseq = sendseq++;
    }
    void setStart(int pktNum) {
        this->flag += START;
        this->filenum = pktNum;
    }
    int getStart() {
        if (this->flag & START) {
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
            sum += temp[i];
            //处理溢出
            while (sum >= 65536)
            {
                int t = sum >> 16;
                sum = sum & 0x0000ffff;
                sum += t;
            }
        }
        //print2(sum);
        //Sleep(100000);
        //按位取反,填入对应位置
        this->checksum = ~(u_short)sum;
    }
    bool checkChecksum()//接收方对校验和进行检验
    {
        int sum = 0;
        u_char temp2[1046];
        memset(temp2, 0, sizeof(temp2));
        memcpy(temp2, this, sizeof(packet));
        u_short* temp = (u_short*)temp2;
        for (int i = 0; i < 523; i++)
        {
            if (i == 10)
                continue;
            //sum += temp[2 * i]  + temp[2 * i + 1] << 8;
            sum += temp[i];
            //printf("1 ");
            //print2(sum);
            while (sum >= 65536)
            {//溢出
                int t = sum >> 16;//计算方法与设置校验和相同
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
        //printf("%hu\n", checksum);
        //printf("%d\n", sum);
        //把计算出来的校验和和报文中该字段的值相加，如果等于0xffff(全1)，则校验成功
        if (checksum + (u_short)sum == 65535)
            return true;
        return false;
    }
};
#pragma pack()

