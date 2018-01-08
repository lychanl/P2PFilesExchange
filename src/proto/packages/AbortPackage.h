
#ifndef P2PFILESEXCHANGE_ABORTPACKAGE_H
#define P2PFILESEXCHANGE_ABORTPACKAGE_H

#include "proto/Package.h"

namespace proto
{
	class AbortPackage : public Package
	{
	public:
		constexpr static char ID[] = "ABRT";

		AbortPackage();

		int getSerializedSize();

	protected:
		int parseHeader();
		void parseData();

		void serialize();
	};
}


#endif //P2PFILESEXCHANGE_ABORTPACKAGE_H
