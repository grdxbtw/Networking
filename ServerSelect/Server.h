#pragma once
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <WinSock2.h>

#include "net_base.h"


namespace Net
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

	public:
		explicit Server(const char* ipaddress, uint16_t port);
		~Server();

		void start();
		void shutdown();

	private:
		bool init();

		bool send_str(SOCKET s, const std::string& text);
		bool recv_str(SOCKET s,  std::string& text);
		bool do_work(SOCKET sck);

		void remove_client(SOCKET sck);
		void add_client(SOCKET sck);
		void worker();
		void clear_connections();

	private:
		Server(Server&&) = delete; 
		Server& operator=(Server&&) = delete; 

	};
}

