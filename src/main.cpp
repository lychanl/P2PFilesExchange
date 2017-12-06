#include <conn/TCPServer.h>
#include "ui/UI.h"

using namespace ui;
using namespace std;

int main()
{
	conn::TCPConnection::enableConnections();

    UI userInterface = UI();

    userInterface.start();

    conn::TCPConnection::waitForNoConnections();

    return 0;

}