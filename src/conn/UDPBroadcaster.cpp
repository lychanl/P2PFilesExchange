
#include "UDPBroadcaster.h"
#include <unistd.h>

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
	}

	pthread_mutex_unlock(&this->mutex);

	return ret;
}