//
// Created by jlyskawa on 29.11.17.
//

#include "exception.h"

using namespace std;

Exception::Exception(const string& what) : whatStr(what)
{
}

const char* Exception::what() const noexcept
{
	return whatStr.c_str();
}