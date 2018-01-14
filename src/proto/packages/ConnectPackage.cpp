
#include "ConnectPackage.h"

using namespace proto;

constexpr char ConnectPackage::ID[];

ConnectPackage::ConnectPackage()
		: Package(4)
{
}

int ConnectPackage::getSerializedSize()
{
	return 4;
}

int ConnectPackage::parseHeader()
{
	return 0;
}

void ConnectPackage::parseData()
{

}

void ConnectPackage::serialize()
{
	put ((void*)ID, 4, 0);
}