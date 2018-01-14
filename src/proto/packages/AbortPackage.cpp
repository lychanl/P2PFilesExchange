
#include "AbortPackage.h"

using namespace proto;

constexpr char AbortPackage::ID[];

AbortPackage::AbortPackage()
	: Package(4)
{
}

int AbortPackage::getSerializedSize()
{
	return 4;
}

int AbortPackage::parseHeader()
{
	return 0;
}

void AbortPackage::parseData()
{

}

void AbortPackage::serialize()
{
	put ((void*)ID, 4, 0);
}