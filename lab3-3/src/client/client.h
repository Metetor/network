#ifndef __CLIENT_H__
#define __CLIENT_H__
#include"conf.h"
#include "pak.h"
#include"rdt3.h"
#include"FILE.h"
int buildConnect()
{
	extern int sendseq;
	packet startpak,rpak;
	startpak.set_flag(SYN);
	startpak.set_seq(sendseq++);
	if (stopWaitsend(startpak, rpak))
		return 1;
	else
		return 0;
}
int disConnect()
{
	packet endpak, rpak;
	endpak.set_flag(FIN);
	if (stopWaitsend(endpak, rpak))
		return 1;
	else
		return 0;
}
int FSM()
{
	switch (status)
	{
	case 0:
		printf("-------------------尝试建立连接-------------------\n");
		//int conret = buildConnect();
		if (buildConnect())
		{
			printf("建立连接成功\n");
			status = 1;
		}
		else
			printf("建立连接失败\n");
		break;
	case 1:
		printf("-------------------文件传输-------------------\n");
		//输入文件名
		char name[50];
		printf("[FILE,INPUT]  请输入文件名:");
		scanf("%s",&name);
		sendfile(name);
		break;
	}
	return 1;
}
#endif