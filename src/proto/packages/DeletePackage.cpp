
#include <cstring>
#include "DeletePackage.h"


using namespace proto;

constexpr char DeletePackage::ID[];

DeletePackage::DeletePackage()
		: Package(72)
{

}

int DeletePackage::getSerializedSize()
{
	return 72;
}

files::Descriptor& DeletePackage::getDescriptor()
{
	return this->descriptor;
}

void DeletePackage::setDescriptor(const files::Descriptor &descriptor)
{
	this->descriptor = descriptor;
}

int DeletePackage::parseHeader()
{
	this->descriptor.owner = getUInt(4);
	this->descriptor.date = (unsigned long long)getUInt(8) << 32 | getUInt(12);
	strncpy(this->descriptor.name, (char*)getData(16), 52);
	return 0;
}

void DeletePackage::parseData()
{
}

void DeletePackage::serialize()
{
	put((void*)ID, 4, 0);
	put((unsigned)descriptor.owner, 4);
	put((unsigned)(this->descriptor.date >> 32), 8);
	put((unsigned)(this->descriptor.date), 12);
	put((void*)this->descriptor.name, 52, 16);
}