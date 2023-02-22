#include"FILE.h"
#include"pak.h"
#include"conf.h"
#include<fstream>
void outfile(char* name, char buffer[65536][1024], int pktNum,int length)
{
	std::ofstream out(name, std::ofstream::binary);
	for (int i = 0; i <pktNum; i++)
	{
		for (int j = 0; j < 1024; j++)
			out << buffer[i][j];
	}
	for (int j = 0; j < length; j++)
		out << buffer[pktNum][j];
	out.close();
}
int recvfile()
{
	int expectednumseq=0;
	int length=0;
	packet pak, pak2;
	packet lastACK;
	int i = 0;
	while (1)
	{
		//expectednumseq= sendseq;
		memset(&pak, 0, sizeof(pak));
		memset(&pak2, 0, sizeof(pak2));
		memset(recvBuffer[i], 0, sizeof(recvBuffer[i]));
		recvpak(sockSer, addrCli, pak);

		
		if (pak.get_flag(TF)&&pak.checkchecksum(sizeof(pak)))
		{
			printf("收到第%d个包 \n", i);
			if (pak.get_seq() == expectednumseq)
			{
				pak.cpdata(recvBuffer[i],length);
				pak2.set_flag(ACK);
				pak2.set_ack(pak.get_seq() + 1);
				pak2.set_seq(sendseq++);
				memcpy(&lastACK, &pak2, sizeof(packet));
				sendpak(sockSer, addrCli, pak2);
				expectednumseq++;
				i++;
			}
			else
			{
				printf("数据传输乱序\n");
				sendpak(sockSer, addrCli, lastACK);
			}
		}
		if (pak.get_flag(EF) && pak.checkchecksum(sizeof(pak)))
		{
			printf("接收到EF包\n");
			packet pak1;
			pak1.set_flag(ACK);
			pak1.set_ack(pak.get_seq() + 1);
			pak1.set_seq(sendseq++);
			sendpak(sockSer, addrCli, pak1);
			break;
		}
	}
	outfile(name, recvBuffer, i-1,length);
	return 1;
}