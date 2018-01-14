
#ifndef P2PFILESEXCHANGE_PROTOCOLS_H
#define P2PFILESEXCHANGE_PROTOCOLS_H


#include <conn/UDPBroadcaster.h>
#include <conn/UDPServer.h>
#include <conn/TCPConnection.h>
#include <files/FileManager.h>
#include <set>
#include <map>

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
		void missingNode(const conn::IPv4Address& addr); //synchronous
		Result getFile(const files::Descriptor& file, int fd); //synchronous
		Result deactivateFile(const files::Descriptor& file); //synchronous

		conn::IPv4Address getNode(const files::Descriptor& file);

	private:
		conn::UDPBroadcaster* broadcaster;
		files::FileManager* fileManager;
		static Protocols* instance;

		pthread_mutex_t nodesMutex;
		std::map<unsigned int, long long int> nodes;
		pthread_mutex_t greetedNodesMutex;
		std::set<unsigned int> greetedNodes;

		volatile bool isRedistributing = false;

		void broadcastFilesList();
		void redistribute(bool separateThread, bool disconnecting);
		void sendFile(files::Descriptor& descriptor, conn::TCPConnection& connection);
		void receiveFile(files::Descriptor& descriptor, conn::TCPConnection& connection);

		static void* _redistribute(void*);
		static void* _uploadFile(void* file);
		static void* _keepAliveSender(void*);
		static void* _keepAliveMonitor(void*);

		volatile bool keepAliveThreadsRunning = false;

		Protocols() {}
		Protocols(const Protocols&) {}
		Protocols(Protocols&&) {}
		Protocols& operator=(const Protocols&) {};
	};
}


#endif //P2PFILESEXCHANGE_PROTOCOLS_H
