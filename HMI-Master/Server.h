#pragma once
#include "../Global/Server/SocketServer.h"

#include <map>

class Server : public SocketServer
{
public:
	void initialize(unsigned short port);
	void run();

private:

	void onClientConnected(const ConnectionInfo& connection) override;
	void onClientDisconnected(const ConnectionInfo& connection) override;
	void onDataReceived(Packet& data, const ConnectionInfo& sender) override;

	std::vector<int> connectedSockets;
};
