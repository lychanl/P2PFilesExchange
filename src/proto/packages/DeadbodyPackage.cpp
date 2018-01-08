
#include "DeadbodyPackage.h"

using namespace proto;

constexpr char DeadbodyPackage::ID[];

DeadbodyPackage::DeadbodyPackage()
	: Package(8), address(0)
{

}

int DeadbodyPackage::getSerializedSize()
{
	return 8;
}

const conn::IPv4Address& DeadbodyPackage::getAddress()
{
	return this->address;
}

void DeadbodyPackage::setAddress(const conn::IPv4Address& address)
{
	this->address = address;
}

int DeadbodyPackage::parseHeader()
{
	this->address = conn::IPv4Address(getUInt(4));

	return 0;
}

void DeadbodyPackage::parseData()
{
}

void DeadbodyPackage::serialize()
{
	put ((void*)ID, sizeof(ID), 0);
	put((unsigned)this->address.getAddress(), 4);
}