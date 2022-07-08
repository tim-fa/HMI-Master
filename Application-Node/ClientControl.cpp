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

		switch (hmiPacket->commandId) {
		case HMI::Acknowledge:
			Log::info("ACK recieved from 0x%X", hmiPacket->sourceId);
			break;
		case HMI::HMIDataPacket:
			Log::info("Received data string: %s", hmiPacket->data);
			break;
		case HMI::RequestConfirmation:
			break;
		case HMI::RequestResend:
			break;
		default:
			Log::error("Unknown command %d", hmiPacket->commandId);
		}
	}
}
