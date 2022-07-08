#include "SocketServer.h"

#include <memory>
#include <fstream>
#include <iostream>
#include <thread>
#include <algorithm>

#include "../StdWrapper/formatString.h"

SocketServer::~SocketServer()
{
	stopConnectionManagerThread();
	stopServerThread();
	shutdown();
}

void SocketServer::initialize(unsigned short port)
{
	initializeWsaData();
	createSocket();
	setHostInformation(port);
	bindHost();
}

void SocketServer::setHostInformation(unsigned short port)
{
	this->port = port;
}

void SocketServer::bindHost()
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(localSocket, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) {
		throw std::runtime_error("Failed to bind to socket");
	}
	listen(localSocket, 5);
}

void SocketServer::handleServer()
{
	SOCKADDR_IN sockAddrIn = { 0 };
	int addrsize = sizeof(sockAddrIn);

	while (serverThreadActive)
	{
		SOCKET clientSocket = accept(localSocket, (struct sockaddr*)&sockAddrIn, &addrsize);
		if (clientSocket == SOCKET_ERROR) {
			continue;
		}
		auto info = std::make_shared<ConnectionInfo>();
		info->addr = sockAddrIn;
		info->socket = clientSocket;
		info->threadActive = true;
		connections.push_back(info);
		connections.back()->connectionThread = std::thread([&]() {
			auto con = connections.back();
			while (con->threadActive) {
				con->handleConnection(*this);
			}
		});
		onClientConnected(*info);
	}
}

void SocketServer::startConnectionManagerThread()
{
	if (connectionThreadActive)
		return;
	connectionThreadActive = true;
	connectionThread = std::thread([&]() {
		while (connectionThreadActive) {
			handleConnections();
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	});
}

void SocketServer::stopConnectionManagerThread()
{
	if (!connectionThreadActive)
		return;
	connectionThreadActive = false;
	connectionThread.join();
}

void SocketServer::handleConnections()
{
	for (int i = 0; i < (int)connections.size(); i++)
	{
		if (!connections.at(i)->threadActive) {
			closesocket(connections.at(i)->socket);
			if (connections.at(i)->connectionThread.joinable())
				connections.at(i)->connectionThread.join();
			connections.erase(connections.begin() + i);
		}
	}
}

void SocketServer::startServerThread()
{
	startConnectionManagerThread();
	if (serverThreadActive)
		return;
	serverThreadActive = true;
	serverThread = std::thread([&]() {
		while (serverThreadActive) {
			handleServer();
		}
	});
}

void SocketServer::stopServerThread()
{
	stopConnectionManagerThread();
	if (!serverThreadActive)
		return;
	serverThreadActive = false;
	serverThread.join();
}

void ConnectionInfo::handleConnection(SocketServer& server)
{
	try {
		Packet p = server.receiveData(socket);
		server.onDataReceived(p, *this);
	}
	catch (...) {
		threadActive = false;
		server.onClientDisconnected(*this);
	}
}

