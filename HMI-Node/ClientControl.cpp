#include "ClientControl.h"

#include "../Global/Log.h"
#include "../Global/HMIDefinitions.h"



void ClientControl::onDataReceived(Packet& p)
{
	auto hmiPacket = reinterpret_cast<HMI::HMIPacket*>(&p.data[0]);
	if (HMI::verifyPacket(*hmiPacket)) {
		if (hmiPacket->commandId != HMI::Acknowledge) {
			Log::info("Sending ACK to 0x%X", hmiPacket->sourceId);
			auto ackPacket = HMI::generatePacket(HMI::Acknowledge, HMI::nodeId, hmiPacket->sourceId, "");
			sendData(getSocket(), "", &ackPacket, HMI::getHmiPacketSize(ackPacket));
		}
		Log::info(R"(Packet Contents:  
	+----------------------------+   
	  checksum: 0x%X
	+----------------------------| 
	  commandId: 0x%X 
	+----------------------------+
	  from: 0x%X
	+----------------------------+
	  to: 0x%X
	+----------------------------+
	  amount of data bytes: %d
	+----------------------------+
)", hmiPacket->checksum, hmiPacket->commandId, hmiPacket->sourceId, hmiPacket->targetId, hmiPacket->dataLength);


		HMI::HMIPacket sendPacket;
		std::string inputBuffer;
		switch (hmiPacket->commandId) {
		case HMI::Acknowledge:
			Log::info("ACK recieved from 0x%X", hmiPacket->sourceId);
			break;
		case HMI::RequestInput:
			Log::info("Application-SG %X expects a user input. Please enter a string and confirm with [ENTER]", hmiPacket->sourceId);
			std::cin >> inputBuffer;
			sendPacket = HMI::generatePacket(HMI::HMIDataPacket, HMI::nodeId, hmiPacket->sourceId, inputBuffer);
			sendData(getSocket(), "", &sendPacket, HMI::getHmiPacketSize(sendPacket));
			break;
		case HMI::RequestResend:
			break;
		default:
			Log::error("Unsupported command %d", hmiPacket->commandId);
		}
	}
}
