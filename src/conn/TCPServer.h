
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
		TCPServer(TCPServer& tcpServer);
		TCPServer(TCPServer&& tcpServer) noexcept;
		int run();
		void stop();
		~TCPServer();

	private:
		class GlobalTCPServer
		{
		public:
			GlobalTCPServer(unsigned short int port, connHandler handler);
			int run();
			void stop();

			void newRef();
			//Returns number of remaining references
			//If none and server is running stops server
			int removeRef();
			~GlobalTCPServer();
		private:
			connHandler handler;
			int socket;
			int sigfd;
			struct sockaddr_in bindAddress;

			static void* _run(void* server);
			void _stop();

			pthread_mutex_t mutex;
			pthread_t thread;
			int references;
			bool running;
		};

		GlobalTCPServer* server = nullptr;
	};
}


#endif //P2PFILESEXCHANGE_TCPSERVER_H
