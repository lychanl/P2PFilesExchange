#include "FileManager.h"

using namespace files;

files::FileManager::FileManager()
{

}

int FileManager::addLocalFile(const std::string &localPath)
{
	return 0;
}

int FileManager::deleteLocalFile(FileManager::FileId file)
{
	return 0;
}

FileManager::Descriptor FileManager::getDescriptor(FileManager::FileId file) const
{
	return nullptr;
}

const conn::IPv4Address &FileManager::getOwner(FileManager::FileId file) const
{
	return conn::IPv4Address(0);
}

const conn::IPv4Address &FileManager::getNode(FileManager::FileId file) const
{
	return conn::IPv4Address(0);
}

int FileManager::deactivateLocalFile(FileManager::FileId file)
{
	return 0;
}

const FileManager::FileList &FileManager::listAllFiles() const
{
	return FileManager::FileList();
}

const FileManager::FileList &FileManager::listLocalFiles() const
{
	return FileManager::FileList();
}

int FileManager::addRemoteFile(FileManager::Descriptor descriptor)
{
	return 0;
}

int FileManager::setFileAsLocal(FileManager::FileId file, const std::string &localPath)
{
	return 0;
}
