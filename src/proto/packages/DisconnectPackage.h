
#ifndef P2PFILESEXCHANGE_DISCONNECTPACKAGE_H
#define P2PFILESEXCHANGE_DISCONNECTPACKAGE_H

#include "proto/Package.h"

namespace proto
{
	class DisconnectPackage : public Package
	{
	public:
		constexpr static char ID[] = "DISC";

		DisconnectPackage();

		int getSerializedSize();

	protected:
		int parseHeader();

		void parseData();

		void serialize();
	};
}


#endif //P2PFILESEXCHANGE_DISCONNECTPACKAGE_H
