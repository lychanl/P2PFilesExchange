
#include "Server.h"

#include <unistd.h>
#include <csignal>
#include <sys/signalfd.h>
#include <sys/select.h>

using namespace conn;

Server::Server(GlobalServer* server)
{
	this->server = server;
}

Server::~Server()
{
	delete this->server;
}

int Server::run()
{
	return this->server->run();
}

void Server::stop()
{
	this->server->stop();
}

Server::GlobalServer::GlobalServer()
{
	this->running = false;
}

Server::GlobalServer::~GlobalServer()
{
	this->stop();
}

int Server::GlobalServer::run()
{
	if (!running)
	{
		this->socket = initSocket();

		if (socket < 0) return -1;

		sigset_t mask;
		sigemptyset(&mask);
		sigaddset(&mask, SIGUSR1);
		signalfd = ::signalfd(-1, &mask, SFD_NONBLOCK);

		if (signalfd < 0)
		{
			close(this-> socket);
			return -1;
		}

		pthread_create(&this->thread, nullptr, runLoop, this);
	}

	running = true;
}

void* Server::GlobalServer::runLoop(void *_server)
{
	auto server = static_cast<GlobalServer*>(_server);

	signalfd_siginfo siginfo;

	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);

	pthread_sigmask(SIG_BLOCK, &mask, nullptr);

	fd_set fds;

	FD_ZERO(&fds);

	int nfds = (server->socket > server->signalfd ? server->socket : server->signalfd) + 1;

	while (true)
	{
		FD_SET(server->socket, &fds);
		FD_SET(server->signalfd, &fds);

		select(nfds, &fds, nullptr, nullptr, nullptr);

		if (read(server->signalfd, &siginfo, sizeof(signalfd_siginfo)) > 0)
			break;

		if (server->_run(server->socket) != 0)
			break;
	}

	return nullptr;
}

void Server::GlobalServer::stop()
{
	if (running)
	{
		pthread_kill(this->thread, SIGUSR1);

		pthread_join(this->thread, nullptr);

		close(signalfd);
		close(socket);
	}

	running = false;
}