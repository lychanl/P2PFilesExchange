
#ifndef P2PFILESEXCHANGE_TCPCONNECTION_H
#define P2PFILESEXCHANGE_TCPCONNECTION_H

#include "IPv4Address.h"

#include <sys/socket.h>
#include <exception.h>
#include <proto/Package.h>

namespace conn
{
	class TCPConnection
	{
	public:
		typedef enum
		{
			STATUS_OPEN,
			STATUS_CLOSED,
			STATUS_ERR, //an error occurred, but connection is kept open
			STATUS_FATAL, //an error occured and connection is no longer open
		} Status;

		//Creates and opens connection
		//Check status if error occured
		explicit TCPConnection(const IPv4Address& address);
		//Creates connection using an already open socket
		TCPConnection(int socket, const IPv4Address& address);

		~TCPConnection();

		//Returns:
		// 0 if connected
		// -1 if failed
		int reconnect();
		//Returns:
		// 0 if success
		// -1 if failed
		int close();

		static void waitForNoConnections();
		static void enableConnections();

		//Sends n bytes to remote
		//Returns:
		// 0 if ok
		// -1 if error
		int send(const void* buffer, size_t n);
		int send(proto::Package* package);
		//Reads exactly n bytes from remote
		//Returns:
		// 0 if ok
		// -1 if error
		int recv(void* buffer, size_t n);
		int recv(proto::Package* package);
		int recvNoId(proto::Package* package);

		Status getStatus() const;
		int getError() const;

		const IPv4Address& getRemoteAddress() const;
	private:
		//Returns:
		// 0 if ok
		// -1 if error
		int createSocket();
		//Returns:
		// 0 if ok
		// -1 if error
		int connect();

		Status status = STATUS_OPEN;
		int error = 0;
		IPv4Address remoteAddr;
		int socket;

		TCPConnection(TCPConnection& conn) : remoteAddr(0u, 0u) {};
		TCPConnection(TCPConnection&& conn) noexcept : remoteAddr(0u, 0u) {};
		TCPConnection& operator=(const TCPConnection& conn) {};
	};
}

#endif //P2PFILESEXCHANGE_TCPCONNECTION_H
