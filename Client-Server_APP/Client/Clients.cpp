#include "Clients.h"

int Clients::countConn = 0;
using namespace std;
Clients::Clients()
{
	cout << "Enter a port: "; cin >> port;
	cin.clear();
	fflush(stdin);
	cin.ignore(INT_MAX, '\n');
	erStat = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (erStat != 0)
	{
		cout << "Error WinSock version initializaion " << WSAGetLastError() << endl;
		throw;
	}
	else
	{
		cout << "WinSock initialization is OK" << endl;
	}

	//Создание сокета
	ClientSock = socket(AF_INET, SOCK_STREAM, 0); //AF_INET - семейство IPv4, SOCK_STREAM - TCP протокол
	if (ClientSock == INVALID_SOCKET)
	{
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		throw;
	}
	else
	{
		cout << "Server socket initialization is OK" << endl;
	}

	//Привязка сокета к паре IP-адрес/Порт
	in_addr ip_to_num;
	erStat = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);
	if (erStat <= 0)
	{
		cout << "Error in IP translation to special numeric format" << endl;
		throw;
	}

	sockaddr_in clientInfo;
	ZeroMemory(&clientInfo, sizeof(clientInfo));

	clientInfo.sin_family = AF_INET;
	clientInfo.sin_addr = ip_to_num;
	clientInfo.sin_port = htons(port);

	erStat = connect(ClientSock, (sockaddr*)&clientInfo, sizeof(clientInfo));
	if (erStat != 0)
	{
		cout << "Connection to Server is FAILED.Error: " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		throw;
	}
	else
	{
		cout << "Connection established SUCCESSFULLY. Ready to send a message to Server" << endl;
	}
	countConn++;
}

Clients::~Clients()
{
	shutdown(ClientSock, SD_BOTH);
	closesocket(ClientSock);
	WSACleanup();
	countConn--;
}

string getTimeStr(string msg)
{
	time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
	string s(30, '\0');
	strftime(&s[0], s.size(), "[%Y-%m-%d %H:%M:%S.%MS] ", localtime(&now));
	return s.append(msg);
}

void Clients::Send()
{
	short packet_size = 0;
	char msg[512];
	int time = 0;
	ZeroMemory(&msg, sizeof(msg));
	std::string s;
	char t[512] = { '\0' };
	mutex m;
	
	cout << "Enter your name: "; fgets(msg, sizeof(msg), stdin);
	cout << "Enter your time (seconds): "; cin >> time;
	
	while (true) {
		s = getTimeStr((string)msg);
		memset(&t, '\0', sizeof(t));
		int i = 0;
		for (auto el : s)
		{
			if (el != '\0' && el != '\n')
			{
				t[i] = el;
				i++;
			}
		}
		m.lock();
		packet_size = send(ClientSock, (const char*)&t, sizeof(t), 0);
		m.unlock();
		this_thread::sleep_for(chrono::seconds(time));
		cout << "Send " << packet_size << " Bytes" << endl;
		if (packet_size == INVALID_SOCKET)
		{
			cout << "Can't send message to Server. Error: " << WSAGetLastError() << endl;
			closesocket(ClientSock);
			WSACleanup();
			throw;
		}
	}
}


void Clients::Send(std::string msg, int time)
{
	mutex m;
	short packet_size = 0;
	std::string s;
	char t[512] = {'\0'};
	while (true) {
		memset(&t, '\0', sizeof(t));
		int i = 0;
		s = getTimeStr(msg);
		for(auto el : s)
		{
			if (el != '\0')
			{
				t[i] = el;
				i++;
			}
		}
		m.lock();
		packet_size = send(ClientSock, (const char*)&t, sizeof(t), 0);
		m.unlock();
		this_thread::sleep_for(chrono::seconds(time));
		cout << "Send " << packet_size << " Bytes" << endl;
		if (packet_size == INVALID_SOCKET)
		{
			cout << "Can't send message to Server. Error: " << WSAGetLastError() << endl;
			closesocket(ClientSock);
			WSACleanup();
			throw;
		}
	}
}



int Clients::GetcounConn()
{
	return countConn;
}


