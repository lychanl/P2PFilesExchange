#include <conn/TCPServer.h>
#include "ui/UI.h"
#include "files/FileManager.h"

using namespace ui;
using namespace std;

int main()
{
	conn::TCPConnection::enableConnections();
    conn::IPv4Address local = conn::IPv4Address::getLocalAddress(0);
    conn::UDPBroadcaster udpBroadcaster(conn::IPv4Address::getBroadcastAddress(local, 24, conn::IPv4Address::APPLICATION_PORT));

	conn::TCPServer tcpServer(conn::IPv4Address::APPLICATION_PORT, proto::Protocols::tcpHandler);
	conn::UDPServer udpServer(conn::IPv4Address::APPLICATION_PORT, 2 << 16, proto::Protocols::udpHandler);

    files::FileManager fileManager("files");

    proto::Protocols::init(&udpBroadcaster, &fileManager);

    UI userInterface = UI(&fileManager, &tcpServer, &udpServer);

	udpBroadcaster.open();

	userInterface.start();

	tcpServer.stop();
	udpServer.stop();

    conn::TCPConnection::waitForNoConnections();

    return 0;

}