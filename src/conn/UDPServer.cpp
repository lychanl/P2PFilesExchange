
#include "UDPServer.h"

#include <unistd.h>
#include <csignal>
#include <sys/signalfd.h>
#include <log/Logger.h>

using namespace conn;

UDPServer::UDPServer(int port, int maxDGramSize, handler handler)
		: Server(new GlobalUDPServer(port, maxDGramSize, handler))
{
	this->server = new GlobalUDPServer(port, maxDGramSize, handler);
}

UDPServer::GlobalUDPServer::GlobalUDPServer(int port, int maxDGramSize, handler handler)
{
	this->address = IPv4Address::getAnyAddress(port);
	this->bufSize = maxDGramSize;
	this->handl = handler;

	this->buffer = new char[this->bufSize];
}

int UDPServer::GlobalUDPServer::initSocket()
{
	int ret = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (ret < 0)
	{
		Logger::getInstance().logError("UDPServer: Error while creating socket. Errno: " + std::to_string(errno));
		return -1;
	}

	if (::bind(ret, reinterpret_cast<sockaddr*>(&this->address), sizeof (this->address)) < 0)
	{
		Logger::getInstance().logError("UDPServer: Error while binding socket. Errno: " + std::to_string(errno));
		::close(ret);
		return -1;
	}

	int bc = 1;

	::setsockopt(ret, SOL_SOCKET, SO_BROADCAST, &bc, sizeof(bc));

	return ret;
}

int UDPServer::GlobalUDPServer::_run(int socketToRead)
{
	sockaddr_in client;
	socklen_t clSize = sizeof(sockaddr_in);

	int read = recvfrom(socketToRead,
						this->buffer,
						this->bufSize,
						MSG_DONTWAIT,
						reinterpret_cast<sockaddr*>(&client),
						&clSize);

	if (read < 0)
	{
		Logger::getInstance().logError("UDPServer: Error while receiving. Errno: " + std::to_string(errno));
	}

	if (read > 0)
	{
		IPv4Address clAddr(client);

		this->handl(this->buffer, read, clAddr);
	}

	return 0;
}

UDPServer::~UDPServer()
{
	delete this->server;
}

UDPServer::GlobalUDPServer::~GlobalUDPServer()
{
	delete[] buffer;
}