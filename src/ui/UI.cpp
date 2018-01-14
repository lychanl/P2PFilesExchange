#include <exception.h>
#include <log/Logger.h>
#include <fcntl.h>
#include <conn/TCPServer.h>
#include "UI.h"

using namespace ui;

bool ui::interruptSignalFlag = false;

int UI::parseUserInput(const std::string &inputString)
{
    if (interruptSignalFlag)
    {
        return parser.disconnect();
    }

    if (inputString.substr(0, 7) == "connect")
    {
        if ((inputString.length() <= 12))
        {
            cout << "Invalid adress" << endl;
            return -1;
        }

        return parser.connect(inputString.substr(sizeof("connect"), inputString.length() - 8));
    }
    else if (inputString == "disconnect")
    {
       return parser.disconnect();
    }
    else if(inputString.substr(0, 6) == "upload")
    {
        string filename;

        if (inputString.length() <= 7)
        {
            cout << "Invalid filename" << endl;
            return -3;
        }

        filename = inputString.substr(7, inputString.length() - 7);

        return parser.uploadFile(filename);
    }
    else if(inputString.substr(0, 6) == "delete")
    {
        string filename;

        if (inputString.length() <= 7)
        {
            cout << "Invalid filename" << endl;
            return -4;
        }

        filename = inputString.substr(7, inputString.length() - 7);

        return parser.deleteFile(filename);
    }
    else if(inputString.substr(0, 8) == "download")
    {
        string filename;

        if (inputString.length() <= 9)
        {
            cout << "Invalid filename" << endl;
            return -5;
        }

        filename = inputString.substr(9, inputString.length() - 9);

        return parser.downloadFile(filename);
    }
    else if(inputString == "la")
    {
        parser.listAll();
    }
    else if(inputString == "l")
    {
        parser.listLocal();
    }
    else
    {
        cout << "Invalid command" << endl;
        cout << "Available commands:" << endl;
        cout << "connect <address>" << endl;
        cout << "disconnect" << endl;
        cout << "upload <filename>" << endl;
        cout << "delete <filename>" << endl;
        cout << "download <filename>" << endl;
        cout << "la" << endl;
        cout << "l" << endl;
    }
    return 0;
}

UI::UI(files::FileManager *fileManager, conn::TCPServer *tcpServer, conn::UDPServer *udpServer): parser(fileManager,tcpServer, udpServer)
{
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

int UI::Parser::connect(const std::string &address)
{
    conn::IPv4Address::setLocalAddress(address);
    fileManager->setLocalNode(conn::IPv4Address::getLocalAddress(0));
    tcpServer->run();
    udpServer->run();
    proto::Protocols::getInstance().connect();
    return 0;
}

int UI::Parser::disconnect()
{
    std::cout << "disconnect" << std::endl;
    proto::Protocols::getInstance().disconnect();
    tcpServer->stop();
    udpServer->stop();
    return DISCONNECT_RETURN_VALUE;
}

int UI::Parser::uploadFile(string file)
{
    files::Descriptor descriptor = fileManager->addDiskFile(file);
    if (descriptor.name[0] != '\0')
        proto::Protocols::getInstance().uploadFile(descriptor);

    return 0;
}

int UI::Parser::deleteFile(string file)
{
    for(auto a: fileManager->listAllFiles())
    {
        if (a.name == file)
        {
            proto::Protocols::getInstance().deactivateFile(a);
            break;
        }
    }
    return 0;
}

int UI::Parser::downloadFile(string file)
{
    for(auto a: fileManager->listAllFiles())
    {
        Logger::getInstance().logMessage("A: " + file);

        if (a.name == file)
        {
            Logger::getInstance().logMessage("before getInstance");

            int fd = open(file.c_str(), O_RDWR);
            proto::Protocols::getInstance().getFile(a, fd);
            Logger::getInstance().logMessage("after getInstance");
            break;
        }
    }
    return 0;
}

void UI::Parser::listAll()
{
    for(auto a: fileManager->listAllFiles())
    {
        std::cout << a.owner << " " << a.name << " " << a.date << std::endl;
    }
}

void UI::Parser::listLocal()
{
    for(auto a: fileManager->listLocalFiles())
    {
        std::cout << a.owner << " " << a.name << " " << a.date << std::endl;
    }
}

UI::Parser::Parser(files::FileManager* filesManager, conn::TCPServer *tcpServer, conn::UDPServer *udpServer)
{
    this->udpServer = udpServer;
    this->tcpServer = tcpServer;
    fileManager = filesManager;
}

UI::Parser::~Parser()
= default;

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

UI::~UI()
{
}
