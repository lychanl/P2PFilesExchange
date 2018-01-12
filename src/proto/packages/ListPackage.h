
#ifndef P2PFILESEXCHANGE_LIST_H
#define P2PFILESEXCHANGE_LIST_H

#include <vector>
#include <files/File.h>
#include "proto/Package.h"

namespace proto
{
	class ListPackage : public Package
	{
	public:
		constexpr static char ID[] = "LIST";

		ListPackage();

		int getSerializedSize();

		unsigned long long int getTime();
		void setTime(unsigned long long int time);
		const std::vector<files::Descriptor>& getFiles();
		void setFiles(const std::vector<files::Descriptor>& files);
	protected:
		int parseHeader();

		void parseData();

		void serialize();
	private:

		unsigned long long int time;
		std::vector<files::Descriptor> files;

		int n;
	};
}


#endif //P2PFILESEXCHANGE_LIST_H
