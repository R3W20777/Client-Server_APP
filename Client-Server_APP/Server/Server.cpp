#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <Windows.h>
#include <fstream>
#include <conio.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

DWORD WINAPI ProcessClient(LPVOID lpParameter);
HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);
ofstream ost("log.txt");

int main()
{
	WSADATA wsaData;
	SOCKET ServSock, AcceptSocket = INVALID_SOCKET;
	in_addr ip_to_num;
	sockaddr_in servInfo;
	int erStat = 0;
	int port;

	cout << "Enter a port: "; cin >> port;
	cin.clear();
	fflush(stdin);
	cin.ignore(INT_MAX, '\n');
	erStat = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (erStat != 0)
	{
		cout << "Error WinSock version initializaion " << WSAGetLastError() << endl;
		return 1;
	}
	else
	{
		cout << "WinSock initialization is OK" << endl;
	}


	ServSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ServSock == INVALID_SOCKET)
	{
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
	{
		cout << "Server socket initialization is OK" << endl;
	}


	
	erStat = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);
	if (erStat <= 0)
	{
		cout << "Error in IP translation to special numeric format" << endl;
		return 1;
	}

	
	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(port);


	erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));
	if (erStat != 0)
	{
		cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
	{
		cout << "Binding socket to Server" << endl;
	}



	erStat = listen(ServSock, SOMAXCONN);
	if (erStat != 0)
	{
		cout << "Can't start to listen to. Error:  " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
	{
		cout << "Listening..." << endl;
	}

	while (true)
	{
		
		AcceptSocket = SOCKET_ERROR;

		while (AcceptSocket == SOCKET_ERROR)
		{
			AcceptSocket = accept(ServSock, NULL, NULL);
		}
		cout << "Client Connected.\n" << endl;

		DWORD dwThreadId;
		CreateThread(NULL, 0, ProcessClient, (LPVOID)AcceptSocket, 0, &dwThreadId);
	}

	ost.close();
	closesocket(ServSock);
	closesocket(AcceptSocket);
	WSACleanup();
	return 0;
	
}

// Создаем функцию которая принимает сообщения
DWORD WINAPI ProcessClient(LPVOID lpParameter)
{
	SOCKET AcceptSocket = (SOCKET)lpParameter;
	vector<char>servBuff(1024);
	int packet_size = 0;
	do
	{
		packet_size = recv(AcceptSocket, servBuff.data(), servBuff.size(), 0);
		if (packet_size > 0)
		{
			WaitForSingleObject(hMutex, INFINITE);
			cout << "Recived " << packet_size << " bytes" << endl;
			ost << servBuff.data() << endl;
			ReleaseMutex(hMutex);

		}
		else if (packet_size == 0)
		{
			cout << "Client disconected" << endl;
			shutdown(AcceptSocket, SD_BOTH);



		}
		else
		{
			cout << "Error: " << WSAGetLastError() << endl;
			return 1;
		}
	} while (packet_size > 0);
}