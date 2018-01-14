
#ifndef P2PFILESEXCHANGE_UDPSERVER_H
#define P2PFILESEXCHANGE_UDPSERVER_H


#include "IPv4Address.h"
#include "Server.h"

namespace conn
{
	class UDPServer : public Server
	{
	public:
		typedef void (*handler)(void* buffer, int recvData, const IPv4Address& sender);

		//creates a UDP server (iterating)
		//that will listen on given port
		UDPServer(int port, int maxDGramSize, handler handler);
		~UDPServer();
	private:

		class GlobalUDPServer : public Server::GlobalServer
		{
		public:
			GlobalUDPServer(int port, int maxDGramSize, handler handler);
			~GlobalUDPServer();
		private:

			int _run(int socketToRead);

			int initSocket();

			sockaddr_in address;

			handler handl;
			char *buffer;
			int bufSize;
		};

		GlobalUDPServer* server;
	};
}


#endif //P2PFILESEXCHANGE_UDPSERVER_H
