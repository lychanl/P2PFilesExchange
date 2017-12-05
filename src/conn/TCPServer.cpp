
#include "TCPServer.h"

#include <sys/signalfd.h>
#include <csignal>
#include <unistd.h>

#define ACCEPT_BUFFER_SIZE 256

using namespace conn;

TCPServer::TCPServer(unsigned short int port, connHandler handler)
{
	this->server = new TCPServer::GlobalTCPServer(port, handler);
}

TCPServer::~TCPServer()
{
	delete this->server;
}

int TCPServer::run()
{
	return this->server->run();
}

void TCPServer::stop()
{
	this->server->stop();
}

TCPServer::GlobalTCPServer::GlobalTCPServer(unsigned short int port, connHandler handler)
{
	bindAddress = IPv4Address::getAnyAddress(port);
	this->running = false;
	this->handler = handler;
}


TCPServer::GlobalTCPServer::~GlobalTCPServer()
{
	if (this->running)
		this->stop();
}

int TCPServer::GlobalTCPServer::run()
{
	if (!running)
	{
		this->socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (socket < 0) return -1;

		if (::bind(this->socket, (sockaddr *) &this->bindAddress, sizeof(this->bindAddress)) < 0)
		{
			close(this->socket);
			return -1;
		}

		if (::listen(this->socket, ACCEPT_BUFFER_SIZE) < 0)
		{
			close(this->socket);
			return -1;
		}

		sigset_t mask;
		sigemptyset(&mask);
		sigaddset(&mask, SIGUSR1);
		sigfd = ::signalfd(-1, &mask, SFD_NONBLOCK);

		if (sigfd < 0)
		{
			close(this-> socket);
			return -1;
		}

		pthread_create(&this->thread, nullptr, _run, this);
	}

	running = true;
}

struct connectionThreadArg
{
	TCPConnection* connection;
	TCPServer::connHandler handler;
};

void* connectionThread(void* _arg)
{
	auto arg = static_cast<struct connectionThreadArg*>(_arg);

	arg->handler(*arg->connection);


	delete arg->connection;
	delete arg;
}

void* TCPServer::GlobalTCPServer::_run(void* _server)
{
	auto server = static_cast<GlobalTCPServer*>(_server);

	signalfd_siginfo siginfo;

	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);

	pthread_sigmask(SIG_BLOCK, &mask, nullptr);

	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(server->socket, &fds);
	FD_SET(server->sigfd, &fds);

	int nfds = server->socket > server->sigfd ? server->socket : server->sigfd;

	while (true)
	{
		//int fd = select(nfds, &fds, nullptr, nullptr, nullptr);

		//if (read(server->sigfd, &siginfo, sizeof(signalfd_siginfo)) > 0)
		//	break;

		sockaddr_in client;
		socklen_t clSize = sizeof(sockaddr_in);

		int socket = accept(server->socket, reinterpret_cast<sockaddr*>(&client), &clSize);//::accept4(server->socket, reinterpret_cast<sockaddr*>(&client), &clSize, SOCK_NONBLOCK);

		if (socket >= 0)
		{
			TCPConnection* conn = new TCPConnection(socket, IPv4Address(client));
			auto arg = new struct connectionThreadArg;

			arg->connection = conn;
			arg->handler = server->handler;

			pthread_t thread;

			pthread_create(&thread, nullptr, connectionThread, arg);
			pthread_detach(thread);
		}
		else
		{
			//manage errors (todo)
		}

		break;
	}

	return nullptr;

}

void TCPServer::GlobalTCPServer::stop()
{
	pthread_kill(this->thread, SIGUSR1);

	pthread_join(this->thread, nullptr);

	close(sigfd);
	close(socket);
}