#ifndef P2PFILESEXCHANGE_UI_H
#define P2PFILESEXCHANGE_UI_H

#include <iostream>
#include <regex>
#include <csignal>
#include <files/FileManager.h>
#include <conn/IPv4Address.h>
#include <proto/Protocols.h>
using namespace std;

namespace ui
{
    extern bool interruptSignalFlag;
    extern bool connected;

    class UI
    {
    public:
        UI(files::FileManager *fileManager, conn::TCPServer *tcpServer, conn::UDPServer *udpServer);

        virtual ~UI();

        int parseUserInput(const std::string& inputString);

        int start();
    private:
        static const int DISCONNECT_RETURN_VALUE = 200;

        struct sigaction sigact;

        void initSignals();

        class Parser
        {
        public:
            Parser(files::FileManager *fileManager, conn::TCPServer *tcpServer, conn::UDPServer *udpServer);

            // connect mask address
            int connect(const std::string& address);
            // disconnect
            int disconnect();
            // upload filename
            int uploadFile(string file);
            // delete filename
            int deleteFile(string file);
            // download filename
            int downloadFile(string fileSrc, string fileDst);
            // la
            void listAll();
            // l
            void listLocal();

            virtual ~Parser();

        private:
            conn::TCPServer *tcpServer;
            conn::UDPServer *udpServer;
            files::FileManager *fileManager;
        };

        Parser parser;
    };

    vector<string> split(const string &s, char delimiter);
}

#endif //P2PFILESEXCHANGE_UI_H
