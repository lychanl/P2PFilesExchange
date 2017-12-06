
#ifndef P2PFILESEXCHANGE_SERVER_H
#define P2PFILESEXCHANGE_SERVER_H

#include <pthread.h>

namespace conn
{
	class Server
	{
	public:

		int run();
		void stop();

		~Server();
	protected:
		class GlobalServer
		{
		public:
			GlobalServer();

			int run();
			void stop();

			~GlobalServer();
		protected:
			//function should read in non-blocking manner from socket
			//server loop will break if returned value will be different from 0
			virtual int _run(int socketToRead) = 0;

			virtual int initSocket() = 0;
		private:
			static void* runLoop(void* server);

			bool running;

			int socket;
			int signalfd;

			pthread_t thread;
		};

		//instance of global server
		//will be deleted on server destruction
		Server(GlobalServer* server);
	private:
		Server(const Server& server) {}
		Server(const Server&& server) noexcept {}
		const Server& operator=(const Server& server) {}
		GlobalServer* server = nullptr;
	};
}


#endif //P2PFILESEXCHANGE_SERVER_H
