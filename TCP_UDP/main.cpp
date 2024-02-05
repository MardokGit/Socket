#pragma warning(disable : 4996)//отключает ошибку связанную с inet_addr
#pragma comment(lib,"Ws2_32.lib")
#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <string>
using namespace std;

enum Packet {
	P_ChatMessage,
	P_Test
};

void SServer();
void ClientS();
void ClientHandler(int index);
void CClientHandler();
bool ProcessPacket(Packet packettype);
bool ProcessPacket_Server(int index, Packet packettype);

SOCKET Connection;
SOCKET Connections[100];
int Counter = 0;

bool ProcessPacket_Server(int index, Packet packettype)
{
	switch (packettype)
	{
	case P_ChatMessage:
	{
		int msg_size;
		recv(Connections[index], (char*)&msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Connections[index], msg, msg_size, NULL);
		for (int i = 0; i < Counter; i++)
		{
			if (i == index)
			{
				continue;
			}

			Packet msgtype = P_ChatMessage;
			send(Connections[i], (char*)&msgtype, sizeof(Packet), NULL);
			send(Connections[i], (char*)&msg_size, sizeof(int), NULL);
			send(Connections[i], msg, msg_size, NULL);
		}
		delete msg;
		break;
	}
	case P_Test:
		cout << "Test packet" << endl;
		break;
	default:
		cout << "Unrecognized packet: " << packettype << endl;
		break;
	}
	return true;
}

bool ProcessPacket(Packet packettype) {
	switch (packettype) {
	case P_ChatMessage:
	{
		int msg_size;
		recv(Connection, (char*)&msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Connection, msg, msg_size, NULL);
		cout << msg;
		delete msg;
		break;
	}
	case P_Test:
		cout << "Test packet" << endl;
		break;
	default:
		cout << "Unrecognized packet: " << packettype << endl;
		break;

	}
	return true;
}

int main()
{
	int Server_or_Client;
	cin >> Server_or_Client;

	switch (Server_or_Client)
	{
	case 0:
		SServer();
		break;
	case 1:
		ClientS();
		break;
	default:
		break;
	}
	return 0;
}

void SServer()
{
	WSAData wsaData;
	WORD DLLVesrion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVesrion, &wsaData) != 0)
	{
		printf("Error\n");
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	for (int i = 0; i < 100; i++)
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

		if (newConnection == 0)
		{
			printf("Error connection to Server\n");
		}
		else
		{
			printf("Connection to Server Succes\n");
			string msg = "Hello. It's my first network program!";
			int msg_size = msg.size();
			Packet msgtype = P_ChatMessage;
			send(newConnection, (char*)&msgtype, sizeof(Packet), NULL);
			send(newConnection, (char*)&msg_size, sizeof(int), NULL);
			send(newConnection, msg.c_str(), msg_size, NULL);

			Connections[i] = newConnection;
			Counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);

			Packet testpacket = P_Test;
			send(newConnection, (char*)&testpacket, sizeof(Packet), NULL);
		}
	}

	system("pause");

}

void ClientS()
{
	WSAData wsaData;
	WORD DLLVesrion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVesrion, &wsaData) != 0)
	{
		printf("Error\n");
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)
	{
		printf("Error: failed connect to server\n");
		exit(1);
	}

	printf("Connected!\n");

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)CClientHandler, NULL, NULL, NULL);

	string msg1;
	while (true)
	{
		cin >> msg1;
		int msg_size = msg1.size();
		Packet packettype = P_ChatMessage;
		send(Connection, (char*)&packettype, sizeof(Packet), NULL);
		send(Connection, (char*)&msg_size, sizeof(int), NULL);
		send(Connection, msg1.c_str(), msg_size, NULL);
		Sleep(10);
	}

	system("pause");
}

void ClientHandler(int index) {
	Packet packettype;
	while (true) {
		recv(Connections[index], (char*)&packettype, sizeof(Packet), NULL);

		if (!ProcessPacket_Server(index, packettype)) {
			break;
		}
	}
	closesocket(Connections[index]);
}

void CClientHandler()
{
	Packet packettype;
	while (true) {
		recv(Connection, (char*)&packettype, sizeof(Packet), NULL);

		if (!ProcessPacket(packettype)) {
			break;
		}
	}
	closesocket(Connection);
}