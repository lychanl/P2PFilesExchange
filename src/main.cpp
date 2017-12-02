#include "ui/UI.h"

using namespace ui;
using namespace std;

int main()
{
	UI userInterface = UI();

	while(true)
	{
		string userInput;

		// todo: actual prompt
		cout << "P2P program prompt" << endl;

		cin >> userInput;

		userInterface.parseUserInput(userInput);
	}
}