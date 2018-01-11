#include <cstring>
#include <fcntl.h>
#include "FileManager.h"

using namespace files;

files::FileManager::FileManager(conn::IPv4Address localNode, std::string fileDir) : localNode(localNode),
																					fileList(fileDir)
{
	pthread_rwlock_init(&fileListLock, nullptr);
}


FileManager::~FileManager()
{
	pthread_rwlock_destroy(&fileListLock);
}

int FileManager::addDiskFile(const std::string &diskPath)
{
	LocalFile f(localNode);
	std::string path = fileList.copyToFileDir(diskPath);
	const char *baseName = basename(path.c_str());

	// add file to list
	f.owner = localNode.getAddress();
	f.date = static_cast<unsigned long long int>(time(nullptr));
	strncpy(f.name, baseName, 52);
	f.path = path;
	pthread_rwlock_wrlock(&fileListLock);
	fileList.addLocalFile(f);
	pthread_rwlock_unlock(&fileListLock);
	return 0;
}

int FileManager::addDiskFile(Descriptor file, const std::string &diskPath)
{
	LocalFile f(localNode);
	f = file;
	std::string path = fileList.copyToFileDir(diskPath);

	// add file to list
	f.path = path;
	pthread_rwlock_wrlock(&fileListLock);
	fileList.addLocalFile(f);
	pthread_rwlock_unlock(&fileListLock);
	return 0;
}

const conn::IPv4Address FileManager::getNode(Descriptor file)
{
	pthread_rwlock_rdlock(&fileListLock);
	conn::IPv4Address n = fileList.findFile(file).node;
	pthread_rwlock_unlock(&fileListLock);
	return n;
}

const std::vector<Descriptor> FileManager::listAllFiles()
{
	std::vector<Descriptor> f;
	pthread_rwlock_rdlock(&fileListLock);
	f = fileList.listAll();
	pthread_rwlock_unlock(&fileListLock);
	return f;
}

const std::vector<Descriptor> FileManager::listLocalFiles()
{
	std::vector<Descriptor> f;
	pthread_rwlock_rdlock(&fileListLock);
	f = fileList.listAllLocal();
	pthread_rwlock_unlock(&fileListLock);
	return f;
}

int FileManager::addRemoteFile(Descriptor descriptor, conn::IPv4Address node, unsigned long long int date)
{
	return addRemoteFiles({descriptor}, node, date);
}

int FileManager::addRemoteFiles(std::vector<Descriptor> descriptors, conn::IPv4Address node, unsigned long long int date)
{
	pthread_rwlock_wrlock(&fileListLock);
	auto it = listDates.find(node.getAddress());
	if(it != listDates.end())
	{
		if(it->second > date) // older list, do nothing
		{
			pthread_rwlock_unlock(&fileListLock);
			return 1;
		}
		else if(it->second < date) // new list, delete all descriptors from this node
		{
			fileList.deleteFromNode(it->first);
		}
	}
	// if new or current list add/update date entry and descriptor entries
	listDates[node.getAddress()] = date;
	for(auto it : descriptors)
	{
		File f(node);
		f = it;
		fileList.addRemoteFile(f);
	}
	pthread_rwlock_unlock(&fileListLock);
	return 0;
}

int FileManager::removeRemoteFile(Descriptor descriptor)
{
	pthread_rwlock_wrlock(&fileListLock);
	fileList.deleteRemoteFile(descriptor);
	pthread_rwlock_unlock(&fileListLock);
	return 0;
}

// for deadbody or something
int FileManager::removeRemoteFilesFromNode(conn::IPv4Address node)
{
	pthread_rwlock_wrlock(&fileListLock);
	fileList.deleteFromNode(node.getAddress());
	pthread_rwlock_unlock(&fileListLock);
	return 0;
}


// untested
int FileManager::deactivateLocalFile(Descriptor file)
{
	pthread_rwlock_wrlock(&fileListLock);
	LocalFile &f = fileList.findLocalFile(file);

	pthread_mutex_lock(&f.mutex);
	f.active = false;
	if (f.threadCount == 0)
	{
		pthread_mutex_unlock(&f.mutex);
		remove(f.path.c_str());
		fileList.deleteLocalFile(f); //delete from file list
		pthread_rwlock_unlock(&fileListLock);
		return 0;

	}
	pthread_mutex_unlock(&f.mutex);
	pthread_rwlock_unlock(&fileListLock);
	return 0;
}

// returns fd open for reading
int FileManager::openLocalFile(Descriptor file)
{
	int fd;
	pthread_rwlock_rdlock(&fileListLock);
	LocalFile &f = fileList.findLocalFile(file);
	pthread_mutex_lock(&f.mutex);
	f.threadCount++;
	pthread_mutex_unlock(&f.mutex);
	fd = open(f.path.c_str(), O_RDONLY);
	pthread_rwlock_unlock(&fileListLock);
	return fd;
}

// close the fd yourself
int FileManager::closeLocalFile(Descriptor file)
{
	pthread_rwlock_wrlock(&fileListLock); // wr because we might need to delete it
	LocalFile &f = fileList.findLocalFile(file);

	pthread_mutex_lock(&f.mutex);
	f.threadCount--;
	if (f.threadCount == 0 && !f.active)
	{
		pthread_mutex_unlock(&f.mutex);
		pthread_mutex_destroy(&f.mutex);
		remove(f.path.c_str()); // at this point, no other thread should be waiting for it
		fileList.deleteLocalFile(f); //delete from file list
		pthread_rwlock_unlock(&fileListLock);
		return 0;
	}
	pthread_mutex_unlock(&f.mutex);
	pthread_rwlock_unlock(&fileListLock);
	return 0;
}

int FileManager::makeLocalFileRemote(Descriptor file, conn::IPv4Address newNode)
{
	pthread_rwlock_wrlock(&fileListLock);
	LocalFile &f = fileList.findLocalFile(file);
	File newF = f;
	newF.node = newNode;

	pthread_mutex_lock(&f.mutex);
	f.active = false;
	if (f.threadCount == 0)
	{
		pthread_mutex_unlock(&f.mutex);
		remove(f.path.c_str());
		fileList.deleteLocalFile(f); //delete from file list
	}
	else pthread_mutex_unlock(&f.mutex);
	fileList.addRemoteFile(newF);
	pthread_rwlock_unlock(&fileListLock);
	return 0;
}

bool FileManager::isActive(Descriptor localFile)
{
	bool a;
	pthread_rwlock_rdlock(&fileListLock);
	LocalFile &f = fileList.findLocalFile(localFile);
	pthread_mutex_lock(&f.mutex);
	a = f.active;
	pthread_mutex_unlock(&f.mutex);
	pthread_rwlock_unlock(&fileListLock);
	return a;
}