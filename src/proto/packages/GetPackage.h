
#ifndef P2PFILESEXCHANGE_GETPACKAGE_H
#define P2PFILESEXCHANGE_GETPACKAGE_H


#include <files/File.h>
#include <proto/Package.h>


namespace proto
{
	class GetPackage : public Package
	{
	public:
		constexpr static char ID[] = "GET ";

		GetPackage();

		int getSerializedSize();

		void setDescriptor(const files::Descriptor& descriptor);
		files::Descriptor& getDescriptor();
	protected:
		int parseHeader();

		void parseData();

		void serialize();
	private:
		files::Descriptor descriptor;
	};
}

#endif //P2PFILESEXCHANGE_GETPACKAGE_H
