
#ifndef P2PFILESEXCHANGE_OKPACKAGE_H
#define P2PFILESEXCHANGE_OKPACKAGE_H

#include "proto/Package.h"

namespace proto
{
	class OkPackage : public Package
	{
	public:
		constexpr static char ID[] = "OKOK";

		OkPackage();

		int getSerializedSize();

	protected:
		int parseHeader();

		void parseData();

		void serialize();
	};
}


#endif //P2PFILESEXCHANGE_OKPACKAGE_H
