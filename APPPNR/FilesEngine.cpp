#include "FilesEngine.h"
#include "Buffer.h"
#include "FileHelper.h"

bool FilesEngine::LoadFile(const std::string& pathStr, File** pOutFile, bool bBinary)
{
	File* pFile = new File();
	bool bFileReaded = false;

	if (pFile)
	{
		pFile->pBuff = new Buffer();

		if (pFile->pBuff)
		{
			bFileReaded = FileHelper::ReadFile(pathStr, *(pFile->pBuff), bBinary);

			if (bFileReaded)
			{
				pFile->filePathStr = pathStr;

				if (pOutFile)
					*pOutFile = pFile;

				allFiles.push_back(pFile);
			}
			else delete pFile;
		}
		else delete pFile;
	}

	return bFileReaded;
}

size_t FilesEngine::getFilesLoadedCount()
{
	return allFiles.size();
}

const std::vector<File*>& FilesEngine::getAllFiles()
{
	return allFiles;
}

FilesEngine::FilesEngine()
{}

FilesEngine::~FilesEngine()
{
	for (const auto* pFile : allFiles)
		delete pFile;
}
