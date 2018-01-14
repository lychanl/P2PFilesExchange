#include <unistd.h>
#include <proto/packages/HelloPackage.h>
#include <proto/packages/ConnectPackage.h>
#include <proto/packages/DisconnectPackage.h>
#include <proto/packages/DeadbodyPackage.h>
#include <proto/packages/GetPackage.h>
#include <cstring>
#include <proto/packages/NoFilePackage.h>
#include <proto/packages/FilePackage.h>
#include <proto/packages/PutPackage.h>
#include <proto/packages/AbortPackage.h>
#include <proto/packages/DeletePackage.h>
#include <proto/packages/KeepAlivePackage.h>
#include <proto/packages/ListPackage.h>
#include <chrono>
#include <log/Logger.h>
#include "Protocols.h"

using namespace proto;

const int MAX_RERUNS = 1;

Protocols* Protocols::instance = nullptr;

inline static long long int getTime()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

conn::IPv4Address Protocols::getNode(const files::Descriptor &file)
{
	int ret;

	files::DescriptorHasher hasher;
	unsigned hash = hasher(file);

	pthread_mutex_lock(&nodesMutex);

	unsigned n = hash % nodes.size();

	for (auto node : nodes)
		if (n-- == 0)
			ret = node.first;

	pthread_mutex_unlock(&nodesMutex);

	return ret;
}

void Protocols::init(conn::UDPBroadcaster *broadcaster, files::FileManager *fileManager)
{
	instance = new Protocols();
	instance->broadcaster = broadcaster;
	instance->fileManager = fileManager;

	pthread_mutex_init(&instance->nodesMutex, nullptr);
	pthread_mutex_init(&instance->greetedNodesMutex, nullptr);
}

Protocols& Protocols::getInstance()
{
	return *instance;
}

Protocols::Result Protocols::connect()
{
	ConnectPackage connPackage;

	if (broadcaster->send(&connPackage))
		return Result::FAILED;

	redistribute(false, false);

	if (!keepAliveThreadsRunning)
	{
		keepAliveThreadsRunning = true;
		pthread_t sender, monitor;
		pthread_create(&sender, nullptr, _keepAliveSender, nullptr);
		pthread_create(&monitor, nullptr, _keepAliveMonitor, nullptr);
		pthread_detach(sender);
		pthread_detach(monitor);
	}

	return Result::OK;
}

void Protocols::disconnect()
{
	DisconnectPackage disconnectPackage;

	broadcaster->send(&disconnectPackage);

	redistribute(false, true);
}

void Protocols::missingNode(const conn::IPv4Address& addr)
{
	DeadbodyPackage deadbodyPackage;
	deadbodyPackage.setAddress(addr);

	broadcaster->send(&deadbodyPackage);

	redistribute(true, false);
}

Protocols::Result Protocols::getFile(const files::Descriptor &file, int fd)
{
	GetPackage getPackage;
	getPackage.setDescriptor(file);

	conn::IPv4Address addr = conn::IPv4Address(fileManager->getNode(file).getAddress(), conn::IPv4Address::APPLICATION_PORT);

	bool finished = false;

	for (int i = 0; i < MAX_RERUNS; i++)
	{
		do
		{
			conn::TCPConnection connection(addr);
			if (connection.send(&getPackage) != 0)
				break;

			char ID[4];
			if (connection.recv(ID, 4))
				break;

			if (memcmp(ID, NoFilePackage::ID, 4) == 0)
			{
				NoFilePackage noFilePackage;
				connection.recvNoId(&noFilePackage);

				if (noFilePackage.getReason() == NoFilePackage::Reason::DELETED)
					return Result::FILE_REMOVED;
				if (noFilePackage.getReason() == NoFilePackage::Reason::NONE)
					return Result::CANNOT_FIND_FILE;

				addr = noFilePackage.getSentToAddress();
				break;
			}

			FilePackage filePackage;
			if (!connection.recvNoId(&filePackage))
				break;

			if (filePackage.getSize() != 0)
				write(fd, filePackage.getDataPtr(), filePackage.getSize());

			if (filePackage.getLeftSize() - filePackage.getSize())
				finished = true;
		} while (!finished);

		if (finished)
			break;
	}

	if (!finished)
		return Result::FAILED;
	return Result ::OK;
}

void* Protocols::_uploadFile(void* arg)
{
	files::Descriptor* file = static_cast<files::Descriptor*>(arg);

	bool wasRedistibutionAtStart = getInstance().isRedistributing;
	conn::IPv4Address targetAddress = conn::IPv4Address(getInstance().getNode(*file).getAddress(), conn::IPv4Address::APPLICATION_PORT);

	if (targetAddress.getAddress() != conn::IPv4Address::getLocalAddress(0).sin_addr.s_addr)
	{
		int fd = Protocols::getInstance().fileManager->openLocalFile(*file);

		if (fd == -1)
		{
			delete file;
			return nullptr;
		}

		FILE *stream = fdopen(fd, "rb");

		int leftSize = fseek(stream, 0, SEEK_END);
		rewind(stream);

		conn::TCPConnection connection(conn::IPv4Address(targetAddress.getAddress(), conn::IPv4Address::APPLICATION_PORT));

		PutPackage putPackage;
		putPackage.setDescriptor(*file);

		for (int i = 0; i < MAX_RERUNS && leftSize > 0; i++)
		{
			if (connection.send(&putPackage) == 0)
			{
				do
				{
					if (!wasRedistibutionAtStart && getInstance().isRedistributing
						|| !getInstance().fileManager->isActive(*file))
					{
						AbortPackage abortPackage;
						connection.send(&abortPackage);
						break;
					}

					int sizeToSend = leftSize > FilePackage::MAX_DATA_SIZE ? FilePackage::MAX_DATA_SIZE : leftSize;

					char *buffer = nullptr;

					if (sizeToSend > 0)
					{
						buffer = new char[sizeToSend];
						fread(buffer, 1, sizeToSend, stream);
					}

					FilePackage filePackage;
					filePackage.setLeftSize(leftSize);
					filePackage.setSize(sizeToSend);
					filePackage.setDataPtr(buffer);
					if (connection.send(&filePackage) != 0)
					{
						delete[] buffer;
						break;
					}

					leftSize -= sizeToSend;
					delete[] buffer;
					//send to remote node;
				} while (leftSize != 0);
			}
		}

		fclose(stream);

		getInstance().fileManager->closeLocalFile(*file);
		getInstance().fileManager->makeLocalFileRemote(*file, conn::IPv4Address(targetAddress.getAddress(), 0));
	}


	delete file;
}

void Protocols::uploadFile(const files::Descriptor &file)
{
	files::Descriptor* _file = new files::Descriptor(file);

	pthread_t thread;
	pthread_create(&thread, nullptr, _uploadFile, _file);
	pthread_detach(thread);
}

Protocols::Result Protocols::deactivateFile(const files::Descriptor &file)
{
	conn::IPv4Address node = conn::IPv4Address(getNode(file).getAddress(), conn::IPv4Address::APPLICATION_PORT);

	for (int i = 0; i < MAX_RERUNS; i++)
	{
		DeletePackage deletePackage;
		deletePackage.setDescriptor(file);

		conn::TCPConnection connection(node);

		if (connection.send(&deletePackage))
			return Result::OK;
	}

	return Result::FAILED;
}

void Protocols::udpHandler(void *buffer, int recvData, const conn::IPv4Address &sender)
{
	if (sender.getSockaddr().sin_addr.s_addr == conn::IPv4Address::getLocalAddress(0).sin_addr.s_addr)
		return;

	Logger::getInstance().logDebug("Received datagram. Package ID: " + std::string(
			{
					((char *) buffer)[0],
					((char *) buffer)[1],
					((char *) buffer)[2],
					((char *) buffer)[3],
					'\0'
			}) + " Sender: " + (std::string)sender);

	if (memcmp(buffer, ConnectPackage::ID, 4) == 0)
	{
		Protocols::getInstance().redistribute(true, false);
	}
	else if (memcmp(buffer, DeadbodyPackage::ID, 4) == 0)
	{
		Protocols::getInstance().redistribute(true, false);
	}
	else if (memcmp(buffer, DisconnectPackage::ID, 4) == 0)
	{
		Protocols::getInstance().redistribute(true, false);
	}
	else if (memcmp(buffer, HelloPackage::ID, 4) == 0)
	{
		pthread_mutex_lock(&Protocols::getInstance().greetedNodesMutex);

		if (Protocols::getInstance().isRedistributing)
			Protocols::getInstance().greetedNodes.insert(sender.getAddress());

		pthread_mutex_unlock(&Protocols::getInstance().greetedNodesMutex);
	}
	else if (memcmp(buffer, KeepAlivePackage::ID, 4) == 0)
	{
		pthread_mutex_lock(&Protocols::getInstance().nodesMutex);

		Protocols::getInstance().nodes[sender.getAddress()] = getTime();

		pthread_mutex_unlock(&Protocols::getInstance().nodesMutex);
	}
	else if (memcmp(buffer, ListPackage::ID, 4) == 0)
	{
		ListPackage listPackage;
		int headerSize = static_cast<Package*>(&listPackage)->parseHeader(buffer);
		static_cast<Package*>(&listPackage)->parseData((char*)buffer + headerSize);

		Protocols::getInstance().fileManager->addRemoteFiles(listPackage.getFiles(), conn::IPv4Address(sender.getAddress(), 0), listPackage.getTime());
	}
}

void Protocols::tcpHandler(conn::TCPConnection &conn)
{
	char ID[4];

	if (conn.recv(ID, 4) != 0)
		return;

	Logger::getInstance().logDebug("Received connection. Package ID: " + std::string({ID[0], ID[1], ID[2], ID[3], '\0'})
								   + " Sender: " + (std::string)conn.getRemoteAddress());

	if (memcmp(ID, DeletePackage::ID, 4) == 0)
	{
		DeletePackage package;
		if (conn.recvNoId(&package) != 0)
			return;

		if (Protocols::getInstance().fileManager->deactivateLocalFile(package.getDescriptor()) != 0)
		{
			Protocols::getInstance().deactivateFile(package.getDescriptor());
		}

	}
	else if (memcmp(ID, GetPackage::ID, 4) == 0)
	{
		GetPackage package;
		if (conn.recvNoId(&package) != 0)
			return;

		Protocols::getInstance().sendFile(package.getDescriptor(), conn);
	}
	else if (memcmp(ID, PutPackage::ID, 4) == 0)
	{
		PutPackage package;
		if (conn.recvNoId(&package) != 0)
			return;

		Protocols::getInstance().receiveFile(package.getDescriptor(), conn);
	}
}

void Protocols::sendFile(files::Descriptor& descriptor, conn::TCPConnection& connection)
{
	NoFilePackage noFilePackage;

	if (!fileManager->isActive(descriptor))
	{
		noFilePackage.setReason(NoFilePackage::Reason::DELETED);

		connection.send(&noFilePackage);
		return;
	}

	int fd = fileManager->openLocalFile(descriptor);

	if (fd == -1)
	{
		conn::IPv4Address node = fileManager->getNode(descriptor);
		if (node.getAddress() == 0)
			noFilePackage.setReason(NoFilePackage::Reason::NONE);
		else
		{
			noFilePackage.setReason(NoFilePackage::Reason::AWAY);
			noFilePackage.setSentToAddress(node);
		}

		connection.send(&noFilePackage);
		return;
	}

	FILE* fstream = fdopen(fd, "rb");

	int leftSize = fseek(fstream, 0, SEEK_END);
	rewind(fstream);

	char* buffer = new char[FilePackage::MAX_DATA_SIZE];
	do
	{
		int size = leftSize > FilePackage::MAX_DATA_SIZE ? FilePackage::MAX_DATA_SIZE : leftSize;

		fread(buffer, 1, size, fstream);

		FilePackage filePackage;
		filePackage.setLeftSize(leftSize);
		filePackage.setSize(size);
		filePackage.setDataPtr(buffer);

		if (connection.send(&filePackage) != 0)
			break;

		leftSize -= size;

	} while (leftSize > 0);
	delete[] buffer;

	fclose(fstream);
	fileManager->closeLocalFile(descriptor);
}

void Protocols::receiveFile(files::Descriptor& descriptor, conn::TCPConnection& connection)
{
	std::string fileName = "files/" + std::to_string(descriptor.date) + "-" + std::to_string(descriptor.owner) + "-" + descriptor.name;

	FILE* file = fopen(fileName.c_str(), "wb");

	if (file == nullptr)
		return;

	bool finished = false;

	do
	{
		char ID[4];
		if (connection.recv(ID, 4))
			break;

		if (memcmp(ID, AbortPackage::ID, 4) == 0)
			break;

		FilePackage filePackage;
		if (connection.recvNoId(&filePackage) != 0)
			break;

		if (filePackage.getSize() > 0)
			fwrite(filePackage.getDataPtr(), 1, filePackage.getSize(), file);

		if (filePackage.getSize() == filePackage.getLeftSize())
			finished = true;
	} while(true);

	fclose(file);

	if (!finished)
		remove(fileName.c_str());

	else
		fileManager->addDiskFile(descriptor, fileName);
}


void Protocols::broadcastFilesList()
{
	auto localFiles = fileManager->listLocalFiles();
	long long int time = getTime();
	int sent = 0;
	do {
		proto::ListPackage listPackage;

		std::vector<files::Descriptor> list;

		while (localFiles.size() > sent && list.size() < ListPackage::MAX_FILES_N)
			list.push_back(localFiles[sent++]);

		listPackage.setTime(time);
		listPackage.setFiles(list);

		broadcaster->send(&listPackage);

	} while (localFiles.size() > sent);
}

void* Protocols::_redistribute(void*)
{
	sleep(5);

	pthread_mutex_lock(&getInstance().greetedNodesMutex);
	pthread_mutex_lock(&getInstance().nodesMutex);

	getInstance().isRedistributing = false;

	getInstance().nodes.clear();

	for (auto node : getInstance().greetedNodes)
		getInstance().nodes[node] = getTime();

	getInstance().greetedNodes.clear();

	pthread_mutex_unlock(&getInstance().greetedNodesMutex);
	pthread_mutex_unlock(&getInstance().nodesMutex);

	auto localFiles = getInstance().fileManager->listLocalFiles();

	for (auto& file : localFiles)
		getInstance().uploadFile(file);

	getInstance().broadcastFilesList();

	return nullptr;
}

void Protocols::redistribute(bool separateThread, bool disconnecting)
{
	bool wasRedistributing = isRedistributing;

	if (!disconnecting)
	{
		HelloPackage helloPackage;
		broadcaster->send(&helloPackage);
	}

	pthread_mutex_lock(&greetedNodesMutex);

	isRedistributing = true;

	if (!disconnecting)
		greetedNodes.insert(conn::IPv4Address::getLocalAddress(0).sin_addr.s_addr);

	pthread_mutex_unlock(&greetedNodesMutex);

	if (!wasRedistributing && separateThread)
	{
		pthread_t thread;
		pthread_create(&thread, nullptr, _redistribute, nullptr);
		pthread_detach(thread);
	}
	else if (!separateThread)
	{
		_redistribute(nullptr);
	}
}

void* Protocols::_keepAliveSender(void *)
{
	KeepAlivePackage package;
	while (true) sleep(10000);
	while (true)
	{
		sleep(30);
		if (!getInstance().isRedistributing)
			getInstance().broadcaster->send(&package);
	}
}

void* Protocols::_keepAliveMonitor(void *)
{
	int localAddr = conn::IPv4Address::getLocalAddress(0).sin_addr.s_addr;
	while (true) sleep(10000);
	while (true)
	{
		if (getInstance().isRedistributing)
			continue;

		int timeToSleep = 300; //sec;

		pthread_mutex_lock(&getInstance().nodesMutex);

		bool foundBody = false;

		for (auto node : getInstance().nodes)
		{
			if (node.first == localAddr)
				continue;

			int leftTime = (node.second - getTime()) / 1000;

			if (leftTime <= 0)
			{
				timeToSleep = 300;
				DeadbodyPackage deadbodyPackage;
				deadbodyPackage.setAddress(node.first);

				pthread_mutex_unlock(&getInstance().nodesMutex);
				getInstance().broadcaster->send(&deadbodyPackage);

				getInstance().redistribute(true, false);
				foundBody = true;
				break;
			}

			if (timeToSleep > leftTime)
				timeToSleep = leftTime;
		}

		if (!foundBody)
			pthread_mutex_unlock(&getInstance().nodesMutex);

		sleep(timeToSleep);
	}
}