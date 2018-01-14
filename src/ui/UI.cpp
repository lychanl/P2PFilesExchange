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
        vector<std::string> filenames;

        if (inputString.length() <= 9)
        {
            cout << "Invalid filename" << endl;
            return -5;
        }

        filenames = split(inputString, ' ');

        return parser.downloadFile(filenames[1], filenames[2]);
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
        cout << "download <filename src> <filename dst>" << endl;
        cout << "la (list all)" << endl;
        cout << "l (list local)" << endl;
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

int UI::Parser::downloadFile(string fileSrc, string fileDst)
{
    Logger::getInstance().logDebug(std::string("downloadFile: src: " + fileSrc));
    Logger::getInstance().logDebug(std::string("downloadFile: dst: " + fileDst));

    for(auto a: fileManager->listRemoteFiles())
    {
        if (a.name == fileSrc)
        {
            Logger::getInstance().logMessage("before getInstance");

            const char * base = basename(fileDst.c_str());

            string path = fileDst.substr(0, fileDst.length() - sizeof(base));

            system(std::strncat(const_cast<char *>("mkdir -p "), path.c_str(), path.length()));

            int fd = open(base, O_RDWR | O_CREAT);
            return proto::Protocols::getInstance().getFile(a, fd);
        }
    }

    Logger::getInstance().logMessage(fileSrc + std::string(" not found in Remote Files."));
    return 666;
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

std::vector<std::string> ui::split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        if (token[0] == ' ') token.erase(0, 1);
        if (token.length() > 0)
            tokens.push_back(token);
    }
    return tokens;
}