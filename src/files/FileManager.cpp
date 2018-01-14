#include <cstring>
#include <fcntl.h>
#include "FileManager.h"

using namespace files;

files::FileManager::FileManager(const std::string& fileDir) : localNode(0), fileList(fileDir)
{
	pthread_rwlock_init(&fileListLock, nullptr);
}

int FileManager::setLocalNode(const conn::IPv4Address& localNode)
{
	this->localNode = localNode;
	return 0;
}

FileManager::~FileManager()
{
	pthread_rwlock_destroy(&fileListLock);
}

Descriptor FileManager::addDiskFile(const std::string &diskPath)
{
	LocalFile f(localNode);
	std::string path = fileList.copyToFileDir(diskPath);
	const char *baseName = basename(path.c_str());

	// add file to list
	f.node = localNode;
	f.owner = localNode.getAddress();
	f.date = static_cast<unsigned long long int>(time(nullptr));
	strncpy(f.name, baseName, 52);
	f.path = path;
	pthread_rwlock_wrlock(&fileListLock);
	fileList.addLocalFile(f);
	pthread_rwlock_unlock(&fileListLock);
	return f;
}

int FileManager::addDiskFile(const Descriptor& file, const std::string &diskPath)
{
	LocalFile f(localNode);
	f = file;
	std::string path = fileList.copyToFileDir(diskPath);
	f.node = localNode;

	// add file to list
	f.path = path;
	pthread_rwlock_wrlock(&fileListLock);
	fileList.addLocalFile(f);
	pthread_rwlock_unlock(&fileListLock);
	return 0;
}

const conn::IPv4Address FileManager::getNode(const Descriptor& file)
{
	pthread_rwlock_rdlock(&fileListLock);
	try
	{
		conn::IPv4Address n = fileList.findFile(file).node;
		pthread_rwlock_unlock(&fileListLock);
		return n;
	}
	catch(std::out_of_range& e) {
		pthread_rwlock_unlock(&fileListLock);
		return conn::IPv4Address(0);
	}
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

int FileManager::addRemoteFile(const Descriptor& descriptor, const conn::IPv4Address& node, unsigned long long int date)
{
	return addRemoteFiles({descriptor}, node, date);
}

int FileManager::addRemoteFiles(const std::vector<Descriptor>& descriptors, const conn::IPv4Address& node, unsigned long long int date)
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

int FileManager::removeRemoteFile(const Descriptor& descriptor)
{
	pthread_rwlock_wrlock(&fileListLock);
	try
	{
		fileList.deleteRemoteFile(descriptor);
		pthread_rwlock_unlock(&fileListLock);
		return 0;
	}
	catch(std::out_of_range& e) {
		return 1;
	}
}

// for deadbody or something
int FileManager::removeRemoteFilesFromNode(const conn::IPv4Address& node)
{
	pthread_rwlock_wrlock(&fileListLock);
	fileList.deleteFromNode(node.getAddress());
	pthread_rwlock_unlock(&fileListLock);
	return 0;
}


int FileManager::deactivateLocalFile(const Descriptor& file)
{
	pthread_rwlock_wrlock(&fileListLock);
	try
	{
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
	catch(std::out_of_range& e) {
		pthread_rwlock_unlock(&fileListLock);
		return 1;
	}
}

// returns fd open for reading
int FileManager::openLocalFile(const Descriptor& file)
{
	int fd;
	pthread_rwlock_rdlock(&fileListLock);
	try
	{
		LocalFile &f = fileList.findLocalFile(file);
		pthread_mutex_lock(&f.mutex);
		f.threadCount++;
		pthread_mutex_unlock(&f.mutex);
		fd = open(f.path.c_str(), O_RDONLY);
		pthread_rwlock_unlock(&fileListLock);
		return fd;
	}
	catch(std::out_of_range& e) {
		pthread_rwlock_unlock(&fileListLock);
		return -1;
	}
}

// close the fd yourself
int FileManager::closeLocalFile(const Descriptor& file)
{
	pthread_rwlock_wrlock(&fileListLock); // wr because we might need to delete it
	try
	{
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
	catch (std::out_of_range& e) {
		pthread_rwlock_unlock(&fileListLock);
		return 1;
	}
}

int FileManager::makeLocalFileRemote(const Descriptor& file, const conn::IPv4Address& newNode)
{
	pthread_rwlock_wrlock(&fileListLock);
	try
	{
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
	catch (std::out_of_range& e) {
		pthread_rwlock_unlock(&fileListLock);
		return 1;
	}
}

bool FileManager::isActive(const Descriptor& localFile)
{
	bool a;
	pthread_rwlock_rdlock(&fileListLock);
	try
	{
		LocalFile &f = fileList.findLocalFile(localFile);
		pthread_mutex_lock(&f.mutex);
		a = f.active;
		pthread_mutex_unlock(&f.mutex);
		pthread_rwlock_unlock(&fileListLock);
		return a;
	}
	catch(std::out_of_range& e) {
		pthread_rwlock_unlock(&fileListLock);
		return false;
	}
}
