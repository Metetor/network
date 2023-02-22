#pragma once

#include <winsock.h>
#include <fstream>
#include <vector>
#include "rdt3.h"
using namespace std;
vector<packet> sendpkts;
static bool stopTimer = false;
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

    int syn = a.getSyn();
    int ack = a.getAck();
    int start = a.getSF();
    int fin = a.getFin();
    u_short checksum = a.checksum;
    printf("[%s]  SYN:%d ACK:%d msgseq:%d acknum:%d \n", type,syn, ack, a.seq, a.ackseq);
}


//读入文件,pktNum表示该文件分成几段进行传输，length表示最后一段的长度
void readfile(char* name, char buffer[20000][BUF_SIZE], int& length, int& pktNum)
{
    pktNum = 0;
    length = 0;
    char path[100];
    memcpy(path, FILE_PATH, sizeof(FILE_PATH));
    strcat(path, (char*)"\\");
    strcat(path, name);
    //printf("%s", path);
    ifstream fin(path, ifstream::binary);//以二进制方式读入文件
    if (!fin)
    {
        printf("无法读取文件！");
        return;
    }
    char t = fin.get();
    while (fin)
    {
        buffer[pktNum][length % BUF_SIZE] = t;
        length++;
        if (length % BUF_SIZE == 0)
        {
            pktNum++;
            length = 0;
        }
        t = fin.get();
    }
    printf("文件读取成功！\n");
    fin.close();
    for (int i = 0; i <= pktNum; i++) {
        packet temp;
        if (i == pktNum) {
            temp.datalen = length;
            memcpy(temp.data, buffer[i], length);
        }
        else {
            temp.datalen = BUF_SIZE;
            memcpy(temp.data, buffer[i], BUF_SIZE);
        }
        temp.seq = i;
        temp.setChecksum();
        sendpkts.push_back(temp);
    }
    printf("该文件分片数目为：%d\n", pktNum);
};
DWORD WINAPI ACKHandler(LPVOID param) {
    //接收ACK
    packet temp;
    while (recvfrom(clientSocket, (char*)&temp, sizeof(temp), 0, &fromAddr, &addrLen) > 0) {
        log(temp, 2);
        if (temp.checkChecksum()) {
            if (temp.ackseq >= base && temp.ackseq < base + N) {
                base = temp.ackseq + 1;
                if (base == nextseqnum) {
                    stopTimer = false;  //关闭时钟
                    return 0;
                }
                else {
                    start = clock();
                    stopTimer = true;      //重启时钟
                    //count = 0;
                }
            }
            else {
                printf("无效的ACK序列号！\n");
            }
        }
        else {
            printf("校验和错误！\n");
        }
    }
};

int sendfile(char* name)
{
    //读取文件
    printf("开始发送文件！\n");
    int length = 0;//最后一个文件的长度
    int pktNum = 0;//文件个数
    readfile(name, buffer, length, pktNum);
    clock_t timestart = clock();
    //发送SF包
    printf("[SF] 发送SF包\n");
    packet startpkt;
    startpkt.setSF(pktNum);
    startpkt.datalen = strlen(name);
    startpkt.seq = changeStateSeq++;
    memcpy(startpkt.data, name, strlen(name));
    startpkt.setChecksum();
    printf("---------------------%s----------\n",name);
    sendto(clientSocket, (char*)&startpkt, sizeof(startpkt), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
    log(startpkt, 1);
    printf("[TF] 发送TF包\n");
    packet ackstart;
    int count = 0;
    while (1) {
        start = clock();
        while (recvfrom(clientSocket, (char*)&ackstart, sizeof(ackstart), 0, &fromAddr, &addrLen) <= 0) {
            //边界条件
            if (count >= MAX_TIMES) {
                tt = time(0);
                strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));
                printf("[%s] 重传次数过多，发送失败！\n", t);
                return 0;
            }
            if ((clock() - start) > TIME_OUT) {
                count++;
                sendto(clientSocket, (char*)&startpkt, sizeof(startpkt), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
                log(startpkt, 1);
                printf("超时重传第 %d 次！\n", count);
                start = clock();
                return 0;
            }
        }
        if (ackstart.getAck() && ackstart.ackseq == startpkt.seq && ackstart.checkChecksum()) {
            printf("成功发送传输通知！\n");
            log(ackstart, 2);
            break;
        }
        else {
            printf("不是START的ACK！\n");
            printf("check:%d\n", ackstart.checkChecksum());
        }
    }


    //发送文件内容
    printf("[TF] 发送文件内容\n");
    int i = 0;
    packet temp;                //用来接收ACK
    //记录时钟是否有效
    count = 0;                  //记录重传次数
    HANDLE ackhandler = CreateThread(nullptr, 0, ACKHandler, LPVOID(&socket), 0, nullptr);
    while (1) {
        //printf("pktNum:%d\n", pktNum);
        if (base > pktNum) {
            break;
        }

        //滑动窗口有空闲,发送包
        if (base <= nextseqnum && nextseqnum < (base + N) && nextseqnum <= pktNum) {
            sendto(clientSocket, (char*)&sendpkts[nextseqnum], sizeof(sendpkts[nextseqnum]), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
            log(sendpkts[nextseqnum], 1);
            if (base == nextseqnum) {
                start = clock();    //设置时钟
                count = 0;
                stopTimer = true;
            }
            nextseqnum++;
            //printf("%d %d\n", base, nextseqnum);
        }while (recvfrom(clientSocket, (char*)&temp, sizeof(temp), 0, &fromAddr, &addrLen) > 0) {
            log(temp, 2);
            if (temp.checkChecksum()) {
                if (temp.ackseq >= base && temp.ackseq < base + N) {
                    base = temp.ackseq + 1;
                    if (base == nextseqnum) {
                        stopTimer = false;     //关闭时钟
                    }
                    else {
                        start = clock();
                        stopTimer = true;      //重启时钟
                        count = 0;
                    }
                }
                else {
                    printf("无效的ACK序列号！\n");
                }
            }
            else {
                printf("校验和错误！\n");
            }
        }
        
        /*	
        
        
        if (!stopTimer && clock() - start >= MAX_WAIT_TIME) {
			cout << "[TIME OUT]\n";
			for (int i = 0; i < (int)waitingNum(nextSeqNum); i++)
			{
				sendpak(socket, addr, sendPkt[base+i]);
			}
			start = clock();
			stopTimer = false;
		}
        
        */
   



        //超时重传 timeout
        if (stopTimer && clock() - start > TIME_OUT) {
            if (count >= MAX_TIMES) {
                printf("重传次数过多，文件传输失败！\n");
                return 0;
            }
            printf("开始超时重传！\n");
            start = clock();
            stopTimer = true;
            for (int j = base; j < nextseqnum; j++) {
                sendto(clientSocket, (char*)&sendpkts[j], sizeof(sendpkts[j]), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
                log(sendpkts[j], 1);
            }
            count++;
        }
    }
    //吞吐率计算
    clock_t timeend = clock();
    double totaltime = (double)(timeend - timestart) / CLOCKS_PER_SEC;
    printf("传输总时间:%f\n", totaltime);
    printf("吞吐率: %f Mbps\n", (double)(pktNum + 1) * sizeof(packet) * 8 / totaltime / 1024 / 1024);
    //printf("文件发送完毕!\n");
    base = 0;
    nextseqnum = 0;
    vector <packet>().swap(sendpkts);
    return 1;//发送成功
}


int buildConn() {
    packet pkt, pkt2;
    pkt.setSyn();
    //pkt.setAck();
    pkt.seq = changeStateSeq++;
    pkt.setChecksum();

    //memcpy(sendbuf, &pkt, sizeof(pkt));
    sendto(clientSocket, (char*)&pkt, sizeof(pkt), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
    log(pkt, 1);

    //记录重传次数
    int count = 0;
    while (1) {
        start = clock();
        while (recvfrom(clientSocket, (char*)&pkt2, sizeof(pkt2), 0, &fromAddr, &addrLen) <= 0) {
            if (count >= MAX_TIMES) {
                tt = time(0);
                strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));
                printf("[%s] 连接建立失败！\n", t);
                return 0;
            }
            if ((clock() - start) > TIME_OUT) {
                count++;
                printf("超时重传第 %d 次！\n", count);
                sendto(clientSocket, (char*)&pkt, sizeof(pkt), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
                log(pkt, 1);
                start = clock();
            }
        }
        log(pkt2, 2);
        if (pkt2.getAck() && pkt2.ackseq == pkt.seq && pkt2.checkChecksum()) {
            //state = 1;

            tt = time(0);
            strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));
            printf("[%s] 连接建立成功！\n", t);
            return 1;
        }
        else {
            printf("不是该SYN的ACK！\n");
            printf("check:%d\n", pkt2.checkChecksum());
        }
    }
}

int disConn() {
    packet pkt, pkt2;
    pkt.setFin();
    pkt.seq = changeStateSeq++;
    pkt.setChecksum();

    sendto(clientSocket, (char*)&pkt, sizeof(pkt), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
    log(pkt, 1);

    int count = 0;
    while (1) {
        start = clock();
        while (recvfrom(clientSocket, (char*)&pkt2, sizeof(pkt2), 0, &fromAddr, &addrLen) <= 0) {
            if (count >= MAX_TIMES) {
                tt = time(0);
                strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));
                printf("[%s] 连接断开失败！\n", t);
                return 0;
            }
            if ((clock() - start) > TIME_OUT) {
                count++;
                sendto(clientSocket, (char*)&pkt, sizeof(pkt), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
                printf("超时重传第 %d 次！\n", count);
                log(pkt, 1);
                start = clock();
            }
        }
        log(pkt2, 2);
        if (pkt2.getAck() && pkt2.ackseq == pkt.seq && pkt2.checkChecksum()) {
            //state = 1;

            tt = time(0);
            strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));
            printf("[%s] 连接断开成功！\n", t);
            return 1;
        }
        else {
            printf("不是FIN的ACK！\n");
            printf("check:%d\n", pkt2.checkChecksum());
        }
    }
}


int clientInit() {
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
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);//UDP格式数据报
    if (SOCKET_ERROR == clientSocket) {
        printf("[%s] socket创建失败!\n", t);
        WSACleanup();
        return -1;
    }
    printf("[%s] socket创建完成!\n", t);

    //if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == -1) {
    //    printf("setsockopt失败!\n");
    //    return -1;
    //}
    //printf("setsockopt成功!\n");

    //将发送端设置为非阻塞
    u_long iMODE = 1;//1为非阻塞，0为阻塞
    ioctlsocket(clientSocket, FIONBIO, &iMODE);//设置recvfrom是否为阻塞

    //服务器地址信息
    memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
    servAddr.sin_family = PF_INET;
    printf("选择发送目标：[1]server端 [2]路由器\n");
    int x = 0;
    scanf("%d", &x);
    if (x == 1) {
        servAddr.sin_addr.s_addr = inet_addr("127.0.0.3");
        servAddr.sin_port = htons(33333);
    }
    else if (x == 2) {
        servAddr.sin_addr.s_addr = inet_addr("127.0.0.2");
        servAddr.sin_port = htons(22222);
    }
    else {
        return 0;
    }

    memset(&local, 0, sizeof(local));  //每个字节都用0填充
    local.sin_family = PF_INET;  //使用IPv4地址
    local.sin_addr.s_addr = inet_addr("127.0.0.1");
    local.sin_port = htons(11111);  //端口

    tt = time(0);
    strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));

    //绑定
    int r = bind(clientSocket, (sockaddr*)&local, sizeof(local));
    if (r != 0) {
        printf("[%s] bind失败!\n", t);
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }
    printf("[%s] bind完成!\n", t);
}