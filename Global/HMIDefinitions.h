#pragma once
#include <iostream>

namespace HMI {

	constexpr uint8_t MaxDataSendLength = 0xFF;

	extern uint16_t createSendCookie();

	enum CommandId : uint8_t {
		Acknowledge,				// Confirms successfull receival of package
		RequestInput,				// Requires the user to enter a text in the HMI
		RequestConfirmation,		// Only requires the user to press OK in the HMI
		RequestResend,				// Target will resend the last packet
		HMIDataPacket				// Contains a text string from the HMI

	};

	struct HMIPacket {
		uint16_t checksum;
		uint8_t commandId;
		uint8_t dataLength;
		uint16_t sourceId;
		uint16_t targetId;
		char data[MaxDataSendLength];
	};

	extern int generatePacketChecksum(const HMIPacket& packet);
	extern HMIPacket generatePacket(CommandId commandId, uint16_t sourceId, uint16_t targetId, const std::string& data);
	extern int getHmiPacketSize(const HMIPacket& packet);
	extern bool verifyPacket(const HMIPacket& packet);

	extern uint16_t nodeId;
}