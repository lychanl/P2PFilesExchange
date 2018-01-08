
#ifndef P2PFILESEXCHANGE_FILEPACKAGE_H
#define P2PFILESEXCHANGE_FILEPACKAGE_H


#include <files/File.h>
#include <proto/Package.h>


namespace proto
{
	class FilePackage : public Package
	{
	public:
		constexpr static char ID[] = "FILE";

		FilePackage();
		~FilePackage();

		int getSerializedSize();

		void setDataPtr(void*);
		void* getDataPtr();
		void setSize(unsigned size);
		unsigned getSize();
		void setLeftSize(unsigned long long leftSize);
		unsigned long long getLeftSize();
	protected:
		int parseHeader();

		void parseData();

		void serialize();

	private:
		unsigned size;
		unsigned long long leftSize;
		void* data = nullptr;
		bool ownData = false;
	};
}

#endif //P2PFILESEXCHANGE_FILEPACKAGE_H
