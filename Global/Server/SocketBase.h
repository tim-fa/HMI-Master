#pragma once

#include <WinSock2.h>
#include <memory>
#include <vector>
#include <string>

#include "../StdWrapper/formatString.h"


struct PacketHeader {
	int argumentSize;
	int dataSize;
};

struct Packet {
	PacketHeader header;
	std::string arguments;
	std::shared_ptr<char[]> data;
};

class SocketBase {
public:
	void shutdown();

	bool sendData(SOCKET socket, const std::string& arguments, void* data = nullptr, int size = 0);
	Packet receiveData(SOCKET s);
	template<typename ... Args>
	bool sendString(SOCKET s, const std::string& str, Args ...args)
	{
		std::string finalString = fmt::format(str, args...);
		return sendData(s, finalString);
	}

	SOCKET getSocket();

protected:

	void initializeWsaData();
	void createSocket();


	bool sendRawData(SOCKET s, char* dataBuffer, int size);
	std::shared_ptr<char[]> receiveRawData(SOCKET s, int size);

	WSADATA wsadata;
	SOCKET localSocket;

	short port{ 0 };

private:
	static std::tuple<std::shared_ptr<char[]>, int> prepareData(const std::string& arguments, void* data = nullptr, int size = 0);
};