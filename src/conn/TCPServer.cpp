
#include "TCPServer.h"

#include <sys/signalfd.h>
#include <sys/types.h>
#include <csignal>
#include <unistd.h>
#include <pthread.h>

#define ACCEPT_BUFFER_SIZE 256

using namespace conn;

TCPServer::TCPServer(unsigned short int port, connHandler handler)
{
	this->server = new TCPServer::GlobalTCPServer(port, handler);
}

TCPServer::TCPServer(TCPServer &tcpServer)
{
	this->server = tcpServer.server;
	this->server->newRef();
}

TCPServer::TCPServer(TCPServer &&tcpServer) noexcept
{
	this->server = tcpServer.server;
	this->server->newRef();
}

TCPServer::~TCPServer()
{
	if (this->server->removeRef() == 0)
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
	this->references = 1;
	this->running = false;
	this->handler = handler;
	pthread_mutex_init(&this->mutex, nullptr);
}

void TCPServer::GlobalTCPServer::newRef()
{
	pthread_mutex_lock(&this->mutex);
	this->references++;
	pthread_mutex_unlock(&this->mutex);
}

int TCPServer::GlobalTCPServer::removeRef()
{
	pthread_mutex_lock(&this->mutex);
	int ret = --this->references;
	if (ret == 0)
		_stop();
	pthread_mutex_unlock(&this->mutex);

	return ret;
}

TCPServer::GlobalTCPServer::~GlobalTCPServer()
{
	pthread_mutex_destroy(&this->mutex);
}

int TCPServer::GlobalTCPServer::run()
{
	pthread_mutex_lock(&this->mutex);
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
	pthread_mutex_unlock(&this->mutex);
}

void TCPServer::GlobalTCPServer::stop()
{
	pthread_mutex_lock(&this->mutex);
	_stop();
	pthread_mutex_unlock(&this->mutex);
}

struct connectionThreadArg
{
	TCPConnection* connection;
	TCPServer::connHandler handler;
};

void* connectionThread(void* _arg)
{
	struct connectionThreadArg* arg = static_cast<struct connectionThreadArg*>(_arg);

	*arg;

	arg->handler(*arg->connection);


	delete arg->connection;
	delete arg;
}

void* TCPServer::GlobalTCPServer::_run(void* _server)
{
	GlobalTCPServer* server = static_cast<GlobalTCPServer*>(_server);

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
			struct connectionThreadArg* arg = new struct connectionThreadArg;

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

void TCPServer::GlobalTCPServer::_stop()
{
	pthread_kill(this->thread, SIGUSR1);

	pthread_join(this->thread, nullptr);

	close(sigfd);
	close(socket);
}