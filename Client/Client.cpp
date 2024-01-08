#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma warning(disable: 4996)
//
#include "../Server/WSAHelper.h"
#include "Functions.h"
#include "Client_class.h"

const char* addr = "127.0.0.1";

int main(int argc, char* argv[])
{
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
    Net::Client client(addres, port);
    if (!client.connect_to_server())
    {
        std::cerr << "cannot connect to server\n";
        return 1;
    }
    do
    {
        std::string expr;
        std::getline(std::cin, expr);
        if (expr == "q")
            break;

        if (!client.send_str( expr))
        {
            return 1;
        }
        std::string res;
        if (!client.recv_str(res))
        {
            return 1;
        }
        std::cout << "result:\t" << res << std::endl;

    } while (true);

#endif // 1

    return 0;
}