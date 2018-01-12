
#ifndef P2PFILESEXCHANGE_NOFILEPACKAGE_H
#define P2PFILESEXCHANGE_NOFILEPACKAGE_H

#include "proto/Package.h"

namespace proto
{
	class NoFilePackage : public Package
	{
	public:
		enum Reason
		{
			NONE = 'NONE',
			AWAY = 'AWAY',
			DELETED = 'DLTD',
		};

		constexpr static char ID[] = "NOFL";

		NoFilePackage();

		int getSerializedSize();

		void setReason(Reason reason);
		Reason getReason();

		void setSentToAddress(const conn::IPv4Address& sentTo);
		const conn::IPv4Address& getSentToAddress();


	protected:
		int parseHeader();

		void parseData();

		void serialize();

	private:
		Reason reason = Reason::NONE;
		conn::IPv4Address sentTo;
	};
}


#endif //P2PFILESEXCHANGE_NOFILEPACKAGE_H
