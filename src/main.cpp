#include <conn/TCPConnection.h>
#include "ui/UI.h"


#include <unistd.h>
#include <iostream>

using namespace ui;
using namespace std;

int main()
{
	conn::TCPConnection::enableConnections();


	UI userInterface = UI();

	while(true)
	{
		string userInput;

		// todo: actual prompt
		cout << "P2P program prompt" << endl;

		cin >> userInput;

		userInterface.parseUserInput(userInput);
	}

	conn::TCPConnection::waitForNoConnections();

	return 0;

}