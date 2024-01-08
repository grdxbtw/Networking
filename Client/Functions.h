#pragma once
#include <memory>
#include <WinSock2.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib") 

bool send_str(SOCKET s, const std::string& text)
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

bool recv_str(SOCKET s, std::string& text)
{
    uint32_t text_length;
    int n = sizeof(text_length);
    do
    {
        int res = recv(s, reinterpret_cast<char*>(&text_length) + (sizeof(text_length) - n), n, 0);
        if (res < 0)
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