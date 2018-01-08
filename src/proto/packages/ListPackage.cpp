
#include <cstring>
#include "ListPackage.h"

using namespace proto;

constexpr char ListPackage::ID[];

ListPackage::ListPackage()
		: Package(20)
{
}

int ListPackage::getSerializedSize()
{
	return 20 + files.size() * 64;
}

int ListPackage::parseHeader()
{
	this->time = (unsigned long long)getUInt(8) << 32 | getUInt(12);
	this->n = getInt(16);
	return n * 64;
}

void ListPackage::parseData()
{
	for (int i = 0; i < n; i++)
	{
		int offset = 20 + 64 * i;
		files::Descriptor descriptor;

		descriptor.owner = getUInt(offset);
		descriptor.date = (unsigned long long)getUInt(offset + 4) << 32 | getUInt(offset + 8);
		strncpy(descriptor.name, (char*)getData(offset + 12), 52);

		this->files.push_back(descriptor);
	}
}

void ListPackage::serialize()
{
	put ((void*)ID, 4, 0);
	put((unsigned)(time >> 32), 8);
	put((unsigned)time, 12);
	put((unsigned)this->files.size(), 16);

	for (int i = 0; i < this->files.size(); i++)
	{
		int offset = 20 + 64 * i;

		put((unsigned)this->files[i].owner, offset);
		put((unsigned)(this->files[i].date >> 32), offset + 4);
		put((unsigned)(this->files[i].date), offset + 8);
		put((void*)this->files[i].name, 52, offset + 12);
	}
}

unsigned long long int ListPackage::getTime()
{
	return this->time;
}

void ListPackage::setTime(unsigned long long int time)
{
	this->time = time;
}

const std::vector<files::Descriptor>& ListPackage::getFiles()
{
	return this->files;
}

void ListPackage::setFiles(const std::vector<files::Descriptor>& files)
{
	this->files = files;
}