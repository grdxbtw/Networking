// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <WinSock2.h>
#include <string>
#include <thread>
#include <iostream>
#include <vector>
#include <algorithm>


#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable: 4996)

//
#include "WSAHelper.h"
#include "Worker.h"


int main()
{
	
	WSADataHelper wsa;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
    int iResult; 
    sockaddr_in service; 
    service.sin_family = AF_INET; 
    service.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    service.sin_port = htons(27015); 
    struct sockaddr_in results;

    std::vector<std::thread> threads;

    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (ListenSocket == INVALID_SOCKET)
    {
        std::cerr << "socket failed with error; " << WSAGetLastError() << std::endl;
        return 1;
    }
    std::cout << "socket created!\n"; 

    iResult = bind(ListenSocket, (SOCKADDR*)&service, sizeof(service)); 
    if (iResult == SOCKET_ERROR)
    {
        closesocket(ListenSocket);
        std::cerr << "bind failed with error; " << WSAGetLastError() << std::endl;
        return 1;
    }
    std::cout << "bind Success!\n";

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        closesocket(ListenSocket); 
        std::cerr << "listen failed with error; " << WSAGetLastError() << std::endl;
        return 1; 
    }
    std::cout << "listen Success!\n";

    do
    {
        int sizeaddr = sizeof(results); 
        ClientSocket = accept(ListenSocket, (SOCKADDR*)&results, &sizeaddr); 
        if (ClientSocket != INVALID_SOCKET) 
        {
            std::cout << results.sin_addr.S_un.S_addr << '\t' << results.sin_port << '\n'; 
            threads.emplace_back(worker, ClientSocket);
        } 
        else
        {
            std::cerr << "accept failed with error; " << WSAGetLastError() << std::endl; 
            break;
        }
    } while (true);
     
    std::for_each(threads.begin(), threads.end(), [](std::thread& el) { el.join(); });  
    closesocket(ListenSocket); 

	return 0;
}

