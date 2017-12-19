#ifndef P2PFILESEXCHANGE_FILEMANAGER_H
#define P2PFILESEXCHANGE_FILEMANAGER_H

#include <pthread.h>
#include <string>
#include <conn/IPv4Address.h>
#include <vector>

#include "FileList.h"

namespace files
{
	class FileManager
	{
	public:

		explicit FileManager(conn::IPv4Address localNode, std::string fileDir);

		int addDiskFile(const std::string &diskPath); // creates descriptor
		int addDiskFile(Descriptor desc, const std::string &diskPath);

		int addRemoteFile(Descriptor descriptor, conn::IPv4Address node);

		int removeRemoteFile(Descriptor descriptor);

		int deactivateLocalFile(Descriptor file);

		int openLocalFile(Descriptor file); // for reading
		int closeLocalFile(Descriptor file);

		const conn::IPv4Address getNode(Descriptor file);

		const std::vector<Descriptor> listAllFiles();

		const std::vector<Descriptor> listLocalFiles();

	private:
		FileList fileList;
		conn::IPv4Address localNode;
		pthread_rwlock_t fileListLock;
	};
}

#endif //P2PFILESEXCHANGE_FILEMANAGER_H
