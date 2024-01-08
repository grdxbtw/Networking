
#ifndef NET_BASE_H
#define NET_BASE_H

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <string>
#include <memory>

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable: 4996)


class WSADataHelper
{
public:
	WSADataHelper()
	{
		result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	}
	~WSADataHelper()
	{
		if (IsOk())
		{
			WSACleanup();
		}
	}
	bool IsOk()
	{
		return result == NO_ERROR;
	}
private:
	WSAData wsaData;
	int result;
};



#endif // !NET_BASE_H

