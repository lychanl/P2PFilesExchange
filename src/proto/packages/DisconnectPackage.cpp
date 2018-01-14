
#include "DisconnectPackage.h"

using namespace proto;

constexpr char DisconnectPackage::ID[];

DisconnectPackage::DisconnectPackage()
		: Package(4)
{
}

int DisconnectPackage::getSerializedSize()
{
	return 4;
}

int DisconnectPackage::parseHeader()
{
	return 0;
}

void DisconnectPackage::parseData()
{

}

void DisconnectPackage::serialize()
{
	put ((void*)ID, 4, 0);
}