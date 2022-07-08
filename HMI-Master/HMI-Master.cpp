#include "Server.h"
#include <iostream>
#include <Windows.h>


int main()
{
	Server server;
	server.initialize(1337);
	server.run();
	while (true) {
		Sleep(1000);
	}
	return 1;

}
