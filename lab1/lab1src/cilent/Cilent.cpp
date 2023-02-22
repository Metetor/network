#include<iostream>
#include<string>
#include<winsock.h>
#include<ctime>
#include <iomanip>
#include"init.h"
#pragma comment(lib,"ws2_32.lib")
char sendBuf[2048];
char recvBuf[2048];
using namespace std;
DWORD WINAPI handlerRequest(LPVOID lparam);
int main() {
	//WSA初始化
	if (WSAinit() == 0)
	{
		printf("WSAStartup Complete\n");

	}
	else
	{
		perror("WSAinit");
		exit(EXIT_FAILURE);
	}
	char username[50];
	SOCKADDR_IN addrSer;//服务端地址
	SOCKADDR_IN addrCilent;
	SOCKET sockCilent = socket(AF_INET, SOCK_STREAM, 0);
	int len = sizeof(SOCKADDR);
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(6666);
	addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	SOCKET sockSer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	printf("Socket Created\n");
		
//connect
	if (connect(sockCilent, (SOCKADDR*)&addrSer, sizeof(SOCKADDR)) == -1)
	{
		perror("connect");
		//exit(EXIT_FAILURE);
	}
	else
		printf("Connect to Server ...\n");
	//创建线程，用来处理server发来的信息
	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL, NULL, handlerRequest, LPVOID(sockCilent), 0, &dwThreadId);
	int cnt = 0;
	while (1) {

		//信息格式:%user_name%nowtime%msg_len%msg_type%msg%
		//string msg;
		char text[1024];
		/*if (msg == "exit")
			closesocket(sockCilent);*/
		if (!cnt)
		{
			printf("Please Enter Your UserName [50 chars LIMIT]:");
			cin.getline(username,'/n');
			memcpy(USERA.usrname,username,sizeof(username));
			//发送消息来获得usrid
			struct INFO initinfo;
			//memcpy(initinfo.infoType, INIT, sizeof("INIT"));
			initinfo.infoType = JOIN;
			memcpy(initinfo.text, username, sizeof(username));
			//cout << initinfo.text << endl;
			memset(sendBuf, 0, sizeof(sendBuf));
			memcpy(sendBuf, &initinfo, sizeof(initinfo));
			int ret=send(sockCilent, sendBuf, sizeof(sendBuf), 0);
		}

		cout << USERA.usrname<<"> ";
		cin.getline(text,1024,'\n');

		struct INFO info;
		//memset(info, 0, sizeof(info));
		if (!strcmp(text, "sendto"))
		{
			int idtmp = 0;
			info.infoType = SENDTO;
			cout << "userID_to:";
			cin >> idtmp;
			string trash;
			getline(cin, trash);
			info.ID_to=idtmp;
			cout << "Content:";
			//cout << text << endl;
			memset(text, 0, sizeof(text));
			cin.getline(text, 1024, '\n');
			//cout << "Content:";
			memcpy(info.text, text, sizeof(text));
			//cout << info.text;
		}
		else if (!strcmp(text, "exit")) {
			cout << "exit" << endl;
			return 0;
		}
		else
		{
			memcpy(info.cmd, "SENDTOALL", sizeof("SENDTOALL"));
			info.infoType = SENDTOALL;
			info.len = strlen(text);
			memcpy(info.text, text, sizeof(text));
		}
		//msg = username + dt + to_string(text.length()) + "TEXT" + text;
		//cout << info.text << endl;
		memset(sendBuf, 0, sizeof(sendBuf));
		memcpy(sendBuf, &info, sizeof(info));
		//cout << (char*)&sendBuf << endl;
		if (send(sockCilent, sendBuf, sizeof(sendBuf), 0) == -1)
		{
			perror("Cilent Send");
			exit(EXIT_FAILURE);
		}
		cnt++;
		//cout << "send after\n";
	}
	WSACleanup();
	return 0;
}
DWORD WINAPI handlerRequest(LPVOID lparam) {
	int cnt = 0;
	while (1) {
		//cout << cnt << endl;
		SOCKET sockCilent = (SOCKET)(LPVOID)lparam;
		memset(recvBuf, 0, sizeof(recvBuf));//
		if (recv(sockCilent, recvBuf, sizeof(recvBuf), 0) == -1) {
			return 0;
		}
		//cout << "recv something..." << endl;
		struct INFO info;
		memset(&info, 0, sizeof(info));
		memcpy(&info, recvBuf, sizeof(info));
		//cout << "\ninfotype:" << info.infoType << endl;
		//cout << "\ninfotext:"<<info.text << endl;
		switch(info.infoType)
		{
		case INIT://绑定usrname和usrid
		{
			//cout << '\n'<<cnt << endl;
			//USERA.usrid = info.ID_from;
			//cout << "\n"<<USERA.usrid << endl;
		}	
		break;
		case FORWARD://转发
		{
			info.infoType = RECV;
			info.print_std();
		}
		break;
		case JOIN:
			info.print_std();
			break;
		case OK:
			break;
		default:
			break;
		}
		cnt++;
		//cout << setiosflags(ios::right) << string(buf) << endl;
	}
	return 0;
}
