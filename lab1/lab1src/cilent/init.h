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
#include<string>
#include<iostream>
#include<time.h>
using namespace std;
//void print_time(SYSTEMTIME sys);
char InfoType[10][20] = { "INIT","SENDTOALL","SENDTO","FORWARD","OK","JOIN","RECV" };
struct user {
	char usrname[50] = {};
	int usrid = 0;
}USERA;
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
		printf("\n");
		printf("%02d:%02d:%02d  ",
			systime.wHour, systime.wMinute, systime.wSecond);
		cout << "[ " << InfoType[infoType] << " ]";
		cout << "   "<<text<<'\n';
		cout << USERA.usrname << ">";
	}
};

int WSAinit();
int WSAinit() {
	WORD version = MAKEWORD(2, 2);
	WSAData wsadata;
	int error;
	error = WSAStartup(version, &wsadata);
	if (error != 0)
	{
		switch (error)
		{
		case WSASYSNOTREADY:
			printf("WSASYSNOTREADY");
			break;
		case WSAVERNOTSUPPORTED:
			printf("WSAVERNOTSUPPORTED");
			break;
		case WSAEINPROGRESS:
			printf("WSAEINPROGRESS");
			break;
		case WSAEPROCLIM:
			printf("WSAEPROCLIM");
			break;
		}
		return -1;
	}
	return 0;
}
/*void print_time(SYSTEMTIME sys) {
	printf(" %02d:%02d:%02d\n",sys.wHour, sys.wMinute, sys.wSecond);
}*/