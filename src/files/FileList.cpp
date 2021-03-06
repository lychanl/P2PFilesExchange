#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

#include "FileList.h"

using namespace files;

FileList::FileList(const std::string& fileDir) : fileDir(fileDir)
{

}

int FileList::deleteLocalFile(const Descriptor& desc)
{
	auto it = localFiles.find(desc);
	if(it == localFiles.end()) {
		throw(std::out_of_range("file with given descriptor not found"));
	}
	localFiles.erase(it);
	return 0;
}

File &FileList::findFile(const Descriptor& file)
{
	auto it = remoteFiles.find(file);
	if (it != remoteFiles.end()) return it->second;
	return findLocalFile(file);
}

LocalFile &FileList::findLocalFile(const Descriptor& file)
{
	auto it = localFiles.find(file);
	if(it == localFiles.end()) {
		throw std::out_of_range("file with given descriptor not found");
	}
	return it->second;
}

const File &FileList::findFile(const Descriptor& file) const
{
	auto it = remoteFiles.find(file);
	if (it != remoteFiles.end()) return it->second;
	return findLocalFile(file);
}

const LocalFile &FileList::findLocalFile(const Descriptor& file) const
{
	auto it = localFiles.find(file);
	if(it == localFiles.end()) {
		throw std::out_of_range("file with given descriptor not found");
	}
	return it->second;
}

int FileList::addLocalFile(LocalFile &file)
{
	localFiles[file] = file;
	return 0;
}

int FileList::addRemoteFile(File &file)
{
	remoteFiles[file] = file;
	return 0;
}

int FileList::deleteRemoteFile(const Descriptor& desc)
{
	auto it = remoteFiles.find(desc);
	if(it == remoteFiles.end()) {
		throw(std::out_of_range("file with given descriptor not found"));
	}
	remoteFiles.erase(it);
	return 0;
}

std::string FileList::copyToFileDir(const std::string& in)
{
	int input, output;
	const char *baseName = basename(in.c_str());
	std::string out = baseName;
	if(fileDir.size() > 0)
	{
		out = fileDir + "/" + baseName;
	}
	if(in == out)
	{
		return in;
	}
	if ((input = open(in.c_str(), O_RDONLY)) == -1)
	{
		return "";
	}
	if ((output = creat(out.c_str(), 0660)) == -1)
	{
		close(input);
		return "";
	}
	off_t bytesCopied = 0;
	struct stat fileInfo = {0};
	fstat(input, &fileInfo);
	sendfile(output, input, &bytesCopied, static_cast<size_t>(fileInfo.st_size));
	close(input);
	close(output);
	return out;
}

const std::vector<Descriptor> FileList::listAll() const
{
	std::vector<Descriptor> f;
	for (auto it : remoteFiles)
	{
		f.push_back(it.first);
	}
	for (auto it : localFiles)
	{
		f.push_back(it.first);
	}
	return f;
}


const std::vector<Descriptor> FileList::listAllRemote() const
{
	std::vector<Descriptor> f;
	for (auto it : remoteFiles)
	{
		f.push_back(it.first);
	}
	return f;
}
const std::vector<Descriptor> FileList::listAllLocal() const
{
	std::vector<Descriptor> f;
	for (auto it : localFiles)
	{
		f.push_back(it.first);
	}
	return f;
}

int FileList::deleteFromNode(unsigned long int nodeAddress)
{
	std::vector<Descriptor> deleteMe;
	for(auto it = remoteFiles.begin(); it != remoteFiles.end(); ++it)
	{
		if(it->second.node.getAddress() == nodeAddress)
		{
			deleteMe.push_back(it->first);
		}
	}
	for(auto it : deleteMe) {
		remoteFiles.erase(it);
	}
	return 0;
}

const std::string &FileList::getFileDir() const
{
    return fileDir;
}
