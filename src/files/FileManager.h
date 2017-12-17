#ifndef P2PFILESEXCHANGE_FILEMANAGER_H
#define P2PFILESEXCHANGE_FILEMANAGER_H

#include <pthread.h>
#include <string>
#include <vector>
#include <list>
#include <conn/IPv4Address.h>
#include <unordered_map>

namespace files
{
	// WIP concept
	class FileManager
	{
	public:

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

		typedef Descriptor FileId;
		typedef std::vector<FileId> FileList;

		explicit FileManager(conn::IPv4Address localNode);

		int addDiskFile(const std::string &diskPath); // creates descriptor
		int addDiskFile(Descriptor desc, const std::string &diskPath);

		int addRemoteFile(Descriptor descriptor, conn::IPv4Address node);

		int deactivateLocalFile(FileId file);

		int openLocalFile(FileId file); // for reading
		int closeLocalFile(FileId file);

		const conn::IPv4Address getNode(FileId file);

		const FileList listAllFiles();

		const FileList listLocalFiles();

	private:
		struct File : public Descriptor
		{
			explicit File(conn::IPv4Address node);

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

			int state;
			bool active;
			std::string path;
			size_t threadCount;
			//pthread_rwlock_t lock; // this is probably useless rn
			pthread_mutex_t mutex;
		};

		conn::IPv4Address localNode;
		pthread_rwlock_t fileListLock;
		std::unordered_map<FileId, File, DescriptorHasher> remoteFiles;
		std::unordered_map<FileId, LocalFile, DescriptorHasher> localFiles;
		std::string fileDir;

		int copyFile(std::string in, std::string out);

		int deleteLocalFile(LocalFile &file);

		File &findFile(FileId file);

		LocalFile &findLocalFile(FileId file);

		const File &findFile(FileId file) const;

		const LocalFile &findLocalFile(FileId file) const;

	};
}

#endif //P2PFILESEXCHANGE_FILEMANAGER_H
