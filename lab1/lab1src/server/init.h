#pragma once
#pragma warning (disable: 4996) 
#define INIT 0
#define SENDTOALL 1
#define SENDTO	2
#define FORWARD 3
#define OK 4
#define JOIN 5
#define RECV 6
#define EXIT 7
#include<vector>
#include<string>
#include<iostream>
#include<string>
#include<winsock.h>
using namespace std;
char InfoType[10][20] = { "INIT","SENDTOALL","SENDTO","FORWARD","OK","JOIN","RECV" };
void print_time(SYSTEMTIME systime);
struct INFO
{
	int ID_from; //发送者ID
	int ID_to; //接收者ID
	char cmd[10];//命令
	SYSTEMTIME systime;//nowtime
	int len; //发送的消息主体的长度
	char text[1024]; //消息主体
	int infoType;
	INFO() {//得到现在的时间
		time_t timep;
		struct tm* p;
		time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep
		p = localtime(&timep);
		GetLocalTime(&systime);
	}
	void print_std() {
		printf("%02d:%02d:%02d  ",
			systime.wHour, systime.wMinute, systime.wSecond);
		cout << "[ " << InfoType[infoType] << " ]";
		cout << "   " << text << '\n';
	}
};
struct user {
	char usrname[10];
	int usrid=0;
	SOCKET UserSocket;
	user() {
		usrid = 0;
		UserSocket = NULL;
	}
	user(int usrNum,SOCKET usersock) {
		usrid = usrNum;
		UserSocket = usersock;
	}
};
vector<user> usrlist;
char sendBuf[2048];
char recvBuf[2048];
SOCKET searchSock(int usrid);
SOCKET searchSock(char* username);
void SendToAll(INFO info) {
	//cout << "sendToall build up\n";
	//cout << "text"<<info.text << endl;
	//cout << "type"<<info.infoType << endl;
	vector<user>::iterator iter = usrlist.begin();

	for (; iter != usrlist.end(); iter++) {
		//cout << iter->usrid << "--id\n";
		if (iter->usrid != -1) {
			memset(sendBuf, 0, sizeof(sendBuf));//清空缓存
			memcpy(sendBuf, &info, sizeof(info));
			int ret=send(iter->UserSocket, sendBuf, sizeof(sendBuf), 0);
			if (ret == -1) {
				perror("SendToAll");
			}
			//cout << "status:" << ret << '\n' << endl;
		}
	}
	return;
}
void SendTo(SOCKET cilentSock, INFO info) {
	memset(sendBuf, 0, sizeof(sendBuf));//清空缓存
	memcpy(sendBuf, &info, sizeof(info));
	if(send(cilentSock, sendBuf, sizeof(sendBuf), 0)==-1)
		perror("SendToSomebody");
	return;
}
void SendTo(int usrid, INFO info) {
	//cout << "sendto:" << usrid << endl;
	SOCKET cilentSock = searchSock(usrid);
	return SendTo(cilentSock, info);
}
void SendTo(char * usrname, INFO info) {
	SOCKET cilentSock = searchSock(usrname);
	return SendTo(cilentSock, info);
}

SOCKET searchSock(int usrid) {
	vector<user>::iterator iter = usrlist.begin();

	for (; iter != usrlist.end(); iter++) {
		//cout << "idlist" << iter->usrid << endl;
		if (iter->usrid == usrid) {
			//cout << "find it" << endl;
			return iter->UserSocket;
		}
	}
	return 0;
}
SOCKET searchSock(char* username) {
	vector<user>::iterator iter = usrlist.begin();

	for (; iter != usrlist.end(); iter++) {
		if (!strcmp(iter->usrname,username)) {
			return iter->UserSocket;
		}
	}
	return 0;
}


