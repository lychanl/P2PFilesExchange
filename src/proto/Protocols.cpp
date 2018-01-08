
#include "Protocols.h"

using namespace proto;

Protocols* Protocols::instance = nullptr;

void Protocols::init(conn::UDPBroadcaster *broadcaster, files::FileManager *fileManager)
{
	instance = new Protocols();
	instance->broadcaster = broadcaster;
	instance->fileManager = fileManager;

	pthread_mutex_init(&instance->redistributingInitMutex, nullptr);
}

Protocols& Protocols::getInstance()
{
	return *instance;
}

Protocols::Result Protocols::connect()
{ //TODO
	return Result::OK;
}

void Protocols::disconnect()
{} //TODO

void Protocols::missingNode()
{} //TODO

Protocols::Result Protocols::getFile(const files::Descriptor &file, int fd)
{ //TODO
	return Result ::OK;
}

void Protocols::uploadFile(const files::Descriptor &file)
{} //TODO

Protocols::Result Protocols::deactivateFile(const files::Descriptor &file)
{ //TODO
	return Result::OK;
}

void Protocols::udpHandler(void *buffer, int recvData, const conn::IPv4Address &sender)
{} //TODO

void Protocols::tcpHandler(conn::TCPConnection &conn)
{} //TODO
