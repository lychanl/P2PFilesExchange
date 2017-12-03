#include <exception.h>
#include "UI.h"

using namespace ui;

void UI::parseUserInput(const std::string &inputString)
{
    if (inputString.substr(0, 7) == "-connect")
    {
        char * substrEnd;
        long int mask;

        if ((mask = strtol(inputString.substr(9, 2).c_str(), &substrEnd, 10)) == 0)
        {
            throw Exception("Invalid mask " + inputString.substr(9, 2));
        }

        parser.connect(mask, inputString.substr(13, inputString.length() - 13));
    }
    else if (inputString.substr(0, 10) == "-disconnect")
    {
        parser.disconnect();
    }
    else if(inputString.substr(0, 7) == "-upload")
    {
        string filename = inputString.substr(9, inputString.length() - 9);
        parser.uploadFile(filename);
    }
    else if(inputString.substr(0, 7) == "-delete")
    {
        string filename = inputString.substr(9, inputString.length() - 9);
        parser.deleteFile(filename);
    }
    else if(inputString.substr(0, 9) == "-download")
    {
        string filename = inputString.substr(9, inputString.length() - 11);
        parser.downloadFile(filename);
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
}

UI::UI() {
    parser = Parser();
}

int UI::Parser::connect(long int mask, const std::string &address)
{
    return 0;
}

int UI::Parser::disconnect() {
    return 0;
}

int UI::Parser::uploadFile(string file) {
    return 0;
}

int UI::Parser::deleteFile(string file) {
    return 0;
}

int UI::Parser::downloadFile(string file) {
    return 0;
}

void UI::Parser::listAll() {

}

void UI::Parser::listLocal() {

}

UI::Parser::Parser() = default;
