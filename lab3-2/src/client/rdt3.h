#pragma once
#include <WinSock2.h>
#include <fstream>
using namespace std;


#define SYN 0x01
#define ACK 0x02
#define FIN 0x04
#define SF 0x08              //��ʼ�����ļ�

#define BUF_SIZE 1024
#define MAX_TIMES 5             //��ʱ�ش�����
#define TIME_OUT 1000           //��ʱ�ش���ʱ�䣬����

#define N 10                    //�������ڵĴ�С

char filenames[][30] = { "1.jpg","2.jpg","3.txt","helloworld.txt" };
char FILE_PATH[] = "C:\\Users\\cultu\\source\\repos\\udp2_client\\udp2_client";


WSADATA wsaData;                //��ʼ������
SOCKET clientSocket;

time_t tt;                      //��־ʹ�õ�ʱ���
char t[32] = { 0 };

struct sockaddr_in local;       //����IP�Ͷ˿���Ϣ

struct sockaddr_in servAddr;    //������IP�Ͷ˿���Ϣ

struct sockaddr fromAddr;       //���յ��İ�����Դ��ַ��Ϣ
int addrLen = sizeof(fromAddr);

clock_t start, end;             //��ʱ�ش�

char buffer[20000][BUF_SIZE];   //�ļ�������

int changeStateSeq = 0;                //�����л�״̬ʱ�õ����кţ�����START��SYN��FIN������0��ʼ

int base = 0, nextseqnum = 0;   //��������


void print2(int a);
#pragma pack(1)//��1byte����
class packet {
public:
    int flag;//��־λ
    int seq;//���к�
    int ackseq;//Ӧ�����к�
    int filenum;//�ļ��İ�����
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
            //��ʱ������checksum�ֶε�ֵ
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


