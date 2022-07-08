#pragma once

#include "SocketBase.h"
#include <functional>
#include <thread>


class SocketClient : public SocketBase
{
public:
	SocketClient(const std::string& ip, unsigned short port);

	void initialize();
	bool connect();
	void disconnect();
	void shutdown();

	const bool& connectionActive();

	virtual void onDataReceived(Packet& p) = 0;

private:
	void setHostInformation(const std::string& ip, unsigned short port);
	hostent* validateHost();
	bool connectToHost(hostent* hostData);

	void startConnectionThread();
	void stopConnectionThread();
	void handleConnection();

	std::string ip;
	short port;
	bool connectionThreadActive;
	std::thread connectionThread;
};