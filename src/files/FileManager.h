#ifndef P2PFILESEXCHANGE_FILEMANAGER_H
#define P2PFILESEXCHANGE_FILEMANAGER_H

#include <string>
#include <vector>
#include <conn/IPv4Address.h>

namespace files
{
	// WIP concept
	class FileManager
	{
	public:
		typedef std::string FileId;
		typedef void* Descriptor; // 64 byte descriptor for network communication
		typedef std::vector<FileId> FileList;

		FileManager();

		int addLocalFile(const std::string& localPath);
		int addRemoteFile(Descriptor descriptor);
		int setFileAsLocal(FileId file, const std::string& localPath);
		int deleteLocalFile(FileId file);
		int deactivateLocalFile(FileId file);

		Descriptor getDescriptor(FileId file) const;
		const conn::IPv4Address& getOwner(FileId file) const;
		const conn::IPv4Address& getNode(FileId file) const;

		const FileList& listAllFiles() const;
		const FileList& listLocalFiles() const;
	private:
		struct File
		{
			conn::IPv4Address node; // who stores it
			conn::IPv4Address owner;
			std::string name;
		};

		// extends File adding local-only fields
		struct LocalFile : public File
		{
			int state;
			bool active;
			std::string path;
			size_t threadCount;
		};

		std::vector<File> remoteFiles;
		std::vector<LocalFile> localFiles;
	};
}

#endif //P2PFILESEXCHANGE_FILEMANAGER_H
