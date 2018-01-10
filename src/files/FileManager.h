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

		explicit FileManager(conn::IPv4Address localNode, std::string fileDir);
		~FileManager();

		int addDiskFile(const std::string &diskPath); // creates descriptor
		int addDiskFile(Descriptor desc, const std::string &diskPath);

		// if the entry in listDates for this node has an older date, then all descriptors of this node will be erased
		int addRemoteFile(Descriptor descriptor, conn::IPv4Address node, unsigned long long int date);
		int addRemoteFiles(std::vector<Descriptor> descriptors, conn::IPv4Address node, unsigned long long int date);

		int removeRemoteFile(Descriptor descriptor);
		int removeRemoteFilesFromNode(conn::IPv4Address node);

		int deactivateLocalFile(Descriptor file);

		int openLocalFile(Descriptor file); // for reading
		int closeLocalFile(Descriptor file);

		const conn::IPv4Address getNode(Descriptor file);

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
