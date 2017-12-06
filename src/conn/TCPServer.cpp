
#include "TCPServer.h"

#include <sys/signalfd.h>
#include <csignal>
#include <unistd.h>

#define ACCEPT_BUFFER_SIZE 256

using namespace conn;

TCPServer::TCPServer(unsigned short int port, connHandler handler)
		: Server(static_cast<GlobalServer*>(new TCPServer::GlobalTCPServer(port, handler)))
{
}

TCPServer::GlobalTCPServer::GlobalTCPServer(unsigned short int port, connHandler handler)
{
	bindAddress = IPv4Address::getAnyAddress(port);
	this->handler = handler;
}

int TCPServer::GlobalTCPServer::initSocket()
{
	int ret = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (ret < 0) return -1;

	if (::bind(ret, reinterpret_cast<sockaddr *>(&this->bindAddress), sizeof(this->bindAddress)) < 0)
	{
		close(ret);
		return -1;
	}

	if (::listen(ret, ACCEPT_BUFFER_SIZE) < 0)
	{
		close(ret);
		return -1;
	}

	return ret;
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

int TCPServer::GlobalTCPServer::_run(int socketToRead)
{

	sockaddr_in client;
	socklen_t clSize = sizeof(sockaddr_in);

	int socket = ::accept4(socketToRead, reinterpret_cast<sockaddr *>(&client), &clSize, SOCK_NONBLOCK);

	if (socket >= 0)
	{
		TCPConnection *conn = new TCPConnection(socket, IPv4Address(client));
		auto arg = new struct connectionThreadArg;

		arg->connection = conn;
		arg->handler = handler;

		pthread_t thread;

		pthread_create(&thread, nullptr, connectionThread, arg);
		pthread_detach(thread);
	} else
	{
		//manage errors (todo)
	}

	return 0;
}