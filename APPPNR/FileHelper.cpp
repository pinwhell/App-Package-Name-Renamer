#include "FileHelper.h"
#include "dirent.h"
#include "Buffer.h"
#include "FilesEngine.h"
#include "Utils.h"
#include "FileHelper.h"
#include <Windows.h>

bool FileHelper::FileExist(const std::string& filePathStr)
{
    struct stat st;

    return !stat(filePathStr.c_str(), &st);
}

off_t FileHelper::getFileSize(FILE* pFile)
{
    off_t currOffset = ftell(pFile);

    // Going to the bottom
    fseek(pFile, 0, SEEK_END);
    off_t bottomOffset = ftell(pFile);

    // Restoring Stream Pointer
    fseek(pFile, currOffset, SEEK_SET);

    return bottomOffset;
}

bool FileHelper::ReadFile(const std::string& filePath, Buffer& pOutBuffer, bool bBinary)
{
    FILE* pFile = nullptr;
    bool bFileReaded = false;

    const char* pWriteType = "r";

    if (bBinary)
        pWriteType = "rb";

    if (!fopen_s(&pFile, filePath.c_str(), pWriteType))
    {
        size_t fileSize = getFileSize(pFile);

        if (fileSize > 0)
        {
            pOutBuffer.mSize = fileSize;
            pOutBuffer.mBuff = new unsigned char[fileSize];

            if (pOutBuffer.mBuff) bFileReaded = fread(pOutBuffer.mBuff, fileSize, 1, pFile) > 0;
        }

        fclose(pFile);
    }

    return bFileReaded;
}

bool FileHelper::WriteFile(const std::string& filePath, Buffer& pOutBuffer, bool bBinary)
{
    FILE* pFile = nullptr;
    bool bFileWrited = false;
    const char* pWriteType = "w";

    if (bBinary)
        pWriteType = "wb";


    if (!fopen_s(&pFile, filePath.c_str(), pWriteType))
    {
        if (pOutBuffer.mBuff) bFileWrited = fwrite(pOutBuffer.mBuff, pOutBuffer.mSize, 1, pFile) > 0;

        fclose(pFile);
    }

    return bFileWrited;
}

bool FileHelper::ReplaceAllOcurrInFile(const std::string& filePath, const std::string& toReplace, const std::string& with)
{
    FilesEngine fe;
    File* pFile = nullptr;
    bool bSuccess = false;

    if (fe.LoadFile(filePath, &pFile))
    {
        std::string fullFile = std::string((const char*)(pFile->pBuff->mBuff), pFile->pBuff->mSize);
        Utils::ReplaceAllOcurrences(fullFile, toReplace, with);
        bSuccess = pFile->OverrideWithString(fullFile);
    }

    return bSuccess;
}

void FileHelper::ReplaceAllOcurrInFilesFromRoot(const std::string& rootPath, const std::string& toReplace, const std::string& with)
{
    DIR* pDir = opendir(rootPath.c_str());
    struct dirent* currEnt = nullptr;

    while ((currEnt = readdir(pDir)) != nullptr)
    {
        if (!strcmp(currEnt->d_name, "..") ||
            !strcmp(currEnt->d_name, "."))
            continue;

        std::string currPath = rootPath + "/" + std::string(currEnt->d_name);

        if (currEnt->d_type & DT_DIR)
            ReplaceAllOcurrInFilesFromRoot(currPath, toReplace, with);
        else if (currEnt->d_type & S_IFREG)
            ReplaceAllOcurrInFile(currPath, toReplace, with);
    }
}

bool FileHelper::MoveAllFiles(const std::string& oldRoot, std::string& newRoot, int flags)
{
    bool bUnlinkEmptyDirs = flags & MAF_UNLINK_EMPTY_DIRS;
    bool bIgnoreFilesError = flags & MAF_IGNORE_FILES_MOVE_ERROR;
    bool bSucess = true;
    DIR* pDir = opendir(oldRoot.c_str());
    struct dirent* currEnt = nullptr;

    if (pDir)
    {
        while ((currEnt = readdir(pDir)) != nullptr)
        {
            if (!strcmp(currEnt->d_name, "..") ||
                !strcmp(currEnt->d_name, "."))
                continue;

            std::string oldRootPath = oldRoot + "/" + std::string(currEnt->d_name);
            std::string newRootPath = newRoot + "/" + std::string(currEnt->d_name);

            if (currEnt->d_type & DT_DIR)
            {
                if (!FileHelper::FileExist(newRootPath))
                    CreateDirectoryA(newRootPath.c_str(), NULL);

                bool bOperationResult = MoveAllFiles(oldRootPath, newRootPath, flags);

                if (!bOperationResult && !bIgnoreFilesError)
                {
                    bSucess = false;
                    break;
                }
            }
            else if (currEnt->d_type & S_IFREG)
            {
                bool bCurrentFileOperationResult = !rename(oldRootPath.c_str(), newRootPath.c_str());

                if (!bCurrentFileOperationResult && !bIgnoreFilesError)
                {
                    bSucess = false;
                    break;
                }
            }
        }
    }
    else bSucess = false;

    if (bUnlinkEmptyDirs && bSucess)
        RemoveDirectoryA(oldRoot.c_str());

    return bSucess;
}

bool FileHelper::LookupOcurrencesByName(const std::string& rootPath, const std::string& name, std::vector<std::string>& outOcurrences)
{
    DIR* pDir = opendir(rootPath.c_str());
    struct dirent* currEnt = nullptr;

    if (pDir)
    {
        while ((currEnt = readdir(pDir)) != nullptr)
        {
            if (!strcmp(currEnt->d_name, "..") ||
                !strcmp(currEnt->d_name, "."))
                continue;

            std::string currPath = rootPath + "/" + std::string(currEnt->d_name);

            if (!strcmp(currEnt->d_name, name.c_str()))
                outOcurrences.push_back(currPath);

            if (currEnt->d_type & DT_DIR)
                LookupOcurrencesByName(currPath, name, outOcurrences);
        }
    }

    return outOcurrences.size() != 0;
}

bool FileHelper::IsFolderEmpty(const std::string& entryPath)
{
    DIR* pDir = opendir(entryPath.c_str());
    struct dirent* currEnt = nullptr;

    if (pDir)
    {
        while ((currEnt = readdir(pDir)) != nullptr)
        {
            if (!strcmp(currEnt->d_name, "..") ||
                !strcmp(currEnt->d_name, "."))
                continue;

            return false;
        }

        closedir(pDir);
    }

    return true;
}

bool FileHelper::LookupFilesOcurrencesByName(const std::string& rootPath, const std::string& name, std::vector<std::string>& outOcurrences, int flags)
{
    bool bStrictMatch = flags & LFON_STRICT_MATCH;

    DIR* pDir = opendir(rootPath.c_str());
    struct dirent* currEnt = nullptr;

    if (pDir)
    {
        while ((currEnt = readdir(pDir)) != nullptr)
        {
            if (!strcmp(currEnt->d_name, "..") ||
                !strcmp(currEnt->d_name, "."))
                continue;

            std::string currPath = rootPath + "/" + std::string(currEnt->d_name);

            if (currEnt->d_type & DT_DIR)
                LookupFilesOcurrencesByName(currPath, name, outOcurrences, flags);
            else if(currEnt->d_type & S_IFREG && 
                (bStrictMatch ? !strcmp(currEnt->d_name, name.c_str()) : (strstr(currEnt->d_name, name.c_str()) != nullptr)))
                outOcurrences.push_back(currPath);
        }
    }

    return outOcurrences.size() != 0;
}

int FileHelper::DeleteDirectory(const std::string& refcstrRootDirectory, bool bDeleteSubdirectories)
{
    bool            bSubdirectory = false;       // Flag, indicating whether
                                                 // subdirectories have been found
    HANDLE          hFile;                       // Handle to directory
    std::string     strFilePath;                 // Filepath
    std::string     strPattern;                  // Pattern
    WIN32_FIND_DATA FileInformation;             // File information

    strPattern = refcstrRootDirectory + "\\*.*";
    hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (FileInformation.cFileName[0] != '.')
            {
                strFilePath.erase();
                strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

                if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    if (bDeleteSubdirectories)
                    {
                        // Delete subdirectory
                        int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
                        if (iRC)
                            return iRC;
                    }
                    else
                        bSubdirectory = true;
                }
                else
                {
                    // Set file attributes
                    if (::SetFileAttributes(strFilePath.c_str(),
                        FILE_ATTRIBUTE_NORMAL) == FALSE)
                        return ::GetLastError();

                    // Delete file
                    if (::DeleteFile(strFilePath.c_str()) == FALSE)
                        return ::GetLastError();
                }
            }
        } while (::FindNextFile(hFile, &FileInformation) == TRUE);

        // Close handle
        ::FindClose(hFile);

        DWORD dwError = ::GetLastError();
        if (dwError != ERROR_NO_MORE_FILES)
            return dwError;
        else
        {
            if (!bSubdirectory)
            {
                // Set directory attributes
                if (::SetFileAttributes(refcstrRootDirectory.c_str(),
                    FILE_ATTRIBUTE_NORMAL) == FALSE)
                    return ::GetLastError();

                // Delete directory
                if (::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
                    return ::GetLastError();
            }
        }
    }

    return 0;
}

bool FileHelper::CopyAllFiles(const std::string& oldRoot, std::string& newRoot, int flags)
{
    bool bUnlinkEmptyDirs = flags & MAF_UNLINK_EMPTY_DIRS;
    bool bIgnoreFilesError = flags & MAF_IGNORE_FILES_MOVE_ERROR;
    bool bSucess = true;
    DIR* pDir = opendir(oldRoot.c_str());
    struct dirent* currEnt = nullptr;

    if (pDir)
    {
        while ((currEnt = readdir(pDir)) != nullptr)
        {
            if (!strcmp(currEnt->d_name, "..") ||
                !strcmp(currEnt->d_name, "."))
                continue;

            std::string oldRootPath = oldRoot + "/" + std::string(currEnt->d_name);
            std::string newRootPath = newRoot + "/" + std::string(currEnt->d_name);

            if (currEnt->d_type & DT_DIR)
            {
                if (!FileHelper::FileExist(newRootPath))
                    CreateDirectoryA(newRootPath.c_str(), NULL);

                bool bOperationResult = CopyAllFiles(oldRootPath, newRootPath, flags);

                if (!bOperationResult && !bIgnoreFilesError)
                {
                    bSucess = false;
                    break;
                }
            }
            else if (currEnt->d_type & S_IFREG)
            {
                bool bCurrentFileOperationResult = CopyFileA(oldRootPath.c_str(), newRootPath.c_str(), false) != 0;

                if (!bCurrentFileOperationResult && !bIgnoreFilesError)
                {
                    bSucess = false;
                    break;
                }
            }
        }
    }
    else bSucess = false;

    if (bUnlinkEmptyDirs && bSucess)
        RemoveDirectoryA(oldRoot.c_str());

    return bSucess;
}

bool FileHelper::MakePath(const std::string& fullPath)
{
    //Find Where is the path valid at
    std::string rootPath = fullPath;
    std::vector<std::string> fullPathExploded = Utils::explode(fullPath, '/');
    std::vector<std::string> rootPathExploded;
    size_t foldrsBckwrd = fullPathExploded.size() - 1u;
    bool bSucceed = true;

    while (!FileHelper::FileExist(rootPath))
    {
        rootPathExploded = Utils::explode(rootPath, '/'); rootPathExploded.pop_back();

        rootPath = Utils::implode(rootPathExploded, '/');

        foldrsBckwrd--;
    }

    for (size_t i = foldrsBckwrd + 1; i < fullPathExploded.size(); i++)
    {
        rootPathExploded.push_back(fullPathExploded[i]);

        std::string currRoot = Utils::implode(rootPathExploded, '/');

        if (!CreateDirectoryA(currRoot.c_str(), NULL))
        {
            bSucceed = false;
            break;
        }
    }

    return bSucceed && foldrsBckwrd != 0;
}
