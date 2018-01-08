
#include "OkPackage.h"

using namespace proto;


constexpr char OkPackage::ID[];

OkPackage::OkPackage()
		: Package(4)
{
}

int OkPackage::getSerializedSize()
{
	return 4;
}

int OkPackage::parseHeader()
{
	return 0;
}

void OkPackage::parseData()
{

}

void OkPackage::serialize()
{
	put ((void*)ID, sizeof(ID), 0);
}