#pragma once 
#define WIN32_LEAN_AND_MEAN 

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

//
#include "../Server/WSAHelper.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma warning(disable: 4996)

namespace Net
{
	class Client
	{
	private:
		const char* address;
		uint16_t port;

		//
		WSADataHelper wsa;
		SOCKET ConnectSocket;
		struct sockaddr_in service;
		struct sockaddr_in results;

	public:
		Client(const char* ipaddress, uint16_t port);
		~Client();
		bool connect_to_server(); 
		bool send_str (const std::string& text);
		bool recv_str(std::string& text);
	};
}

