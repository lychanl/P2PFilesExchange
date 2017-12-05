
#ifndef P2PFILESEXCHANGE_TCPSERVER_H
#define P2PFILESEXCHANGE_TCPSERVER_H

#include "TCPConnection.h"
#include <pthread.h>

namespace conn
{
	class TCPServer
	{
	public:
		typedef void(*connHandler)(TCPConnection&);

		//creates new server
		//that listens on given port
		TCPServer(unsigned short int port, connHandler handler);
		int run();
		void stop();
		~TCPServer();

	private:
		TCPServer(TCPServer& tcpServer) {};
		TCPServer(TCPServer&& tcpServer) noexcept {};
		TCPServer& operator=(const TCPServer& tcpServer) {};

		class GlobalTCPServer
		{
		public:
			GlobalTCPServer(unsigned short int port, connHandler handler);
			int run();
			void stop();

			~GlobalTCPServer();
		private:
			connHandler handler;
			int socket;
			int sigfd;
			struct sockaddr_in bindAddress;

			static void* _run(void* server);

			pthread_t thread;
			bool running;
		};

		GlobalTCPServer* server = nullptr;
	};
}


#endif //P2PFILESEXCHANGE_TCPSERVER_H
