#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

#include "FileList.h"

using namespace files;

FileList::FileList(std::string fileDir) : fileDir(fileDir)
{

}

int FileList::deleteLocalFile(Descriptor desc)
{
	auto it = localFiles.find(desc);
	if(it == localFiles.end()) {
		throw(std::out_of_range("file with given descriptor not found"));
	}
	localFiles.erase(it);
	return 0;
}

File &FileList::findFile(files::Descriptor file)
{
	auto it = remoteFiles.find(file);
	if (it != remoteFiles.end()) return it->second;
	return findLocalFile(file);
}

LocalFile &FileList::findLocalFile(files::Descriptor file)
{
	auto it = localFiles.find(file);
	if(it == localFiles.end()) {
		throw std::out_of_range("file with given descriptor not found");
	}
	return it->second;
}

const File &FileList::findFile(files::Descriptor file) const
{
	auto it = remoteFiles.find(file);
	if (it != remoteFiles.end()) return it->second;
	return findLocalFile(file);
}

const LocalFile &FileList::findLocalFile(files::Descriptor file) const
{
	auto it = localFiles.find(file);
	if(it == localFiles.end()) {
		throw std::out_of_range("file with given descriptor not found");
	}
	return it->second;
}

int FileList::addLocalFile(files::LocalFile &file)
{
	localFiles[file] = file;
	return 0;
}

int FileList::addRemoteFile(files::File &file)
{
	remoteFiles[file] = file;
	return 0;
}

int FileList::deleteRemoteFile(Descriptor desc)
{
	auto it = remoteFiles.find(desc);
	if(it == remoteFiles.end()) {
		throw(std::out_of_range("file with given descriptor not found"));
	}
	remoteFiles.erase(it);
	return 0;
}

std::string FileList::copyToFileDir(std::string in)
{
	int input, output;
	const char *baseName = basename(in.c_str());
	std::string out = fileDir + "/" + baseName;
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
