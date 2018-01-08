
#include <cstring>
#include "FilePackage.h"

using namespace proto;

constexpr char FilePackage::ID[];

FilePackage::FilePackage()
		: Package(16)
{

}

int FilePackage::getSerializedSize()
{
	return 16 + size;
}

int FilePackage::parseHeader()
{
	this->size = getUInt(4);
	this->leftSize = (unsigned long long)getUInt(8) << 32 | getUInt(12);
	return this->size;
}

void FilePackage::parseData()
{
	this->ownData = true;
	this->data = new char[this->size];
	strncpy((char*)this->data, (char*)getData(16), this->size);
}

void FilePackage::serialize()
{
	put((void*)ID, sizeof(ID), 0);
	put(this->size, 4);
	put((unsigned)(this->leftSize >> 32), 8);
	put((unsigned)(this->leftSize), 12);
	put(this->data, this->size, 16);
}

FilePackage::~FilePackage()
{
	if (this->ownData)
		delete[] (char*)this->data;
}

void FilePackage::setDataPtr(void* data)
{
	if (this->ownData)
		delete[] (char*)this->data;
	this->ownData = false;
	this->data = data;
}

void* FilePackage::getDataPtr()
{
	return this->data;
}

void FilePackage::setSize(unsigned size)
{
	this->size = size;
}

unsigned FilePackage::getSize()
{
	return this->size;
}

void FilePackage::setLeftSize(unsigned long long leftSize)
{
	this->leftSize = leftSize;
}

unsigned long long FilePackage::getLeftSize()
{
	return this->leftSize;
}