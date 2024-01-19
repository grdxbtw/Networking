#pragma once
#include <WinSock2.h>

#include <algorithm>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

namespace NetServer {
#include "net_base.h"
}

namespace NetServer
{
	class Server
	{
	private:
		const char* address;
		uint16_t port;

		//
		static WSADataHelper wsa;
		SOCKET ListenSocket; 
		std::vector<SOCKET> clients; 
		std::mutex clients_mtx; 
		std::atomic_bool stop_working; 
		std::thread main_worker;

	public:
		explicit Server(const char* ipaddress, uint16_t port);
		~Server();

		bool start();
		void shutdown();
		void clear_connections(); 

	private:
		bool init();

		bool accept_client(SOCKET listenSocket);

		bool send_str(SOCKET s, const std::string& text);
		bool recv_str(SOCKET s,  std::string& text);
		bool do_work(SOCKET sck);

		void remove_client(SOCKET sck);
		void add_client(SOCKET sck);
		void worker(SOCKET listensocket); 


	private:
		Server(Server&&) = delete; 
		Server& operator=(Server&&) = delete; 

	};
}

