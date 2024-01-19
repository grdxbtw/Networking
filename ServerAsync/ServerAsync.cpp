// ServerAsync.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include "server_asio.h"

int main()
{
    boost::asio::io_context io_context;
    Net_asio::ServerAsio srv(io_context, 27015); 
    io_context.run();
    return 0;
}

