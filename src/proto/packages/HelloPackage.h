
#ifndef P2PFILESEXCHANGE_HELLOPACKAGE_H
#define P2PFILESEXCHANGE_HELLOPACKAGE_H

#include "proto/Package.h"

namespace proto
{
	class HelloPackage : public Package
	{
	public:
		constexpr static char ID[] = "HELL";

		HelloPackage();

		int getSerializedSize();

	protected:
		int parseHeader();

		void parseData();

		void serialize();
	};
}


#endif //P2PFILESEXCHANGE_HELLOPACKAGE_H
