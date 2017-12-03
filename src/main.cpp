#include <conn/TCPServer.h>
#include "conn/IPv4Address.h"

#include <unistd.h>
#include <iostream>

#define MSG "Test succeeded!"
#define MSG_LEN sizeof MSG

void sHandler(conn::TCPConnection& conn)
{
	char buf[MSG_LEN];
	conn.recv(buf, MSG_LEN);
	std::cout << buf << std::endl;
}

int main()
{
	conn::TCPConnection::enableConnections();
	conn::TCPServer server(6669, sHandler);

	server.run();

	{
		auto addr = conn::IPv4Address(INADDR_LOOPBACK, 6669);

		conn::TCPConnection c(addr);

		c.send(MSG, MSG_LEN);
		c.close();
	}
	server.stop();

	conn::TCPConnection::waitForNoConnections();

	return 0;
}