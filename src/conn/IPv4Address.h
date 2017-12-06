#ifndef P2PFILESEXCHANGE_IPV4ADDRESS_H
#define P2PFILESEXCHANGE_IPV4ADDRESS_H

#include <string>
#include <netinet/in.h>

namespace conn
{
	class IPv4Address
	{
	public:
		IPv4Address(const std::string& address, unsigned short int port);
		IPv4Address(const std::string& address);

		IPv4Address(unsigned long int address, unsigned short int port);
		IPv4Address(unsigned long int address);

		IPv4Address(const struct sockaddr_in& addr);

		const struct sockaddr_in& getSockaddr() const;

		unsigned long int getAddress() const;
		unsigned short int getPort() const;

		static sockaddr_in getAnyAddress(int port);
		static sockaddr_in getBroadcastAddress(const IPv4Address& address, int mask, int port);
	private:
		struct sockaddr_in sockAddr;
		unsigned long int address;
		unsigned short int port;
	};
}

#endif //P2PFILESEXCHANGE_IPV4ADDRESS_H