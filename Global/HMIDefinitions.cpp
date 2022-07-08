#include "HMIDefinitions.h"
#include <Windows.h>
#include <random>
#include "Log.h"

#undef max
#undef min

namespace HMI {
	uint16_t nodeId;

	uint16_t createSendCookie()
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 0xFFFF);
		return dist6(rng);
	}

	int generatePacketChecksum(const HMIPacket& packet) {

		byte* packetBytes = (byte*)&packet;
		int checksum = 0;
		for (int byteIndex = sizeof(packet.checksum); byteIndex < getHmiPacketSize(packet) - sizeof(packet.checksum); byteIndex++) {
			checksum ^= packetBytes[byteIndex];
		}
		return checksum;
	}

	HMIPacket generatePacket(CommandId commandId, uint16_t sourceId, uint16_t targetId, const std::string& data) {
		HMIPacket hmiPacket;

		hmiPacket.commandId = commandId;
		hmiPacket.dataLength = data.length();
		hmiPacket.sourceId = sourceId;
		hmiPacket.targetId = targetId;
		memcpy(hmiPacket.data, data.c_str(), std::min((uint8_t)(data.length() + 1), HMI::MaxDataSendLength));
		hmiPacket.checksum = generatePacketChecksum(hmiPacket);
		return hmiPacket;

	}

	int getHmiPacketSize(const HMIPacket& packet) {
		return sizeof(HMIPacket) - HMI::MaxDataSendLength * sizeof(char) + sizeof(uint8_t) * packet.dataLength;
	}
	bool verifyPacket(const HMIPacket& packet)
	{
		auto checksum = HMI::generatePacketChecksum(packet);
		if (checksum == packet.checksum && packet.targetId == HMI::nodeId) {
			Log::info("Packet recieved from 0x%X addressed to 0x%X (this node).", packet.sourceId, packet.targetId);
			return true;
		}
		else if (checksum != packet.checksum) {
			Log::error("Packet seems to be corrupted. Not sending ACK packet back");
			return false;
		}
		else {
			Log::warning("Ignoring packet since it is addressed to 0x%X", packet.targetId);
			return false;
		}
	}
}