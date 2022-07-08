#include <iostream>
#include <thread>

#include "ClientControl.h"

#include "../Global/HMIDefinitions.h"
#include "../Global/Log.h"

#undef max

int main()
{
	ClientControl client("127.0.0.1", 1337);

	HMI::nodeId = 0; // HMI-Control Unit always has device ID 0

	while (true) {
		if (!client.connectionActive())
		{
			Log::info("Connecting to master...");
			client.connect();
			continue;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}