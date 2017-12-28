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
		explicit FileList(std::string fileDir);

		std::string copyToFileDir(std::string in);

		int addLocalFile(LocalFile &file);

		int deleteLocalFile(Descriptor desc);

		int addRemoteFile(File &file);

		int deleteRemoteFile(Descriptor desc);

		int deleteFromNode(unsigned long int nodeAddress);

		const std::vector<Descriptor> listAll() const;

		const std::vector<Descriptor> listAllLocal() const;

		File &findFile(Descriptor file);

		LocalFile &findLocalFile(Descriptor file);

		const File &findFile(Descriptor file) const;

		const LocalFile &findLocalFile(Descriptor file) const;

	private:
		std::unordered_map<Descriptor, File, DescriptorHasher> remoteFiles;
		std::unordered_map<Descriptor, LocalFile, DescriptorHasher> localFiles;
		std::string fileDir;
	};
}


#endif //P2PFILESEXCHANGE_FILELIST_H
