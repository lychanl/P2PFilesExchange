
#include "KeepAlivePackage.h"

using namespace proto;

constexpr char KeepAlivePackage::ID[];

KeepAlivePackage::KeepAlivePackage()
		: Package(4)
{
}

int KeepAlivePackage::getSerializedSize()
{
	return 4;
}

int KeepAlivePackage::parseHeader()
{
	return 0;
}

void KeepAlivePackage::parseData()
{

}

void KeepAlivePackage::serialize()
{
	put ((void*)ID, 4, 0);
}