
#ifndef P2PFILESEXCHANGE_TCPSERVER_H
#define P2PFILESEXCHANGE_TCPSERVER_H

#include "TCPConnection.h"
#include "Server.h"

namespace conn
{
	class TCPServer : public Server
	{
	public:
		typedef void(*connHandler)(TCPConnection&);

		//creates new server
		//that listens on given port
		TCPServer(unsigned short int port, connHandler handler);

	private:

		class GlobalTCPServer : public Server::GlobalServer
		{
		public:
			GlobalTCPServer(unsigned short int port, connHandler handler);
		private:
			connHandler handler;
			struct sockaddr_in bindAddress;

			int _run(int socketToRead);

			int initSocket();
		};

		GlobalTCPServer* server = nullptr;
	};
}


#endif //P2PFILESEXCHANGE_TCPSERVER_H
