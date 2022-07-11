#pragma once
#include <deque>

#include "../Global/HMIDefinitions.h"
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

	void handlePacketQueue();

	void onDataReceived(Packet& p) override;

private:
	std::deque<HMI::HMIPacket> packetQueue;
};

