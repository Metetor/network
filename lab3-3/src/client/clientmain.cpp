#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#define _CRT_NONSTDC_NO_DEPRECATE
#pragma warning( disable : 4996 )
#include<iostream>
#include<winsock.h>
#include<string>
#include"init.h"
#include"client.h"
using namespace std;
int main() {
	init();
	while (1)
	{
		FSM();
	}
	Sleep(20);
}
