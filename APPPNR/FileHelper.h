#pragma once

#include <stdio.h>
#include <string>
#include <vector>

#define MAF_UNLINK_EMPTY_DIRS 1
#define MAF_IGNORE_FILES_MOVE_ERROR (1u << 1u)

#define CAF_UNLINK_EMPTY_DIRS 1
#define CAF_IGNORE_FILES_MOVE_ERROR (1u << 1u)

#define LFON_STRICT_MATCH 1

struct Buffer;

class FileHelper {
public:
    static bool FileExist(const std::string& filePathStr);
    static off_t getFileSize(FILE* pFile);
    static bool ReadFile(const std::string& filePath, Buffer& pOutBuffer, bool bBinary = true);
    static bool WriteFile(const std::string& filePath, Buffer& pOutBuffer, bool bBinary = true);
    static bool ReplaceAllOcurrInFile(const std::string& filePath, const std::string& toReplace, const std::string& with);
    static void ReplaceAllOcurrInFilesFromRoot(const std::string& rootPath, const std::string& toReplace, const std::string& with);
    static bool MoveAllFiles(const std::string& oldRoot, std::string& newRoot, int flags = 0);
    static bool CopyAllFiles(const std::string& oldRoot, std::string& newRoot, int flags = 0);
    static bool MakePath(const std::string& fullPath);
    static int DeleteDirectory(const std::string& refcstrRootDirectory, bool bDeleteSubdirectories = true);
    static bool LookupOcurrencesByName(const std::string& rootPath, const std::string& name, std::vector<std::string>& outOcurrences);
    static bool LookupFilesOcurrencesByName(const std::string& rootPath, const std::string& name, std::vector<std::string>& outOcurrences, int flags = 0);
    static bool IsFolderEmpty(const std::string& entryPath);
};

