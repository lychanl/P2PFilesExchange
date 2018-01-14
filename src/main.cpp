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

	udpBroadcaster.open();

    files::FileManager fileManager(local, "");

    proto::Protocols::init(&udpBroadcaster, &fileManager);

    UI userInterface = UI();

    userInterface.start();

    conn::TCPConnection::waitForNoConnections();

    return 0;

}