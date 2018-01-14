#ifndef P2PFILESEXCHANGE_FILELIST_H
#define P2PFILESEXCHANGE_FILELIST_H

#include <unordered_map>
#include <vector>
#include "File.h"

namespace files
{
	class FileList
	{
	public:
		explicit FileList(const std::string& fileDir);

		std::string copyToFileDir(const std::string& in);

		int addLocalFile(LocalFile &file);

		int deleteLocalFile(const Descriptor& desc);

		int addRemoteFile(File &file);

		int deleteRemoteFile(const Descriptor& desc);

		int deleteFromNode(unsigned long int nodeAddress);

		const std::vector<Descriptor> listAll() const;

		const std::vector<Descriptor> listAllLocal() const;

		const std::vector<Descriptor> listAllRemote() const;

		File &findFile(const Descriptor& file);

		LocalFile &findLocalFile(const Descriptor& file);

		const File &findFile(const Descriptor& file) const;

		const LocalFile &findLocalFile(const Descriptor& file) const;

	private:
		std::unordered_map<Descriptor, File, DescriptorHasher> remoteFiles;
		std::unordered_map<Descriptor, LocalFile, DescriptorHasher> localFiles;
		LocalFile emptyFile;
		std::string fileDir;
	};
}


#endif //P2PFILESEXCHANGE_FILELIST_H
