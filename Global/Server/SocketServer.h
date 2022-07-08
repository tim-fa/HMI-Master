#pragma once

#include "SocketBase.h"

#include <vector>
#include <thread>
#include <functional>

struct ConnectionInfo {
	ConnectionInfo() = default;

	SOCKADDR_IN addr;
	SOCKET socket;

	std::thread connectionThread;
	bool threadActive;
	void handleConnection(class SocketServer& server);
};

class SocketServer : public SocketBase {
public:
	~SocketServer();

	void initialize(unsigned short port);
	void setHostInformation(unsigned short port);
	void bindHost();

	void startServerThread();
	void stopServerThread();

	virtual void onClientConnected(const ConnectionInfo& connection) = 0;
	virtual void onClientDisconnected(const ConnectionInfo& connection) = 0;
	virtual void onDataReceived(Packet& data, const ConnectionInfo& sender) = 0;

private:
	void startConnectionManagerThread();
	void stopConnectionManagerThread();
	void handleConnections();


	void handleServer();

	bool connectionThreadActive{ false };
	std::thread connectionThread;

	bool serverThreadActive{ false };
	std::thread serverThread;

	std::vector<std::shared_ptr<ConnectionInfo>> connections;

	SOCKADDR_IN addr;
};