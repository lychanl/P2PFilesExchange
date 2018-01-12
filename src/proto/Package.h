
#ifndef P2PFILESEXCHANGE_PACKAGE_H
#define P2PFILESEXCHANGE_PACKAGE_H

#include "conn/TCPConnection.h"
#include <cstdint>

namespace proto
{
	class Package
	{
	public:
		explicit Package(int headerSize);

		int parseHeader(void* data);
		void parseData(void* data);

		void serializeTo(void* data);

		virtual int getSerializedSize() = 0;

		int getHeaderSize();
	protected:
		//must return size of the rest of the data to parse
		virtual int parseHeader() = 0;
		virtual void parseData() = 0;

		virtual void serialize() = 0;


		int getInt(int offset);
		unsigned int getUInt(int offset);
		char getChar(int offset);
		void* getData(int offset);

		void put(int value, int offset);
		void put(unsigned int value, int offset);
		void put(char value, int offset);
		void put(void* data, int size, int offset);
	private:
		int headerSize;
		int dataOffset;
		void* data;
	};
}


#endif //P2PFILESEXCHANGE_PACKAGE_H
