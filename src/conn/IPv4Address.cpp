#include "IPv4Address.h"
#include "exception.h"

#include <arpa/inet.h>

using namespace conn;
using namespace std;

IPv4Address::IPv4Address(unsigned long int address) : IPv4Address(address, 0)
{
}

IPv4Address::IPv4Address(const std::string& address) : IPv4Address(address, 0)
{
}

IPv4Address::IPv4Address(const struct sockaddr_in &addr)
{
	this->sockAddr = addr;
	this->address = ntohl(addr.sin_addr.s_addr);
	this->port = ntohs(addr.sin_port);
}
IPv4Address::IPv4Address(const std::string& address, unsigned short int port)
{
	if (inet_pton(AF_INET, address.c_str(), &this->sockAddr.sin_addr) == -1)
		throw Exception("Invalid IP address: " + address);

	this->address = ntohl(this->sockAddr.sin_addr.s_addr);
	this->port = port;

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
}

IPv4Address::IPv4Address(unsigned long int address, unsigned short int port)
{
	this->address = address;
	this->port = port;

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(address);
	sockAddr.sin_port = htons(port);
}

unsigned long int IPv4Address::getAddress() const
{
	return this->address;
}

unsigned short int IPv4Address::getPort() const
{
	return this->port;
}

const struct sockaddr_in& IPv4Address::getSockaddr() const
{
	return this->sockAddr;
}

sockaddr_in IPv4Address::getAnyAddress(int port)
{
	sockaddr_in ret{};

	ret.sin_family = AF_INET;
	ret.sin_addr.s_addr = htonl(INADDR_ANY);
	ret.sin_port = htons(port);

	return ret;
}

sockaddr_in IPv4Address::getBroadcastAddress(int port)
{
	sockaddr_in ret{};

	ret.sin_family = AF_INET;
	ret.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	ret.sin_port = htons(port);

	return ret;
}