#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <Windows.h>
#include <chrono>
#include <ctime>
#include <thread>
#include <mutex>
#include <string>



#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable : 4996)

class Clients
{
public:
	Clients();
	~Clients();
	void Send();
	void Send(std::string msg, int time);
	int GetcounConn();
private:
	WSADATA wsaData;
	SOCKET ClientSock;
	int erStat = 0;
	int port;
	static int countConn;

};

