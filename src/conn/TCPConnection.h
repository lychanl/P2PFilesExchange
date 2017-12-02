
#ifndef P2PFILESEXCHANGE_TCPCONNECTION_H
#define P2PFILESEXCHANGE_TCPCONNECTION_H

#include "IPv4Address.h"

#include <sys/socket.h>
#include <exception.h>

namespace conn
{
	class TCPConnection
	{
	public:
		enum Status
		{
			STATUS_OPEN,
			STATUS_CLOSED,
			STATUS_ERR, //an error occurred, but connection is kept open
			STATUS_FATAL, //an error occured and connection is no longer open
		};

		//Creates and opens connection
		//Check status if error occured
		TCPConnection(const IPv4Address& address);
		//Creates connection using an already open socket
		TCPConnection(int socket, const IPv4Address& address);

		TCPConnection(TCPConnection& conn) : remoteAddr(0li, 0) { throw Exception("Forbidden!"); };
		TCPConnection(TCPConnection&& conn) : remoteAddr(0li, 0) { throw Exception("Forbidden!"); };

		~TCPConnection();

		//Returns:
		// 0 if connected
		// -1 if failed
		int reconnect();
		//Returns:
		// 0 if success
		// -1 if failed
		int close();

		static void waitForAllClosed();
		static void enableConnections();

		//Sends n bytes to remote
		//Returns:
		// 0 if ok
		// -1 if error
		int send(const void* buffer, int n);
		//Reads exactly n bytes from remote
		//Returns:
		// 0 if ok
		// -1 if error
		int recv(void* buffer, int n);

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
	};
}

#endif //P2PFILESEXCHANGE_TCPCONNECTION_H
