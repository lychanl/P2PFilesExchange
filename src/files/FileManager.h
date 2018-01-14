#ifndef P2PFILESEXCHANGE_FILEMANAGER_H
#define P2PFILESEXCHANGE_FILEMANAGER_H

#include <pthread.h>
#include <string>
#include <conn/IPv4Address.h>
#include <vector>
#include <unordered_map>

#include "FileList.h"

namespace files
{
	class FileManager
	{
	public:

		explicit FileManager(conn::IPv4Address localNode, const std::string& fileDir);
		~FileManager();
		// everything that returns int except openLocalFile will return 0 on success
		Descriptor addDiskFile(const std::string &diskPath); // creates descriptor
		int addDiskFile(const Descriptor& desc, const std::string &diskPath);

		// if the entry in listDates for this node has an older date, then all descriptors of this node will be erased
		int addRemoteFile(const Descriptor& descriptor, const conn::IPv4Address& node, unsigned long long int date);
		int addRemoteFiles(const std::vector<Descriptor>& descriptors, const conn::IPv4Address& node, unsigned long long int date);

		int removeRemoteFile(const Descriptor& descriptor);
		int removeRemoteFilesFromNode(const conn::IPv4Address& node);

		int deactivateLocalFile(const Descriptor& file);
		int makeLocalFileRemote(const Descriptor& file, const conn::IPv4Address& newNode);
		bool isActive(const Descriptor& localFile);

		int openLocalFile(const Descriptor& file); // for reading, returns -1 on failure
		int closeLocalFile(const Descriptor& file);

		const conn::IPv4Address getNode(const Descriptor& file);

		const std::vector<Descriptor> listAllFiles();

		const std::vector<Descriptor> listLocalFiles();

	private:

		std::unordered_map<unsigned long int, unsigned long long int> listDates; // keeps <node, date> pairs to tell
																				// if a descriptor indicates start
																				// of a new list or not
		FileList fileList;
		conn::IPv4Address localNode;
		pthread_rwlock_t fileListLock;
	};
}

#endif //P2PFILESEXCHANGE_FILEMANAGER_H
