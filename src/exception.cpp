#include "exception.h"

using namespace std;

Exception::Exception(const string& what) : whatStr(what)
{
}

const char* Exception::what() const noexcept
{
	return whatStr.c_str();
}