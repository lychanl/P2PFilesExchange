
#include <cstring>
#include "Package.h"

using namespace proto;

Package::Package(int headerSize)
{
	this->headerSize = headerSize;
	this->data = nullptr;
}

int Package::parseHeader(void *data)
{
	this->data = data;
	this->dataOffset = 0;

	int ret = this->parseHeader();

	this->data = nullptr;
	return ret;
}

void Package::parseData(void *data)
{
	this->data = data;
	this->dataOffset = headerSize;


	this->parseData();

	this->data = nullptr;
}

void Package::serializeTo(void *data)
{
	this->data = data;
	serialize();
}

int Package::getInt(int offset)
{
	unsigned u = getUInt(offset);
	u = getUInt(offset);

	int sign = u & (1 << 31);
	int value = u & ((1 << 31) - 1);

	return sign ? -value : value;
}

unsigned int Package::getUInt(int offset)
{
	return ((unsigned)getChar(offset) << 24) +
			((unsigned)getChar(offset + 1) << 16) +
			((unsigned)getChar(offset + 2) << 8) +
			(unsigned)getChar(offset + 3);
}

char Package::getChar(int offset)
{
	return ((char*)this->data)[offset - dataOffset];
}

void* Package::getData(int offset)
{
	return this->data + offset - dataOffset;
}

void Package::put(int value, int offset)
{
	if (value > 0)
		put((unsigned)value, offset);
	else
		put((unsigned)value + (unsigned)(1 << 31), offset);
}

void Package::put(unsigned int value, int offset)
{
	put((char)(value >> 24), offset);
	put((char)((value >> 16) & 255), offset + 1);
	put((char)((value >> 8) & 255), offset + 2);
	put((char)(value & 255), offset + 3);
}

void Package::put(char value, int offset)
{
	((char*)data)[offset] = value;
}

void Package::put(void *data, int size, int offset)
{
	memcpy(this->data + offset, data, size);
}