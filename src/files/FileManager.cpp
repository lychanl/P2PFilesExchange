#include <cstring>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include "FileManager.h"

using namespace files;

files::FileManager::FileManager(conn::IPv4Address localNode) : localNode(localNode)
{
	pthread_rwlock_init(&fileListLock, nullptr);
}

int FileManager::addDiskFile(const std::string &diskPath)
{
	LocalFile f(localNode);
	const char *baseName = basename(diskPath.c_str());
	std::string path = fileDir + "/" + baseName;

	// copy file to fileDir
	if (copyFile(diskPath, path) == -1) return -1;

	// add file to list
	f.owner = localNode.getAddress();
	f.date = time(nullptr);
	strncpy(f.name, baseName, 52);
	f.path = path;
	pthread_rwlock_wrlock(&fileListLock);
	localFiles.emplace(f, f);
	pthread_rwlock_unlock(&fileListLock);
	return 0;
}

int FileManager::addDiskFile(Descriptor file, const std::string &diskPath)
{
	LocalFile f(localNode);
	(Descriptor) f = file;
	std::string path = fileDir + "/" + f.name;

	// copy file to fileDir
	if (copyFile(diskPath, path) == -1) return -1;

	// add file to list
	f.path = path;
	pthread_rwlock_wrlock(&fileListLock);
	localFiles.emplace(file, f);
	pthread_rwlock_unlock(&fileListLock);
	return 0;
}

const conn::IPv4Address FileManager::getNode(FileManager::FileId file)
{
	pthread_rwlock_rdlock(&fileListLock);
	conn::IPv4Address n = findFile(file).node;
	pthread_rwlock_unlock(&fileListLock);
	return n;
}

const FileManager::FileList FileManager::listAllFiles()
{
	FileList f;
	pthread_rwlock_rdlock(&fileListLock);
	for (auto it : remoteFiles)
	{
		f.push_back(it.second);
	}
	for (auto it : localFiles)
	{
		f.push_back(it.second);
	}
	pthread_rwlock_unlock(&fileListLock);
	return f;
}

const FileManager::FileList FileManager::listLocalFiles()
{
	FileList f;
	pthread_rwlock_rdlock(&fileListLock);
	for (auto it : localFiles)
	{
		f.push_back(it.second);
	}
	pthread_rwlock_unlock(&fileListLock);
	return f;
}

int FileManager::addRemoteFile(FileManager::Descriptor descriptor, conn::IPv4Address node)
{
	File f(node);
	(Descriptor) f = descriptor;
	pthread_rwlock_wrlock(&fileListLock);
	remoteFiles.emplace(descriptor, f);
	pthread_rwlock_unlock(&fileListLock);
	return 0;
}

// untested
int FileManager::deactivateLocalFile(FileManager::FileId file)
{
	pthread_rwlock_wrlock(&fileListLock);
	LocalFile &f = findLocalFile(file);

	pthread_mutex_lock(&f.mutex);
	f.active = false;
	if (f.threadCount == 0)
	{
		remove(f.path.c_str());
		pthread_mutex_unlock(&f.mutex);
		pthread_mutex_destroy(&f.mutex);
		//pthread_rwlock_destroy(&f.lock);
		deleteLocalFile(f); //delete from file list
		pthread_rwlock_unlock(&fileListLock);
		return 0;

	}
	pthread_mutex_unlock(&f.mutex);
	pthread_rwlock_unlock(&fileListLock);
	return 0;
}

// returns fd open for reading
int FileManager::openLocalFile(FileManager::FileId file)
{
	int fd;
	pthread_rwlock_rdlock(&fileListLock);
	LocalFile &f = findLocalFile(file);
	pthread_mutex_lock(&f.mutex);
	f.threadCount++;
	pthread_mutex_unlock(&f.mutex);
	//pthread_rwlock_rdlock(&f.lock);
	fd = open(f.path.c_str(), O_RDONLY);
	pthread_rwlock_unlock(&fileListLock);
	return fd;
}

// close the fd yourself
int FileManager::closeLocalFile(FileManager::FileId file)
{
	pthread_rwlock_rdlock(&fileListLock);
	LocalFile &f = findLocalFile(file);

	//pthread_rwlock_unlock(&f.lock);
	pthread_mutex_lock(&f.mutex);
	f.threadCount--;
	if (f.threadCount == 0 && !f.active)
	{
		remove(f.path.c_str());
		pthread_mutex_unlock(&f.mutex); // at this point, no other thread should be waiting for it
		pthread_mutex_destroy(&f.mutex);
		//pthread_rwlock_destroy(&f.lock);
		deleteLocalFile(f); //delete from file list
		pthread_rwlock_unlock(&fileListLock);
		return 0;
	}
	pthread_mutex_unlock(&f.mutex);
	pthread_rwlock_unlock(&fileListLock);
}

// not thread safe
FileManager::File &FileManager::findFile(FileManager::FileId file)
{
	auto it = remoteFiles.find(file);
	if (it != remoteFiles.end()) return it->second;
	return findLocalFile(file);
}

// not thread safe
FileManager::LocalFile &FileManager::findLocalFile(FileManager::FileId file)
{
	return localFiles.find(file)->second;
}

// not thread safe
const FileManager::File &FileManager::findFile(FileManager::FileId file) const
{
	auto it = remoteFiles.find(file);
	if (it != remoteFiles.end()) return it->second;
	return findLocalFile(file);
}

// not thread safe
const FileManager::LocalFile &FileManager::findLocalFile(FileManager::FileId file) const
{
	return localFiles.find(file)->second;
}

// not thread safe
int FileManager::deleteLocalFile(LocalFile &file)
{
	auto it = localFiles.find(file);
	localFiles.erase(it);
	return 0;
}

int FileManager::copyFile(std::string in, std::string out)
{
	int input, output;
	if ((input = open(in.c_str(), O_RDONLY)) == -1)
	{
		return -1;
	}
	if ((output = creat(out.c_str(), 0660)) == -1)
	{
		close(input);
		return -1;
	}
	off_t bytesCopied = 0;
	struct stat fileInfo = {0};
	fstat(input, &fileInfo);
	ssize_t result = sendfile(output, input, &bytesCopied, fileInfo.st_size);
	close(input);
	close(output);
	return (int) result;
}

bool FileManager::Descriptor::operator==(const FileManager::Descriptor &other) const
{
	return (owner == other.owner) && (date == other.date) && (strncmp(name, other.name, 52) == 0);
}

bool FileManager::File::operator==(const FileManager::File &other) const
{
	return Descriptor::operator==(other);
}

FileManager::File::File(conn::IPv4Address node) : node(node)
{

}

FileManager::LocalFile::LocalFile(conn::IPv4Address node) : File(node)
{
	//pthread_rwlock_init(&lock, nullptr);
}