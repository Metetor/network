#pragma once
#pragma once
#include <WinSock2.h>
#include <fstream>
#include "rdt3.h"
using namespace std;

void print2(int a) {
    char b[100];
    itoa(a, b, 2);
    printf("%s\n", b);
}

void log(packet a, int x) {
    //时间
    tt = time(0);
    strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));
    char type[5];
    if (x == 1) {
        strcpy(type, "Send");
    }
    else if (x == 2) {
        strcpy(type, "Recv");
    }
    else {
        strcpy(type, "未知");
    }
    //标志位
    int syn = a.getSyn();
    int ack = a.getAck();
    int start = a.getStart();
    int fin = a.getFin();
    u_short checksum = a.checksum;
    printf("[%s] SYN:%d ACK:%d msgseq:%d acknum:%d \n", type,syn, ack,a.seq, a.acknum);
}


int serverInit() {
    tt = time(0);
    strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));

    //初始化DLL

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("[%s] 初始化失败!\n", t);
        return -1;
    }
    printf("[%s] 初始化完成!\n", t);

    tt = time(0);
    strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));

    //创建socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (SOCKET_ERROR == serverSocket) {
        printf("[%s] socket创建失败!\n", t);
        WSACleanup();
        return -1;
    }
    printf("[%s] socket创建完成!\n", t);


    memset(&local, 0, sizeof(local));  //每个字节都用0填充
    local.sin_family = PF_INET;  //使用IPv4地址
    //local.sin_addr.s_addr = inet_addr("127.0.0.2");
    //local.sin_port = htons(22222);  //端口
    local.sin_addr.s_addr = inet_addr("127.0.0.3");
    local.sin_port = htons(33333);  //端口

    tt = time(0);
    strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));

    //绑定
    int r = bind(serverSocket, (sockaddr*)&local, sizeof(local));
    if (r != 0) {
        printf("[%s] bind失败!\n", t);
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }
    printf("[%s] bind完成!\n", t);
}

void outfile(char* name, char buffer[20000][BUF_SIZE], int length, int& pktNum)
{
    char path[100];
    memcpy(path, OUT_PATH, sizeof(OUT_PATH));
    strcat(path, (char*)"\\");
    strcat(path, name);
    printf("输出路径：%s\n", path);
    //以二进制方式写入文件
    ofstream fout(path, ofstream::binary);
    for (int i = 0; i < pktNum; i++)
    {
        for (int j = 0; j < BUF_SIZE; j++)
            fout << buffer[i][j];
    }
    for (int j = 0; j < length; j++)
        fout << buffer[pktNum][j];
    fout.close();
}


int recvfile()
{
    int expectednumseq = 0;
    packet pkt, pkt2;
    packet lastACK;
    for (int i = 0; i <= pktNum; )
    {

        memset(&pkt, 0, sizeof(packet));
        memset(&pkt2, 0, sizeof(packet));
        memset(buffer[i], 0, BUF_SIZE);

        recvfrom(serverSocket, (char*)&pkt, sizeof(pkt), 0, &clntAddr, &nSize);


        printf("收到第 %d 个包！\n", i);
        log(pkt, 2);
        lose++;
        if (pkt.checkChecksum()) {
            if (pkt.seq == expectednumseq) {
                memcpy(buffer[i], pkt.data, pkt.datalen);
                pkt2.setAck();
                pkt2.acknum = expectednumseq++;
                pkt2.setChecksum();
                memcpy(&lastACK, &pkt2, sizeof(packet));
                sendto(serverSocket, (char*)&pkt2, sizeof(pkt2), 0, &clntAddr, nSize);
                log(pkt2, 1);
                i++;
            }
            else {
                printf("数据传输乱序！\n");
                sendto(serverSocket, (char*)&lastACK, sizeof(lastACK), 0, &clntAddr, nSize);
                log(lastACK, 1);
            }
        }
        else {
            printf("校验和错误！\n");
        }
    }
    //将buffer中存储的内容输出到文件名为name的文件中
    outfile(name, buffer, pkt2.datalen, pktNum);
    return 1;
}
