#include <cstring>
#include "File.h"

using namespace files;

bool Descriptor::operator==(const Descriptor &other) const
{
	return (owner == other.owner) && (date == other.date) && (strncmp(name, other.name, 52) == 0);
}

File::File(const conn::IPv4Address& node) : node(node)
{

}

bool File::operator==(const File &other) const
{
	return Descriptor::operator==(other);
}

File &File::operator=(const Descriptor &desc)
{
	owner = desc.owner;
	date = desc.date;
	strncpy(name, desc.name, 52);
	return *this;
}

File::File() : node(0)
{

}

LocalFile::LocalFile(const conn::IPv4Address& node) : File(node)
{
	pthread_mutex_init(&mutex, nullptr);
}

LocalFile &LocalFile::operator=(const Descriptor &desc)
{
	owner = desc.owner;
	date = desc.date;
	strncpy(name, desc.name, 52);
	return *this;
}

LocalFile::~LocalFile()
{
	pthread_mutex_destroy(&mutex);
}

LocalFile::LocalFile()
{
	pthread_mutex_init(&mutex, nullptr);
}
