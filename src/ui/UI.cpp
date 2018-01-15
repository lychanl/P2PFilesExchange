#include <exception.h>
#include <log/Logger.h>
#include <fcntl.h>
#include <conn/TCPServer.h>
#include <fstream>
#include "UI.h"

using namespace ui;

bool ui::interruptSignalFlag = false;
bool ui::connected = false;

int UI::parseUserInput(const std::string &inputString)
{
    if (interruptSignalFlag)
    {
        if (connected)
            return parser.disconnect();
        else
            return DISCONNECT_RETURN_VALUE;
    }

    vector<std::string> filenames;

    filenames = ui::split(inputString, ' ');

    if (filenames.empty())
    {
        std::cout << "invalid command" << std::endl;
        return 5;
    }

    if (filenames[0] == "connect")
    {
        if (filenames.size() < 2)
        {
            Logger::getInstance().logMessage("Not enough arguments for connect");
            return 1;
        }

        if (connected)
        {
            Logger::getInstance().logMessage("Node already connected");
            return 4;
        }

        parser.connect(filenames[1]);
    }
    else if (filenames[0] == "disconnect")
    {
        if (connected) {
            Logger::getInstance().logDebug("before disconnect");
            return parser.disconnect();
        }
    }
    else if(filenames[0] == "upload")
    {
        if (!connected){
            Logger::getInstance().logDebug("Not connected");
            return 8;
        }

        if (filenames.size() != 2){
            Logger::getInstance().logMessage("Invalid arguments number for upload");
            return 2;
        }

        return parser.uploadFile(filenames[1]);
    }
    else if(filenames[0] == "delete")
    {
        if (!connected){
            Logger::getInstance().logDebug("Not connected");
            return 8;
        }

        if (filenames.size() != 2){
            Logger::getInstance().logMessage("Invalid arguments number for delete");
            return 3;
        }

        return parser.deleteFile(filenames[1]);
    }
    else if(filenames[0] == "download")
    {
        if (!connected){
            Logger::getInstance().logDebug("Not connected");
            return 8;
        }

        if (filenames.size() != 3){
            Logger::getInstance().logMessage("Invalid arguments number for download");
            return 3;
        }

        return parser.downloadFile(filenames[1], filenames[2]);
    }
    else if(filenames[0] == "la")
    {
        if (!connected){
            Logger::getInstance().logDebug("Not connected");
            return 8;
        }

        parser.listAll();
    }
    else if(inputString == "l")
    {
        if (!connected){
            Logger::getInstance().logDebug("Not connected");
            return 8;
        }

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
        filenames.clear();
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
    if (proto::Protocols::getInstance().connect() == proto::Protocols::Result::OK){
        Logger::getInstance().logMessage("Connected.");
        ui::connected = true;
    }

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

    for(auto a: fileManager->listAllFiles())
    {
        if (a.name == fileSrc)
        {
            Logger::getInstance().logMessage("before getInstance");

            const char * base = basename(fileDst.c_str());

            string path = fileDst.substr(0, fileDst.length() - strlen(base));


            system(std::strncat(const_cast<char *>("mkdir -p "), path.c_str(), path.length()));

            std::vector<files::Descriptor> local = fileManager->listLocalFiles();

            if (std::count(local.begin(), local.end(), a) > 0)
            {
                std::ifstream  src(strncat(const_cast<char *>(fileManager->getFileDir().c_str()), fileSrc.c_str(), fileSrc.length()), std::ios::binary);
                std::ofstream  dst(fileDst, std::ios::binary);

                dst << src.rdbuf();
            }
            else
            {
                int fd = open(fileDst.c_str(), O_RDWR | O_CREAT);

                return proto::Protocols::getInstance().getFile(a, fd);
            }
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

UI::Parser::~Parser() = default;

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

UI::~UI() = default;

std::vector<std::string> ui::split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        if (token[0] == delimiter) token.erase(0, 1);
        if (token.length() > 0)
            tokens.push_back(token);
    }
    return tokens;
}