
#ifndef P2PFILESEXCHANGE_PROTOCOLS_H
#define P2PFILESEXCHANGE_PROTOCOLS_H


#include <conn/UDPBroadcaster.h>
#include <conn/UDPServer.h>
#include <conn/TCPConnection.h>
#include <files/FileManager.h>

namespace proto
{
	class Protocols
	{
	public:
		enum Result
		{
			OK,
			FILE_REMOVED,
			CANNOT_FIND_FILE,
			FAILED
		};

		// arguments must be accessible from all threads that will use this class,
		// including tcp and udp server loops
		static void init(conn::UDPBroadcaster* broadcaster, files::FileManager* fileManager);
		static Protocols& getInstance();

		static void udpHandler(void* buffer, int recvData, const conn::IPv4Address& sender);
		static void tcpHandler(conn::TCPConnection& conn);

		void uploadFile(const files::Descriptor& file); //asynchronous
		Result connect(); //synchronous
		void disconnect(); //synchronous
		void missingNode(); //synchronous
		Result getFile(const files::Descriptor& file, int fd); //synchronous
		Result deactivateFile(const files::Descriptor& file); //synchronous

	private:
		conn::UDPBroadcaster* broadcaster;
		files::FileManager* fileManager;
		static Protocols* instance;

		bool redistributingInit;
		pthread_mutex_t redistributingInitMutex;

		void broadcastFilesList();
		void redistribute();
		void sendFile();
		void receiveFile();
	};
}


#endif //P2PFILESEXCHANGE_PROTOCOLS_H
