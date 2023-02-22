#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include "rdt3.h"
#include "server.h"
using namespace std;
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll

int main() {

    //初始化
    serverInit();

    int state = 0;

    packet pkt, pkt2;

    while (1) {
        memset(&pkt, 0, sizeof(packet));
        memset(&pkt2, 0, sizeof(packet));
        switch (state) {
            //********************建立连接********************
        case 0:
            printf("********************等待建立连接********************\n");
            recvfrom(serverSocket, (char*)&pkt, sizeof(pkt), 0, &clntAddr, &nSize);
            log(pkt, 2);
            //memcpy(&pkt, recvbuf, sizeof(pkt));
            if (pkt.getSyn() && pkt.checkChecksum() && pkt.seq == changeStateSeq) {
                pkt2.setAck();
                pkt2.acknum = changeStateSeq++;
                pkt2.setChecksum();
                state = 1;
                sendto(serverSocket, (char*)&pkt2, sizeof(pkt2), 0, &clntAddr, nSize);
                log(pkt2, 1);

                tt = time(0);
                strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));
                printf("[%s] 连接建立成功！\n", t);
            }
            break;
        case 1:
            //Sleep(100);
            printf("********************等待接收START********************\n");

            recvfrom(serverSocket, (char*)&pkt, sizeof(pkt), 0, &clntAddr, &nSize);
            log(pkt, 2);
            if (pkt.getStart() && pkt.checkChecksum() && pkt.seq == changeStateSeq) {
                pktNum = pkt.filenum;
                memset(name, 0, sizeof(name));
                memcpy(name, pkt.data, pkt.datalen);
                printf("文件名:%s\n", name);
                //Sleep(100000);
                pkt2.setAck();
                pkt2.acknum = changeStateSeq++;
                pkt2.setChecksum();
                state = 2;
                sendto(serverSocket, (char*)&pkt2, sizeof(pkt2), 0, &clntAddr, nSize);
                log(pkt2, 1);
            }
            break;
        case 2:
            printf("开始接收文件内容\n");
            recvfile();
            state = 3;

            break;
        case 3:
            printf("********************等待断开连接********************\n");
            recvfrom(serverSocket, (char*)&pkt, sizeof(pkt), 0, &clntAddr, &nSize);
            log(pkt, 2);
            if (pkt.getFin() && pkt.checkChecksum() && pkt.seq == changeStateSeq) {
                pkt2.setAck();
                pkt2.acknum = changeStateSeq++;
                pkt2.setChecksum();
                sendto(serverSocket, (char*)&pkt2, sizeof(pkt2), 0, &clntAddr, nSize);
                log(pkt2, 1);

                tt = time(0);
                strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));
                printf("[%s] 连接断开成功！\n", t);
                changeStateSeq = 0;
            }
            else {
                printf("fin:%d check:%d seq:%d\n", pkt.getFin(), pkt.checkChecksum(), pkt.seq == changeStateSeq);
            }
            state = 0;
            break;
        }
    }

    //关闭套接字
    closesocket(serverSocket);
    //释放DLL资源
    WSACleanup();
    return 0;
}