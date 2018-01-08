
#include <cstring>
#include "GetPackage.h"


using namespace proto;

constexpr char GetPackage::ID[];

GetPackage::GetPackage()
		: Package(72)
{

}

int GetPackage::getSerializedSize()
{
	return 72;
}

files::Descriptor& GetPackage::getDescriptor()
{
	return this->descriptor;
}

void GetPackage::setDescriptor(const files::Descriptor &descriptor)
{
	this->descriptor = descriptor;
}

int GetPackage::parseHeader()
{
	this->descriptor.owner = getUInt(4);
	this->descriptor.date = (unsigned long long)getUInt(8) << 32 | getUInt(12);
	strncpy(this->descriptor.name, (char*)getData(16), 52);
	return 0;
}

void GetPackage::parseData()
{
}

void GetPackage::serialize()
{
	put((void*)ID, sizeof(ID), 0);
	put((unsigned)descriptor.owner, 4);
	put((unsigned)(this->descriptor.date >> 32), 8);
	put((unsigned)(this->descriptor.date), 12);
	put((void*)this->descriptor.name, 52, 16);
}