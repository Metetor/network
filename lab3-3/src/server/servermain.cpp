#include<iostream>
#include"pak.h"
#include"rdt3.h"
#include "init.h"
using namespace std;
int main() 
{
	init();
	while (1)
	{
		packet b;
		recvpak(sockSer, addrCli, b);
		if (!FSM(b))
			break;
	}
	Sleep(20);
}