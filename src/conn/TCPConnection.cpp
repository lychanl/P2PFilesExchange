#include "TCPConnection.h"

#include <unistd.h>
#include <pthread.h>
#include <log/Logger.h>

int connections = 0;
pthread_mutex_t connectionsMutex;
pthread_cond_t noConnectionsCond;

using namespace conn;

TCPConnection::TCPConnection(int socket, const IPv4Address& address) : socket(socket), remoteAddr(address)
{
	pthread_mutex_lock(&connectionsMutex);

	connections++;

	pthread_mutex_unlock(&connectionsMutex);
}

TCPConnection::TCPConnection(const IPv4Address& address) : remoteAddr(address)
{
	if (createSocket() != 0)
	{
		Logger::getInstance().logError("TCPConnection: Error while creating or binding socket. Errno: " + error);
		return;
	}

	connect();
}

TCPConnection::~TCPConnection()
{
	close();
}

int TCPConnection::createSocket()
{
	static sockaddr_in localaddr = IPv4Address::getAnyAddress(0);

	socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket <= 0)
	{
		error = errno;
		status = STATUS_FATAL;
		return -1;
	}

	if (::bind(socket, (struct sockaddr*) &localaddr, sizeof(sockaddr_in)) != 0)
	{
		error = errno;
		::close(socket);
		socket = 0;
		status = STATUS_FATAL;
		return -1;
	}
	return 0;
}

int TCPConnection::connect()
{
	pthread_mutex_lock(&connectionsMutex);

	if (::connect(socket, reinterpret_cast<const sockaddr*>(&remoteAddr.getSockaddr()), sizeof(sockaddr_in)) != 0)
	{
		Logger::getInstance().logError("TCPConnection: Error while connecting. Errno: " + errno);
		pthread_mutex_unlock(&connectionsMutex);

		error = errno;
		status = STATUS_FATAL;
		return -1;
	}

	connections++;

	pthread_mutex_unlock(&connectionsMutex);

	return 0;
}

int TCPConnection::reconnect()
{
	if (status == STATUS_OPEN)
		return 0;

	if (socket == 0)
	{
		if (createSocket() != 0)
			return -1;
	}

	if (connect() != 0)
		return -1;

	status = STATUS_OPEN;

	return 0;
}

int TCPConnection::close()
{
	if (socket != 0)
	{
		::close(socket);

		pthread_mutex_lock(&connectionsMutex);

		connections--;
		if (connections == 0)
			pthread_cond_signal(&noConnectionsCond);

		pthread_mutex_unlock(&connectionsMutex);

	}

	socket = 0;
	status = STATUS_CLOSED;
}

int TCPConnection::getError() const
{
	return error;
}

TCPConnection::Status TCPConnection::getStatus() const
{
	return status;
}

const IPv4Address& TCPConnection::getRemoteAddress() const
{
	return remoteAddr;
}

int TCPConnection::send(const void *buffer, size_t n)
{
	if (status == STATUS_CLOSED || status == STATUS_FATAL)
		return -1;

	size_t offset = 0;
	size_t left = n;
	size_t sent;
	while (left > 0)
	{
		sent = ::send(socket, buffer + offset, left, 0);
		if (sent < 0)
		{
			error = errno;
			status = STATUS_ERR;

			Logger::getInstance().logError("TCPConnection: Error while sending. Errno: " + error);

			return -1;
		}

		left -= sent;
		offset += sent;
	}

	status = STATUS_OPEN;
	return 0;
}

int TCPConnection::send(proto::Package* package)
{
	int size = package->getSerializedSize();
	char* buffer = new char[size];

	package->serializeTo(buffer);

	int ret = send(buffer, size);

	delete[] buffer;

	return ret;
}

int TCPConnection::recv(void *buffer, size_t n)
{
	if (status == STATUS_CLOSED || status == STATUS_FATAL)
		return -1;

	size_t offset = 0;
	size_t left = n;
	size_t received;

	while (left > 0)
	{
		received = ::recv(socket, buffer + offset, left, 0);
		if (received < 0)
		{
			error = errno;
			status = STATUS_ERR;

			Logger::getInstance().logError("TCPConnection: Error while receiving. Errno: " + error);

			return -1;
		}

		left -= received;
		offset += received;
	}
}

int TCPConnection::recv(proto::Package* package)
{
	char* buffer = new char[package->getHeaderSize()];

	if (recv(buffer, package->getHeaderSize()))
	{
		delete[] buffer;
		return -1;
	}

	int dataSize = package->parseHeader(buffer);
	delete[] buffer;

	if (dataSize == 0)
		return 0;

	buffer = new char[dataSize];

	if (recv(buffer, dataSize))
	{
		delete[] buffer;
		return -1;
	}

	package->parseData(buffer);
	delete[] buffer;

	return 0;
}

int TCPConnection::recvNoId(proto::Package* package)
{
	if (package->getHeaderSize() == 4)
		return 0;

	char* buffer = new char[package->getHeaderSize() - 4];

	if (recv(buffer, package->getHeaderSize()) - 4)
	{
		delete[] buffer;
		return -1;
	}

	int dataSize = package->parseHeader(buffer - 4);
	delete[] buffer;

	if (dataSize == 0)
		return 0;

	buffer = new char[dataSize];

	if (recv(buffer, dataSize))
	{
		delete[] buffer;
		return -1;
	}

	package->parseData(buffer);
	delete[] buffer;

	return 0;
}

void TCPConnection::enableConnections()
{
	pthread_mutex_init(&connectionsMutex, nullptr);
	pthread_cond_init(&noConnectionsCond, nullptr);
}

void TCPConnection::waitForNoConnections()
{
	pthread_mutex_lock(&connectionsMutex);
	while (connections > 0)
		pthread_cond_wait(&noConnectionsCond, &connectionsMutex);
	pthread_mutex_unlock(&connectionsMutex);
}