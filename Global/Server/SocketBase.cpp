#include "SocketBase.h"

#include"../StdWrapper/formatString.h"
#include <stdexcept>
#include <tuple>

void SocketBase::initializeWsaData()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) == SOCKET_ERROR)
		throw std::runtime_error("WSASartup failed");
}

void SocketBase::createSocket()
{
	this->localSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (this->localSocket == SOCKET_ERROR)
		throw std::runtime_error("Couldn't create socket");
}

std::tuple<std::shared_ptr<char[]>, int> SocketBase::prepareData(const std::string& arguments, void* data, int size)
{
	PacketHeader command;
	command.argumentSize = static_cast<int>(arguments.length() + 1);
	command.dataSize = size;
	unsigned long finalBufferSize = sizeof(PacketHeader) + command.argumentSize + command.dataSize;

	std::shared_ptr<char[]> dataBuffer(new char[finalBufferSize]);
	memcpy_s(dataBuffer.get(), sizeof(PacketHeader), reinterpret_cast<byte*>(&command), sizeof(PacketHeader));
	memcpy_s(&(dataBuffer.get())[sizeof(PacketHeader)], command.argumentSize, arguments.c_str(), command.argumentSize);
	if (data && size) {
		memmove(&(dataBuffer.get())[sizeof(PacketHeader) + command.argumentSize], data, command.dataSize);

	}
	return std::make_tuple(dataBuffer, finalBufferSize);
}


void SocketBase::shutdown()
{
	WSACleanup();
}

bool SocketBase::sendData(SOCKET socket, const std::string& arguments, void* data, int size)
{
	std::tuple<std::shared_ptr<char[]>, int> d = prepareData(arguments, data, size);
	return sendRawData(socket, &std::get<0>(d)[0], std::get<1>(d));
}


bool SocketBase::sendRawData(SOCKET s, char* dataBuffer, int size)
{
	auto sentDataCount = 0;
	while (sentDataCount < size) {
		sentDataCount += send(s, (const char*)&dataBuffer[sentDataCount], size - sentDataCount, 0);
		if (sentDataCount == SOCKET_ERROR)
			return false;
	}
	return true;
}

std::shared_ptr<char[]> SocketBase::receiveRawData(SOCKET s, int size)
{
	std::shared_ptr<char[]> dataBuffer(new char[size]);
	int receivedDataCount = 0;
	while (receivedDataCount < size)
	{
		int nBytes = recv(s, &(dataBuffer.get())[receivedDataCount], size - receivedDataCount, 0);
		if (nBytes == SOCKET_ERROR) {
			throw std::runtime_error("Receiving data failed");
		}
		receivedDataCount += nBytes;
	}
	return dataBuffer;
}

Packet SocketBase::receiveData(SOCKET s)
{
	Packet p;
	auto header = receiveRawData(s, sizeof(PacketHeader));
	if (header == nullptr) {
		throw std::runtime_error("Packet header is empty");
	}

	p.header = *(PacketHeader*)header.get();
	if (p.header.argumentSize + p.header.dataSize < sizeof(char)) {
		throw std::runtime_error("Packet data is corrupted");
	}

	p.arguments = std::string((char*)receiveRawData(s, p.header.argumentSize).get());

	if ((int)p.arguments.size() < p.header.argumentSize - 1)
		throw std::runtime_error("Packet arguments are corrupted");

	p.data = receiveRawData(s, p.header.dataSize);

	if (!p.data && p.header.dataSize) {
		throw std::runtime_error("Packet doesn't contain raw data but it was expected");
	}
	return p;
}

SOCKET SocketBase::getSocket()
{
	return localSocket;
}