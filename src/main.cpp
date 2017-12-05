#include <conn/TCPServer.h>
#include "ui/UI.h"


#include <unistd.h>
#include <iostream>

#define MSG "Test succeeded!"
#define MSG_LEN sizeof MSG

using namespace ui;
using namespace std;

void sHandler(conn::TCPConnection& conn)
{
	char buf[MSG_LEN];
	conn.recv(buf, MSG_LEN);
	cout << buf << endl;
}

void testTCP()
{
	conn::TCPServer server(6669, sHandler);

	server.run();

	auto addr = conn::IPv4Address(INADDR_LOOPBACK, 6669);

	conn::TCPConnection c(addr);

	c.send(MSG, MSG_LEN);
	c.close();

	server.stop();
}

int main()
{
	conn::TCPConnection::enableConnections();
/*
	UI userInterface = UI();

	while(true)
	{
		string userInput;

		// todo: actual prompt
		cout << "P2P program prompt" << endl;

		cin >> userInput;

		userInterface.parseUserInput(userInput);
	}
*/	testTCP();
	conn::TCPConnection::waitForNoConnections();

	return 0;

}