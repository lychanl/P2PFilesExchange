
#ifndef P2PFILESEXCHANGE_UDPBROADCASTER_H
#define P2PFILESEXCHANGE_UDPBROADCASTER_H

#include "IPv4Address.h"

#include <pthread.h>
#include <proto/Package.h>

namespace conn
{
	class UDPBroadcaster
	{
	public:
		explicit UDPBroadcaster(const IPv4Address& broadcastAddress);
		~UDPBroadcaster();

		int open();
		void close();

		int send(proto::Package* package);
		int send(const void* buffer, int size);

	private:
		UDPBroadcaster(const UDPBroadcaster& b) {}
		UDPBroadcaster(const UDPBroadcaster&& b) noexcept {}
		const UDPBroadcaster&operator=(const UDPBroadcaster& b) {};

		int socket;
		bool opened;

		sockaddr_in addr;

		pthread_mutex_t mutex;
	};
}


#endif //P2PFILESEXCHANGE_UDPBROADCASTER_H
