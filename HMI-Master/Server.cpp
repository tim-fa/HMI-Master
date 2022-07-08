#include "Server.h"
#include <stdexcept>

#include "../Global/Log.h"
#include "../Global/HMIDefinitions.h"

void Server::initialize(unsigned short port)
{
	Log::info("Initializing master with port %d", port);
	SocketServer::initialize(port);
}

void Server::run()
{
	startServerThread();
}
 
void Server::onClientConnected(const ConnectionInfo& connection)
{
	Log::info("%d connected", connection.socket);
	connectedSockets.push_back(static_cast<int>(connection.socket));
}

void Server::onClientDisconnected(const ConnectionInfo& connection)
{
	Log::info("%d disconnected",connection.socket);
	for (int i = 0; i < connectedSockets.size(); i++)
	{
		if (connectedSockets[i] == connection.socket) {
			connectedSockets[i] = connectedSockets.back();
			connectedSockets.pop_back();
		}
	
	}
}

void Server::onDataReceived(Packet& data, const ConnectionInfo& sender)
{
	HMI::HMIPacket* hmiPacket = reinterpret_cast<HMI::HMIPacket*>(&data.data[0]);
	for (auto socket : connectedSockets) {
		Log::info("BUS Received Packet from 0x%X to 0x%X. Broadcasting to BUS", hmiPacket->header.sourceId, hmiPacket->header.targetId);
		sendData(socket,"", hmiPacket, HMI::getHmiPacketSize(*hmiPacket));
	}
}
