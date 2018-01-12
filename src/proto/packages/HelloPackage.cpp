
#include "HelloPackage.h"

using namespace proto;

constexpr char HelloPackage::ID[];

HelloPackage::HelloPackage()
		: Package(4)
{
}

int HelloPackage::getSerializedSize()
{
	return 4;
}

int HelloPackage::parseHeader()
{
	return 0;
}

void HelloPackage::parseData()
{

}

void HelloPackage::serialize()
{
	put ((void*)ID, sizeof(ID), 0);
}