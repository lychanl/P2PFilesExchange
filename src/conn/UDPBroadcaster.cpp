
#include "UDPBroadcaster.h"
#include <unistd.h>
#include <log/Logger.h>

using namespace conn;

UDPBroadcaster::UDPBroadcaster(const IPv4Address& broadcastAddress)
{
	this->addr = broadcastAddress.getSockaddr();
	pthread_mutex_init(&this->mutex, nullptr);
	opened = false;
}

UDPBroadcaster::~UDPBroadcaster()
{
	if (opened)
		close();

	pthread_mutex_destroy(&this->mutex);
}

int UDPBroadcaster::open()
{
	pthread_mutex_lock(&this->mutex);

	if (!this->opened)
	{
		this->socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (socket < 0)
		{
			Logger::getInstance().logError("UDPBroadcaster: Error while creating socket. Errno: " + errno);
			pthread_mutex_unlock(&this->mutex);
			return -1;
		}

		int bc = 1;

		::setsockopt(this->socket, SOL_SOCKET, SO_BROADCAST, &bc, sizeof(bc));
	}
	this->opened = true;

	pthread_mutex_unlock(&this->mutex);

	return 0;
}

void UDPBroadcaster::close()
{
	pthread_mutex_lock(&this->mutex);

	if (opened)
		::close(this->socket);

	opened = false;

	pthread_mutex_unlock(&this->mutex);
}

int UDPBroadcaster::send(const void *buffer, int size)
{
	pthread_mutex_lock(&this->mutex);

	int ret = -1;

	if (opened)
	{
		if (sendto(this->socket, buffer, size, 0, reinterpret_cast<sockaddr*>(&this->addr), sizeof(this->addr)) == size)
			ret = 0;
		else
			Logger::getInstance().logError("UDPBroadcaster: Error while sending. Errno: " + errno);
	}

	pthread_mutex_unlock(&this->mutex);

	return ret;
}

int UDPBroadcaster::send(proto::Package *package)
{
	int size = package->getSerializedSize();
	char* buffer = new char[size];

	package->serializeTo(buffer);

	int ret = send(buffer, size);

	delete[] buffer;

	return ret;
}