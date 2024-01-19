#include "Server.h"
#include "../../../cpp_projects/newCalculator/newCalculator/Kalkulator.h" 

#ifdef _DEBUG 
#include<iostream>
#define PRINT(MSG) std::cout << MSG << std::endl
#else
#define PRINT(MSG) 
#endif // _DEBUG

namespace NetServer
{
	WSADataHelper Server::wsa; 
	Server::Server(const char* ipaddress, uint16_t port)
		:
		address(ipaddress),
		port(port),
		ListenSocket(INVALID_SOCKET) {}

	Server::~Server()
	{		
		shutdown();
	}
	bool Server::start()
	{
		return init();
	}
	void Server::shutdown()
	{	
		if(!stop_working)
		{
			stop_working = true;
			if (main_worker.joinable())
				main_worker.join();

			clear_connections();
			PRINT("server stopped");
		}
	}
	bool Server::init()
	{
		if(!wsa.IsOk())
		{
			PRINT("Couldn't init wsa!");
			return false;
		}

		struct sockaddr_in service;
		service.sin_family = AF_INET;  
		service.sin_addr.s_addr = inet_addr(address); 
		service.sin_port = htons(port);  
		int iResult; 

		ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  
		if (ListenSocket == INVALID_SOCKET) 
		{
			PRINT("socket failed with error; " << WSAGetLastError()); 
			return false;
		}
		PRINT("socket created!");

		iResult = bind(ListenSocket, (SOCKADDR*)&service, sizeof(service));
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ListenSocket);
			PRINT("bind failed with error; " << WSAGetLastError());
			return false;
		}
		PRINT("bind Success!");

		iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ListenSocket);
			PRINT("listen failed with error; " << WSAGetLastError());
			return false;
		}
		PRINT("listen Success!");

		//start processor thread
		stop_working = false;
		main_worker = std::thread(&Server::worker,this, ListenSocket);
		return true;
	}

	bool Server::accept_client(SOCKET listenSocket)
	{	
		struct sockaddr_in results;
		int sizeaddr = sizeof(results);
		SOCKET ClientSocket = accept(listenSocket, (SOCKADDR*)&results, &sizeaddr);
		if (ClientSocket != INVALID_SOCKET)
		{
			PRINT(inet_ntoa(results.sin_addr) << '\t' << ntohs(results.sin_port));
			add_client(ClientSocket);
			return true;
		}
		else
		{
			PRINT("accept failed with error; " << WSAGetLastError());
			return false;
		}
	}

	void Server::clear_connections()
	{
		std::lock_guard<std::mutex> lock(clients_mtx);
		for (auto itr = clients.cbegin(); itr != clients.cend(); ++itr)
		{
			closesocket(*itr);
		}
	}

	bool Server::send_str(SOCKET s, const std::string& text)
	{
		uint32_t text_length = text.size();
		text_length = htonl(text_length);
		int res = send(s, reinterpret_cast<const char*>(&text_length), sizeof(text_length), 0);
		if (res == SOCKET_ERROR)
			return false;

		res = send(s, text.c_str(), text.size(), 0);
		if (res == SOCKET_ERROR)
			return false;

		return true;
	}
	bool Server::recv_str(SOCKET s,  std::string& text)
	{
		uint32_t text_length;
		int n = sizeof(text_length);
		do
		{
			int res = recv(s, reinterpret_cast<char*>(&text_length) + (sizeof(text_length) - n), n, 0);
			if (res <= 0)
				return false;
			n -= res;

		} while (n);
		text_length = ntohl(text_length);
		std::unique_ptr<char[]> buffer = std::make_unique<char[]>(text_length);
		n = text_length;
		do
		{
			int res = recv(s, buffer.get() + (text_length - n), n, 0);
			if (res < 0)
				return false;
			n -= res;

		} while (n);
		text = std::string(buffer.get(), text_length);
		return true;
	}
	bool Server::do_work(SOCKET sck)
	{
		std::string expr;
		Kalkulator<double> k;
		if (!recv_str(sck, expr))
			return false;
		try
		{
			k.Parsing(expr);
		}
		catch (const std::exception&)
		{
			expr = "exception"; 
		}

		return send_str(sck, expr);
	}
	void Server::remove_client(SOCKET sck)
	{
		std::lock_guard<std::mutex> lk(clients_mtx); 
		clients.erase(std::find(clients.begin(), clients.end(), sck)); 
	}
	void Server::add_client(SOCKET sck)
	{
		std::lock_guard<std::mutex> lk(clients_mtx); 
		clients.push_back(sck); 
	}

	void Server::worker(SOCKET listensocket) 
	{
		TIMEVAL timeout = {};
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		int nfds = 0;
		fd_set reading;
		
		do {
			FD_ZERO(&reading);
			{
				nfds = listensocket; 
				FD_SET(listensocket, &reading);

				std::lock_guard<std::mutex> lk(clients_mtx);
				for (auto itr = clients.cbegin(); itr != clients.cend(); ++itr)
				{
					FD_SET(*itr, &reading);
					//max_element +1
					if (nfds < *itr)
						nfds = *itr;
				}
			}
			++nfds; //max socket_id+1 for non-windows system

			//The nfds parameter is included only for compatibility with Berkeley sockets. Ignored.
			int res = select(nfds, &reading, nullptr, nullptr, &timeout);
			if (res < 0)
			{
				PRINT("Error in select");
			}
			else if (res > 0)
			{
				std::vector<SOCKET> client;
				{
					std::lock_guard<std::mutex> lk(clients_mtx);

					for (auto itr = clients.cbegin(); itr != clients.cend(); ++itr)
					{
						if (FD_ISSET(*itr, &reading))
						{
							client.push_back(*itr);
						}
						// mb break if there is no clients ready
					}
				}
				
				for (size_t i = 0; i < client.size(); i++)
				{
					if (!do_work(client[i]))
					{
						//removed closed socket
						remove_client(client[i]);
					}
				}

				if (FD_ISSET(listensocket, &reading))
				{
					accept_client(listensocket);
				}
			
			}
			//else 0 - the time limit expired 

		} while (!stop_working);
		closesocket(listensocket);
	}
}
