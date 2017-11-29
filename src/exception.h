#ifndef P2PFILESEXCHANGE_EXCEPTION_H
#define P2PFILESEXCHANGE_EXCEPTION_H

#include <exception>
#include <string>

class Exception : public std::exception
{
public:
	Exception(const std::string& what);
	const char* what() const noexcept;
private:
	const std::string whatStr;
};

#endif //P2PFILESEXCHANGE_EXCEPTION_H
