#include"init.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;
void WSAinit();
void SendToAll(char* msg);
DWORD WINAPI handlerRequest(LPVOID lparam);
extern vector<user> usrlist;//�û��б�
extern char sendBuf[2048];
extern char recvBuf[2048];
int main() {
	//���巢�ͻ������ͽ��ܻ�����
	int usrNum = 0;
	SOCKADDR_IN addrSer;//����˵�ַ
	SOCKADDR_IN addrClient;
	printf("Start Server Manager\n");
	WSAinit();
	//��ʼ��addrSer
	int len = sizeof(SOCKADDR);
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(6666);
	addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	SOCKET sockSer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (bind(sockSer, (SOCKADDR*)&addrSer, sizeof(SOCKADDR)) == -1)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}
	printf("Start Listening...\n");
	if (listen(sockSer, 5) == -1) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	while (1) {
		usrNum++;
		string msg;
		string text;		
		SOCKET CilentSocket = accept(sockSer, (SOCKADDR*)&addrClient, &len);
		printf("Start Accept request...\n");
		user usra(usrNum,CilentSocket);
		//cout << "usrid:" << usra.usrid << endl;
		//memcpy(usra.usrname, msg.text, sizeof(usra.usrname));
		usrlist.push_back(struct user(usra));
		DWORD dwThreadId;
		HANDLE hThread = CreateThread(NULL, NULL, handlerRequest, LPVOID(CilentSocket), 0, &dwThreadId);
		//CloseHandle(hThread);	
	}
	WSACleanup();
}
DWORD WINAPI handlerRequest(LPVOID lparam) {
	SOCKET CilentSocket = (SOCKET)(LPVOID)lparam;
	struct INFO msg;
	int cnt = 0;
	while (1) {
		memset(&msg, 0, sizeof(msg));//��սṹ��
		memset(recvBuf, 0, 2048);//��ջ���
		if (recv(CilentSocket, recvBuf, sizeof(msg), 0) == -1) {
			perror("recv");
			break;
			//exit(EXIT_FAILURE);
		}
		//printf("recv something...\n");
		memcpy(&msg, recvBuf, sizeof(msg));
		//cout << msg.text << endl;
		switch (msg.infoType) {
		case JOIN://��ʼ����username��user����userid
		{
			/*cout << cnt << endl;
			user usra(CilentSocket);
			memcpy(usra.usrname, msg.text, sizeof(usra.usrname));
			usrlist.push_back(struct user(usra));
			msg.ID_from = usra.usrid;
			SendTo(usra.usrid, msg);*/
			//INFO msgw;
			//memset(&msgw.text, 0, sizeof(msgw.text));
			//msg.infoType = JOIN;
			//memcpy(msgw.text, msg.text, sizeof(msgw.text));
			strcat(msg.text, " Joins,Welcome\n");
			//cout << msg.text << endl;
			//msgw.len = strlen(msgw.text);
			msg.print_std();
			SendToAll(msg);
		}
		break;
		case SENDTOALL://Ⱥ��
		{
			msg.print_std();
			msg.infoType = FORWARD;
			msg.print_std();
			SendToAll(msg);
		}
		break;
		case SENDTO://˽��
		{
			msg.infoType = FORWARD;
			msg.print_std();
			SendTo(msg.ID_to,msg);
		}
		break;
		case OK:
		break;
		case EXIT:
			return 0;
		}
		cnt++;
	}
	return 0;
}
void WSAinit() {
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
		return;
	}
	return;
}
