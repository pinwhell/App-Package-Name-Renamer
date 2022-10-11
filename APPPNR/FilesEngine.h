#pragma once

#include <string>
#include <vector>
#include "File.h"

class FilesEngine
{
private:
	std::vector<File*> allFiles;
public:
	bool LoadFile(const std::string& pathStr, File** pOutFile = nullptr, bool bBinary = true);
	size_t getFilesLoadedCount();
	const std::vector<File*>& getAllFiles();

	FilesEngine();
	~FilesEngine();
};

