
#include "NoFilePackage.h"

using namespace proto;

constexpr char NoFilePackage::ID[];

NoFilePackage::NoFilePackage()
	: Package(8), sentTo(0)
{

}

int NoFilePackage::getSerializedSize()
{
	return reason == Reason::AWAY ? 12 : 8;
}

void NoFilePackage::setReason(Reason reason)
{
	this->reason = reason;
}

NoFilePackage::Reason NoFilePackage::getReason()
{
	return this->reason;
}

void NoFilePackage::setSentToAddress(const conn::IPv4Address& sentTo)
{
	this->sentTo = sentTo;
}

const conn::IPv4Address& NoFilePackage::getSentToAddress()
{
	return this->sentTo;
}

int NoFilePackage::parseHeader()
{
	this->reason = (Reason)getUInt(4);

	return reason == Reason::AWAY ? 4 : 0;
}

void NoFilePackage::parseData()
{
	if (this->reason == Reason::AWAY)
		this->sentTo = getUInt(8);
}

void NoFilePackage::serialize()
{
	put((void*)ID, 4, 0);
	put(this->reason, 4);
	if(this->reason == Reason::AWAY)
		put((unsigned)this->sentTo.getAddress(), 8);
}