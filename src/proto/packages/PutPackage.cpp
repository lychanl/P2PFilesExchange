#include <cstring>
#include "PutPackage.h"


using namespace proto;

constexpr char PutPackage::ID[];

PutPackage::PutPackage()
		: Package(72)
{

}

int PutPackage::getSerializedSize()
{
	return 8;
}

files::Descriptor& PutPackage::getDescriptor()
{
	return this->descriptor;
}

void PutPackage::setDescriptor(const files::Descriptor &descriptor)
{
	this->descriptor = descriptor;
}

int PutPackage::parseHeader()
{
	this->descriptor.owner = getUInt(4);
	this->descriptor.date = (unsigned long long)getUInt(8) << 32 | getUInt(12);
	strncpy(this->descriptor.name, (char*)getData(16), 52);
	return 0;
}

void PutPackage::parseData()
{
}

void PutPackage::serialize()
{
	put((void*)ID, sizeof(ID), 0);
	put((unsigned)descriptor.owner, 4);
	put((unsigned)(this->descriptor.date >> 32), 8);
	put((unsigned)(this->descriptor.date), 12);
	put((void*)this->descriptor.name, 52, 16);
}