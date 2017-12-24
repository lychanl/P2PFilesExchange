#ifndef P2PFILESEXCHANGE_UI_H
#define P2PFILESEXCHANGE_UI_H

#include <iostream>
#include <regex>
#include <csignal>

using namespace std;

namespace ui
{
    extern bool interruptSignalFlag;

    class UI
    {
    public:
        UI();

        int parseUserInput(const std::string& inputString);

        int start();
    private:
        static const int DISCONNECT_RETURN_VALUE = 200;

        struct sigaction sigact;

        void initSignals();

        class Parser
        {
        public:
            Parser();

            // -connect mask address
            int connect(long int mask, const std::string& address);
            // -disconnect
            int disconnect();
            // -upload filename
            int uploadFile(string file);
            // -delete filename
            int deleteFile(string file);
            // -download filename
            int downloadFile(string file);
            // -la
            void listAll();
            // -l
            void listLocal();
        };

        Parser parser;
    };
}

#endif //P2PFILESEXCHANGE_UI_H
