#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include <stdio.h>
#include <WinSock2.h>
#include <windows.h>
#include <time.h>
#include "rdt3.h"
#include <vector>
#include "cilent.h"
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll

//描述发送端的状态
int state = 0;
char name[30];
int choose = 0;

int main() {
    //发送端的初始化
    clientInit();

    while (1) {
        switch (state) {
        case 0:
            //选择文件
            printf("请选择发送文件\n");
            printf("[0] 退出 [1] %s [2] %s [3] %s\n", filenames[0], filenames[1],filenames[2]);

            scanf("%d", &choose);
            //printf("%d\n", choose);
            if (choose < 0 || choose > 4) {
                printf("选择错误！\n");
                break;
            }
            else if (choose == 0) {
                return 0;
            }
            else {
                memset(name, 0, 30);
                strcpy(name, filenames[choose - 1]);
                state = 1;
            }
            break;
        case 1:
            //建立连接
            printf("[CONNECT]  建立连接\n");
            if (!buildConn()) {
                printf("[ERROR]  连接建立失败！\n");
                state = 0;
                break;
            }
            state = 2;
            //Sleep(100000);
            //system("pause");
            break;
        case 2:
            //传输文件
            printf("[TF] 传输文件...\n");
            if (sendfile(name))
            {
                tt = time(0);
                strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));

                printf("[%s]  %s  发送成功\n", t, name);
                state = 3;
            }
            else {
                tt = time(0);
                strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));

                printf("[%s]  %s  发送失败\n", t, name);
                //state = 3;
                return 0;
            }
            break;
        case 3:
            //断开连接
            printf("[disCON]  断开连接\n");
            if (!disConn()) {
                return 0;
            }
            changeStateSeq = 0;
            state = 0;
            break;
        }
    }

    //释放资源
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}