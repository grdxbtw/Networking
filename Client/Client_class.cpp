#include "Client_class.h"

namespace Net
{
	Client::Client(const char* ipaddres, uint16_t port)
		:
		address(ipaddres),
		port(port),
		ConnectSocket(INVALID_SOCKET) {}


	Client::~Client()
	{
		// cleanup
		closesocket(ConnectSocket);
	}

	bool Client::connect_to_server()
	{
		if (!wsa.IsOk())
		{
			std::cerr << "Couldn't init wsa\n";
			return false;
		}

		service.sin_family = AF_INET; 
		service.sin_addr.s_addr = inet_addr(address);  //
		service.sin_port = htons(port);  // 

		int iResult; 

		ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
		if (ConnectSocket == INVALID_SOCKET)
		{
			std::cerr << "socket failed with error; " << WSAGetLastError() << std::endl;
			return false;
		}
		std::cout << "socket created!\n";
		 
		iResult = connect(ConnectSocket, (SOCKADDR*)&service, sizeof(service)); 
		if (iResult == SOCKET_ERROR) 
		{ 
			closesocket(ConnectSocket); 
			std::cerr << "connect failed!\t" << WSAGetLastError() << std::endl; 
			return false; 
		}
		std::cout << "connect sucsed\n";
		return true;
	}

	bool Client::send_str( const std::string& text)
	{
		uint32_t text_length = text.size();
		text_length = htonl(text_length);
		int res = send(ConnectSocket, reinterpret_cast<const char*>(&text_length), sizeof(text_length), 0);
		if (res == SOCKET_ERROR)
			return false;

		res = send(ConnectSocket, text.c_str(), text.size(), 0);
		if (res == SOCKET_ERROR)
			return false;

		return true;
	}

	bool Client::recv_str( std::string& text)
	{
		uint32_t text_length;
		int n = sizeof(text_length);
		do
		{
			int res = recv(ConnectSocket, reinterpret_cast<char*>(&text_length) + (sizeof(text_length) - n), n, 0);
			if (res < 0)
				return false;
			n -= res;

		} while (n);
		text_length = ntohl(text_length);
		std::unique_ptr<char[]> buffer = std::make_unique<char[]>(text_length);
		n = text_length;
		do
		{
			int res = recv(ConnectSocket, buffer.get() + (text_length - n), n, 0);
			if (res < 0)
				return false;
			n -= res;

		} while (n);
		text = std::string(buffer.get(), text_length);
		return true;
	}

}
