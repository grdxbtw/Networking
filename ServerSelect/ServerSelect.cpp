// server_select.cpp : ���� ���� �������� ������� "main". ����� ���������� � ������������� ���������� ���������.
//

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

//
#include "Server.h"

const char* addr = "127.0.0.1";

int main(int argc,  char* argv[])
{
	std::cout << "Start server!\n";
	const char* addres;
	uint16_t port;
	if (argc == 3)
	{
		addres = argv[1];
		port = std::atoi(argv[2]);
	}
	else
	{
		addres = addr;
		port = 27015;
	}

#if 1
	NetServer::Server serv(addres, port);

	if (serv.start())
	{
		std::cout << "server started\n";

		std::string command;
		do
		{
			std::getline(std::cin, command);
		} while (command == "q");
	}

#endif // 0
	
	return 0;
}




