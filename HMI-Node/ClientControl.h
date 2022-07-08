#pragma once


#include "../Global/Server/SocketClient.h"
#include <string>

class ClientControl : public SocketClient
{
public:
	ClientControl(const std::string& ip, unsigned short port)
		: SocketClient(ip, port)
	{
		SocketClient::initialize();
	}

	void onDataReceived(Packet& p) override;
};

