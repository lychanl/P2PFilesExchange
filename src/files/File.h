#ifndef P2PFILESEXCHANGE_FILE_H
#define P2PFILESEXCHANGE_FILE_H

#include <conn/IPv4Address.h>

namespace files
{
	struct Descriptor
	{
		unsigned long int owner;
		unsigned long long int date;
		char name[52];

		bool operator==(const Descriptor &other) const;

		bool operator!=(const Descriptor &other) const
		{
			return !operator==(other);
		}
	};

	struct DescriptorHasher
	{
		std::size_t operator()(const Descriptor &d) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			return ((hash<unsigned long int>()(d.owner) ^ (hash<unsigned long long int>()(d.date) << 1)) >> 1) ^
				   (hash<string>()(d.name) << 1);
		}
	};

	struct File : public Descriptor
	{
		explicit File(conn::IPv4Address node);

		File &operator=(Descriptor &desc);

		conn::IPv4Address node; // who stores it
		bool operator==(const File &other) const;

		bool operator!=(const File &other) const
		{
			return !operator==(other);
		}
	};

	// extends File adding local-only fields
	struct LocalFile : public File
	{
		explicit LocalFile(conn::IPv4Address node);

		LocalFile &operator=(Descriptor &desc);

		int state;
		bool active;
		std::string path;
		size_t threadCount;
		pthread_mutex_t mutex;
	};
}


#endif //P2PFILESEXCHANGE_FILE_H
