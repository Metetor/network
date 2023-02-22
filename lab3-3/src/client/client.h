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
		printf("-------------------���Խ�������-------------------\n");
		//int conret = buildConnect();
		if (buildConnect())
		{
			printf("�������ӳɹ�\n");
			status = 1;
		}
		else
			printf("��������ʧ��\n");
		break;
	case 1:
		printf("-------------------�ļ�����-------------------\n");
		//�����ļ���
		char name[50];
		printf("[FILE,INPUT]  �������ļ���:");
		scanf("%s",&name);
		sendfile(name);
		break;
	}
	return 1;
}
#endif