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
#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll

//�������Ͷ˵�״̬
int state = 0;
char name[30];
int choose = 0;

int main() {
    //���Ͷ˵ĳ�ʼ��
    clientInit();

    while (1) {
        switch (state) {
        case 0:
            //ѡ���ļ�
            printf("��ѡ�����ļ�\n");
            printf("[0] �˳� [1] %s [2] %s [3] %s\n", filenames[0], filenames[1],filenames[2]);

            scanf("%d", &choose);
            //printf("%d\n", choose);
            if (choose < 0 || choose > 4) {
                printf("ѡ�����\n");
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
            //��������
            printf("[CONNECT]  ��������\n");
            if (!buildConn()) {
                printf("[ERROR]  ���ӽ���ʧ�ܣ�\n");
                state = 0;
                break;
            }
            state = 2;
            //Sleep(100000);
            //system("pause");
            break;
        case 2:
            //�����ļ�
            printf("[TF] �����ļ�...\n");
            if (sendfile(name))
            {
                tt = time(0);
                strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));

                printf("[%s]  %s  ���ͳɹ�\n", t, name);
                state = 3;
            }
            else {
                tt = time(0);
                strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));

                printf("[%s]  %s  ����ʧ��\n", t, name);
                //state = 3;
                return 0;
            }
            break;
        case 3:
            //�Ͽ�����
            printf("[disCON]  �Ͽ�����\n");
            if (!disConn()) {
                return 0;
            }
            changeStateSeq = 0;
            state = 0;
            break;
        }
    }

    //�ͷ���Դ
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}