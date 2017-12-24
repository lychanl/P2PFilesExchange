#include <exception.h>
#include "UI.h"

using namespace ui;

bool ui::interruptSignalFlag = false;

int UI::parseUserInput(const std::string &inputString)
{
    if (interruptSignalFlag)
    {
        return parser.disconnect();
    }

    if (inputString.substr(0, 8) == "-connect")
    {
        char * substrEnd;
        long int mask;

        if ((inputString.length() < 15))
        {
            cout << "Invalid command" << endl;
            return -1;
        }

        if ((mask = strtol(inputString.substr(9, 2).c_str(), &substrEnd, 10)) == 0)
        {
            cout << "Invalid mask " + inputString.substr(9, 2) << endl;
            return -2;
        }

        return parser.connect(mask, inputString.substr(13, inputString.length() - 13));
    }
    else if (inputString.substr(0, 11) == "-disconnect")
    {
       return parser.disconnect();
    }
    else if(inputString.substr(0, 7) == "-upload")
    {
        string filename;

        if (inputString.length() < 9)
        {
            cout << "Invalid command" << endl;
            return -3;
        }

        filename = inputString.substr(8, inputString.length() - 8);

        return parser.uploadFile(filename);
    }
    else if(inputString.substr(0, 7) == "-delete")
    {
        string filename;

        if (inputString.length() < 9)
        {
            cout << "Invalid command" << endl;
            return -4;
        }

        filename = inputString.substr(8, inputString.length() - 8);

        return parser.deleteFile(filename);
    }
    else if(inputString.substr(0, 9) == "-download")
    {
        string filename;

        if (inputString.length() < 11)
        {
            cout << "Invalid command" << endl;
            return -5;
        }

        filename = inputString.substr(10, inputString.length() - 10);

        return parser.downloadFile(filename);
    }
    else if(inputString == "-la")
    {
        parser.listAll();
    }
    else if(inputString == "-l")
    {
        parser.listLocal();
    }
    else
    {
        cout << "Invalid command" << endl;
        cout << "Available commands:" << endl;
        cout << "-connect <mask> <address>" << endl;
        cout << "-disconnect" << endl;
        cout << "-upload <filename>" << endl;
        cout << "-delete <filename>" << endl;
        cout << "-download <filename>" << endl;
        cout << "-la" << endl;
        cout << "-l" << endl;
    }
    return 0;
}

UI::UI()
{
    parser = Parser();
    initSignals();
}

int UI::start()
{
    string userInput;

    cout << "TIN P2P project" << endl;
    cout << "Created by: " << endl;
    cout << "Jakub Łyskawa" << endl;
    cout << "Agata Kłoss" << endl;
    cout << "Robert Piwowarek" << endl;

    do
    {
        cout << "Enter command:" << endl;

        getline(cin, userInput);

    } while(parseUserInput(userInput) != DISCONNECT_RETURN_VALUE);
}

int UI::Parser::connect(long int mask, const std::string &address)
{
    return 0;
}

int UI::Parser::disconnect()
{
    std::cout << "disconnect" << std::endl;
    return DISCONNECT_RETURN_VALUE;
}

int UI::Parser::uploadFile(string file)
{
    return 0;
}

int UI::Parser::deleteFile(string file)
{
    return 0;
}

int UI::Parser::downloadFile(string file)
{
    return 0;
}

void UI::Parser::listAll()
{

}

void UI::Parser::listLocal()
{

}

UI::Parser::Parser() = default;

void signalHandler(int sig)
{
    if (sig == SIGINT)
    {
        std::cout << "SIGINT" << std::endl;
        ui::interruptSignalFlag = true;
    }
}

void UI::initSignals()
{
    sigact.sa_handler = signalHandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(SIGINT, &sigact, (struct sigaction *) nullptr);
}