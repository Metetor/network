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
    //ʱ��
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
        strcpy(type, "δ֪");
    }

    int syn = a.getSyn();
    int ack = a.getAck();
    int start = a.getSF();
    int fin = a.getFin();
    u_short checksum = a.checksum;
    printf("[%s]  SYN:%d ACK:%d msgseq:%d acknum:%d \n", type,syn, ack, a.seq, a.ackseq);
}


//�����ļ�,pktNum��ʾ���ļ��ֳɼ��ν��д��䣬length��ʾ���һ�εĳ���
void readfile(char* name, char buffer[20000][BUF_SIZE], int& length, int& pktNum)
{
    pktNum = 0;
    length = 0;
    char path[100];
    memcpy(path, FILE_PATH, sizeof(FILE_PATH));
    strcat(path, (char*)"\\");
    strcat(path, name);
    //printf("%s", path);
    ifstream fin(path, ifstream::binary);//�Զ����Ʒ�ʽ�����ļ�
    if (!fin)
    {
        printf("�޷���ȡ�ļ���");
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
    printf("�ļ���ȡ�ɹ���\n");
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
    printf("���ļ���Ƭ��ĿΪ��%d\n", pktNum);
};
DWORD WINAPI ACKHandler(LPVOID param) {
    //����ACK
    packet temp;
    while (recvfrom(clientSocket, (char*)&temp, sizeof(temp), 0, &fromAddr, &addrLen) > 0) {
        log(temp, 2);
        if (temp.checkChecksum()) {
            if (temp.ackseq >= base && temp.ackseq < base + N) {
                base = temp.ackseq + 1;
                if (base == nextseqnum) {
                    stopTimer = false;  //�ر�ʱ��
                    return 0;
                }
                else {
                    start = clock();
                    stopTimer = true;      //����ʱ��
                    //count = 0;
                }
            }
            else {
                printf("��Ч��ACK���кţ�\n");
            }
        }
        else {
            printf("У��ʹ���\n");
        }
    }
};

int sendfile(char* name)
{
    //��ȡ�ļ�
    printf("��ʼ�����ļ���\n");
    int length = 0;//���һ���ļ��ĳ���
    int pktNum = 0;//�ļ�����
    readfile(name, buffer, length, pktNum);
    clock_t timestart = clock();
    //����SF��
    printf("[SF] ����SF��\n");
    packet startpkt;
    startpkt.setSF(pktNum);
    startpkt.datalen = strlen(name);
    startpkt.seq = changeStateSeq++;
    memcpy(startpkt.data, name, strlen(name));
    startpkt.setChecksum();
    printf("---------------------%s----------\n",name);
    sendto(clientSocket, (char*)&startpkt, sizeof(startpkt), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
    log(startpkt, 1);
    printf("[TF] ����TF��\n");
    packet ackstart;
    int count = 0;
    while (1) {
        start = clock();
        while (recvfrom(clientSocket, (char*)&ackstart, sizeof(ackstart), 0, &fromAddr, &addrLen) <= 0) {
            //�߽�����
            if (count >= MAX_TIMES) {
                tt = time(0);
                strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));
                printf("[%s] �ش��������࣬����ʧ�ܣ�\n", t);
                return 0;
            }
            if ((clock() - start) > TIME_OUT) {
                count++;
                sendto(clientSocket, (char*)&startpkt, sizeof(startpkt), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
                log(startpkt, 1);
                printf("��ʱ�ش��� %d �Σ�\n", count);
                start = clock();
                return 0;
            }
        }
        if (ackstart.getAck() && ackstart.ackseq == startpkt.seq && ackstart.checkChecksum()) {
            printf("�ɹ����ʹ���֪ͨ��\n");
            log(ackstart, 2);
            break;
        }
        else {
            printf("����START��ACK��\n");
            printf("check:%d\n", ackstart.checkChecksum());
        }
    }


    //�����ļ�����
    printf("[TF] �����ļ�����\n");
    int i = 0;
    packet temp;                //��������ACK
    //��¼ʱ���Ƿ���Ч
    count = 0;                  //��¼�ش�����
    HANDLE ackhandler = CreateThread(nullptr, 0, ACKHandler, LPVOID(&socket), 0, nullptr);
    while (1) {
        //printf("pktNum:%d\n", pktNum);
        if (base > pktNum) {
            break;
        }

        //���������п���,���Ͱ�
        if (base <= nextseqnum && nextseqnum < (base + N) && nextseqnum <= pktNum) {
            sendto(clientSocket, (char*)&sendpkts[nextseqnum], sizeof(sendpkts[nextseqnum]), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
            log(sendpkts[nextseqnum], 1);
            if (base == nextseqnum) {
                start = clock();    //����ʱ��
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
                        stopTimer = false;     //�ر�ʱ��
                    }
                    else {
                        start = clock();
                        stopTimer = true;      //����ʱ��
                        count = 0;
                    }
                }
                else {
                    printf("��Ч��ACK���кţ�\n");
                }
            }
            else {
                printf("У��ʹ���\n");
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
   



        //��ʱ�ش� timeout
        if (stopTimer && clock() - start > TIME_OUT) {
            if (count >= MAX_TIMES) {
                printf("�ش��������࣬�ļ�����ʧ�ܣ�\n");
                return 0;
            }
            printf("��ʼ��ʱ�ش���\n");
            start = clock();
            stopTimer = true;
            for (int j = base; j < nextseqnum; j++) {
                sendto(clientSocket, (char*)&sendpkts[j], sizeof(sendpkts[j]), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
                log(sendpkts[j], 1);
            }
            count++;
        }
    }
    //�����ʼ���
    clock_t timeend = clock();
    double totaltime = (double)(timeend - timestart) / CLOCKS_PER_SEC;
    printf("������ʱ��:%f\n", totaltime);
    printf("������: %f Mbps\n", (double)(pktNum + 1) * sizeof(packet) * 8 / totaltime / 1024 / 1024);
    //printf("�ļ��������!\n");
    base = 0;
    nextseqnum = 0;
    vector <packet>().swap(sendpkts);
    return 1;//���ͳɹ�
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

    //��¼�ش�����
    int count = 0;
    while (1) {
        start = clock();
        while (recvfrom(clientSocket, (char*)&pkt2, sizeof(pkt2), 0, &fromAddr, &addrLen) <= 0) {
            if (count >= MAX_TIMES) {
                tt = time(0);
                strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));
                printf("[%s] ���ӽ���ʧ�ܣ�\n", t);
                return 0;
            }
            if ((clock() - start) > TIME_OUT) {
                count++;
                printf("��ʱ�ش��� %d �Σ�\n", count);
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
            printf("[%s] ���ӽ����ɹ���\n", t);
            return 1;
        }
        else {
            printf("���Ǹ�SYN��ACK��\n");
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
                printf("[%s] ���ӶϿ�ʧ�ܣ�\n", t);
                return 0;
            }
            if ((clock() - start) > TIME_OUT) {
                count++;
                sendto(clientSocket, (char*)&pkt, sizeof(pkt), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
                printf("��ʱ�ش��� %d �Σ�\n", count);
                log(pkt, 1);
                start = clock();
            }
        }
        log(pkt2, 2);
        if (pkt2.getAck() && pkt2.ackseq == pkt.seq && pkt2.checkChecksum()) {
            //state = 1;

            tt = time(0);
            strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));
            printf("[%s] ���ӶϿ��ɹ���\n", t);
            return 1;
        }
        else {
            printf("����FIN��ACK��\n");
            printf("check:%d\n", pkt2.checkChecksum());
        }
    }
}


int clientInit() {
    tt = time(0);
    strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));

    //��ʼ��DLL
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("[%s] ��ʼ��ʧ��!\n", t);
        return -1;
    }
    printf("[%s] ��ʼ�����!\n", t);

    tt = time(0);
    strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));

    //����socket
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);//UDP��ʽ���ݱ�
    if (SOCKET_ERROR == clientSocket) {
        printf("[%s] socket����ʧ��!\n", t);
        WSACleanup();
        return -1;
    }
    printf("[%s] socket�������!\n", t);

    //if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == -1) {
    //    printf("setsockoptʧ��!\n");
    //    return -1;
    //}
    //printf("setsockopt�ɹ�!\n");

    //�����Ͷ�����Ϊ������
    u_long iMODE = 1;//1Ϊ��������0Ϊ����
    ioctlsocket(clientSocket, FIONBIO, &iMODE);//����recvfrom�Ƿ�Ϊ����

    //��������ַ��Ϣ
    memset(&servAddr, 0, sizeof(servAddr));  //ÿ���ֽڶ���0���
    servAddr.sin_family = PF_INET;
    printf("ѡ����Ŀ�꣺[1]server�� [2]·����\n");
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

    memset(&local, 0, sizeof(local));  //ÿ���ֽڶ���0���
    local.sin_family = PF_INET;  //ʹ��IPv4��ַ
    local.sin_addr.s_addr = inet_addr("127.0.0.1");
    local.sin_port = htons(11111);  //�˿�

    tt = time(0);
    strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", localtime(&tt));

    //��
    int r = bind(clientSocket, (sockaddr*)&local, sizeof(local));
    if (r != 0) {
        printf("[%s] bindʧ��!\n", t);
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }
    printf("[%s] bind���!\n", t);
}