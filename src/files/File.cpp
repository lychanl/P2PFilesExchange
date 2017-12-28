#include <cstring>
#include "File.h"

using namespace files;

bool Descriptor::operator==(const Descriptor &other) const
{
	return (owner == other.owner) && (date == other.date) && (strncmp(name, other.name, 52) == 0);
}

File::File(conn::IPv4Address node) : node(node)
{

}

bool File::operator==(const File &other) const
{
	return Descriptor::operator==(other);
}

File &File::operator=(Descriptor &desc)
{
	owner = desc.owner;
	date = desc.date;
	strncpy(name, desc.name, 52);
	return *this;
}

File::File() : node(0)
{

}

LocalFile::LocalFile(conn::IPv4Address node) : File(node)
{

}

LocalFile &LocalFile::operator=(Descriptor &desc)
{
	owner = desc.owner;
	date = desc.date;
	strncpy(name, desc.name, 52);
	return *this;
}