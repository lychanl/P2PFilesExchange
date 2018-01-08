
#ifndef P2PFILESEXCHANGE_CONNECTPACKAGE_H
#define P2PFILESEXCHANGE_CONNECTPACKAGE_H

#include "proto/Package.h"

namespace proto
{
	class ConnectPackage : public Package
	{
	public:
		constexpr static char ID[] = "CONN";

		ConnectPackage();

		int getSerializedSize();

	protected:
		int parseHeader();

		void parseData();

		void serialize();
	};
}


#endif //P2PFILESEXCHANGE_CONNECTPACKAGE_H
