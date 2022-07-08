#include "SocketClient.h"


#include "../StdWrapper/formatString.h"
#include <stdexcept>


SocketClient::SocketClient(const std::string& ip, unsigned short port)
	: connectionThreadActive(false)
	, port(port)
	, ip(ip)
{
}


void SocketClient::initialize()
{
	initializeWsaData();
}

bool SocketClient::connect()
{
	createSocket();
	auto hostData = validateHost();
	if (hostData)
	{
		if (connectToHost(hostData)) {
			startConnectionThread();
			return true;
		}
	}
	return false;
}

void SocketClient::disconnect()
{
	stopConnectionThread();
	closesocket(localSocket);
}

const bool& SocketClient::connectionActive()
{
	return connectionThreadActive;
}

void SocketClient::setHostInformation(const std::string& ip, unsigned short port)
{
	this->ip = ip;
	this->port = port;
}

hostent* SocketClient::validateHost()
{
	struct hostent* hostEnt = gethostbyname(ip.c_str());
	return hostEnt;
}

bool SocketClient::connectToHost(hostent* hostData)
{
	if (!hostData)
		throw std::runtime_error("Invalid Host Data");
	struct sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = *(unsigned long*)hostData->h_addr;

	return ::connect(localSocket, (sockaddr*)&sockAddr, sizeof(sockAddr)) != SOCKET_ERROR;
}

void SocketClient::startConnectionThread()
{
	if (connectionThreadActive)
		return;
	if (connectionThread.joinable())
		connectionThread.join();
	connectionThreadActive = true;
	connectionThread = std::thread([&]() {
		try {
			while (connectionThreadActive)
			{
				handleConnection();
			}
		}
		catch (std::exception& e) {
			// TODO: uglyyyyy
			printf("Fatal error: %s. Disconnecting"), e.what();
			closesocket(localSocket);
			connectionThreadActive = false;
		}
	});
}

void SocketClient::stopConnectionThread()
{
	connectionThreadActive = false;
	if (connectionThread.joinable())
		connectionThread.join();
}

void SocketClient::handleConnection()
{
	Packet p = receiveData(localSocket);
	onDataReceived(p);
}

void SocketClient::shutdown()
{
	disconnect();
	SocketBase::shutdown();
}
