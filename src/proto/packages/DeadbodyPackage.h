
#ifndef P2PFILESEXCHANGE_DEADBODYPACKAGE_H
#define P2PFILESEXCHANGE_DEADBODYPACKAGE_H

#include "conn/IPv4Address.h"
#include "proto/Package.h"

namespace proto
{
	class DeadbodyPackage : public Package
	{
	public:
		constexpr static char ID[] = "BODY";

		DeadbodyPackage();

		int getSerializedSize();

		const conn::IPv4Address& getAddress();
		void setAddress(const conn::IPv4Address& address);
	protected:
		int parseHeader();

		void parseData();

		void serialize();

	private:
		conn::IPv4Address address;
	};
}


#endif //P2PFILESEXCHANGE_DEADBODYPACKAGE_H
