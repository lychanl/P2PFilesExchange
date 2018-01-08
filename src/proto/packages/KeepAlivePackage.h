
#ifndef P2PFILESEXCHANGE_KEEPALIVEPACKAGE_H
#define P2PFILESEXCHANGE_KEEPALIVEPACKAGE_H

#include "proto/Package.h"

namespace proto
{
	class KeepAlivePackage : public Package
	{
	public:
		constexpr static char ID[] = "KALV";

		KeepAlivePackage();

		int getSerializedSize();

	protected:
		int parseHeader();

		void parseData();

		void serialize();
	};
}


#endif //P2PFILESEXCHANGE_KEEPALIVEPACKAGE_H
