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

		IPv4Address(unsigned char address[4], unsigned short int port);
		IPv4Address(unsigned char address[4]);

		IPv4Address(unsigned long int address, unsigned short int port);
		IPv4Address(unsigned long int address);

		const struct sockaddr_in& getSockaddr() const;

		unsigned long int getAddress() const;
		unsigned short int getPort() const;

		static sockaddr_in getAnyAddress(int port);
		static sockaddr_in getBroadcastAddress(int port);
	private:
		struct sockaddr_in sockAddr;
		unsigned long int address;
		unsigned short int port;
	};
}

#endif //P2PFILESEXCHANGE_IPV4ADDRESS_H