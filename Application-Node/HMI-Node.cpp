#include <iostream>
#include <thread>

#include "ClientControl.h"

#include "../Global/HMIDefinitions.h"
#include "../Global/Log.h"

#undef max

int main()
{
	ClientControl client("127.0.0.1", 1337);

	HMI::nodeId = HMI::createSendCookie();

	std::string inputBuffer;
	while (true) {
		if (!client.connectionActive())
		{
			Log::info("Connecting to master...");
			client.connect();
			continue;
		}

		std::cin >> inputBuffer;
		if (inputBuffer == "requestText") {
			HMI::HMIPacket sendPacket;
			sendPacket.commandId = HMI::CommandId::RequestInput;
			sendPacket.dataLength = 0;
			sendPacket.sourceId = HMI::nodeId;
			sendPacket.targetId = 0x0000;
			sendPacket.checksum = HMI::generatePacketChecksum(sendPacket);

			client.sendData(client.getSocket(), "", &sendPacket, HMI::getHmiPacketSize(sendPacket));

		}

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}