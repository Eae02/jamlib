#include "Asset.hpp"
#include <miniz.h>
#include <future>

namespace jm::detail
{
	void FreeArchive(mz_zip_archive* archive)
	{
		mz_zip_end(archive);
		delete archive;
	}
	
	bool FindAssetsZip(ProcessAssetCB processAsset)
	{
		auto* archivePtr = new mz_zip_archive();
		if (!mz_zip_reader_init_file(archivePtr, "./assets.zip", 0))
		{
			return false;
		}
		
		std::shared_ptr<mz_zip_archive> archive(archivePtr, &FreeArchive);
		
		int numFiles = mz_zip_reader_get_num_files(archive.get());
		for (int i = 0; i < numFiles; i++)
		{
			mz_zip_archive_file_stat fileStat;
			if (!mz_zip_reader_file_stat(archive.get(), i, &fileStat))
				continue;
			if (mz_zip_reader_is_file_a_directory(archive.get(), i))
				continue;
			
			size_t fileSize = fileStat.m_uncomp_size;
			
			processAsset(fileStat.m_filename, [=] () -> std::vector<char>
			{
				std::vector<char> extractedData(fileSize);
				mz_zip_reader_extract_to_mem(archive.get(), i, extractedData.data(), fileSize, 0);
				return extractedData;
			});
		}
		
		return true;
	}
}
