#pragma once

#include "AppPnr.h"
#include "Utils.h"
#include <algorithm>
#include "FileHelper.h"
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include "RandomHelper.h"
#include "ThreadPool.h"

bool GradleAppPnr::Init()
{
	if (mOldPkgName.empty())
	{
		std::string oldPkgName = "";

		TryFindPackageName(oldPkgName);

		setOldPackageName(oldPkgName);
	}

	mGradlePath = mProjectEntryPath + "/.gradle";


	// Checking Gradle Folder Exist
	struct stat st;

	if (stat(mGradlePath.c_str(), &st))
	{
		std::cout << "Gradle Folder not Found, is this project Gradle?" << std::endl;

		return false;
	}

	return AppPnr::Init();
}

void GradleAppPnr::Run()
{
	bool bOk = true;

	if ((bOk = CleanCache()) != true)
		std::cout << "Unable to Clear the Gradle Cache" << std::endl;

	if (bOk)
		AppPnr::Run();
}

void GradleAppPnr::TryFindPackageName(std::string& outPkgName)
{
	std::string buildGradleFullPath = mProjectEntryPath + "/" + mBuildGradlePath;
	struct stat st;

	if (!stat(buildGradleFullPath.c_str(), &st))
	{
		Utils::ExtractParamByArgName(buildGradleFullPath, mAppIdArgName, outPkgName);
	}
}

void GradleAppPnr::setAppPath(const std::string& _appPath)
{
	mAppPath = _appPath;
}

void GradleAppPnr::setBuildGradlePath(const std::string& _buildGradlePath)
{
	mBuildGradlePath = _buildGradlePath;
}

void GradleAppPnr::setApplicationIdArgName(const std::string& _appIdArgName)
{
	mAppIdArgName = _appIdArgName;
}

bool GradleAppPnr::HasCache()
{
	std::vector<std::string> allOcurrences;
	std::string fullAppIntermediariesPath = mProjectEntryPath + "/" + mAppPath + "/build/intermediaries";

	bool bHasGradleCache = FileHelper::LookupFilesOcurrencesByName(mGradlePath, "executionHistory", allOcurrences);
	bool bHasIntermediariesCache = FileHelper::IsFolderEmpty(fullAppIntermediariesPath);

	return bHasGradleCache || bHasIntermediariesCache;
}

bool GradleAppPnr::CleanCache()
{
	std::vector<std::string> allOcurrences;
	std::string fullAppIntermediariesPath = mProjectEntryPath + "/" + mAppPath + "build/intermediates";

	bool bHasGradleCache = FileHelper::LookupFilesOcurrencesByName(mGradlePath, "executionHistory", allOcurrences);
	bool bHasIntermediariesCache = !FileHelper::IsFolderEmpty(fullAppIntermediariesPath);

	if (bHasIntermediariesCache)
	{
		std::cout << "Erasing Intermediates Cache" << std::endl;

		if (FileHelper::DeleteDirectory(fullAppIntermediariesPath))
			return false;
	}

	if (bHasGradleCache)
	{
		std::cout << "Erasing Gradle Cache" << std::endl;

		for (std::string ocurrence : allOcurrences)
		{		
			if (DeleteFileA(ocurrence.c_str()) == 0)
				return false;
		}
	}

	return true;
}

bool AppPnr::HandlePackageFolders()
{
	bool bSuccess = false;
	std::cout << "Moving Folders from '" << mSlashedOldPkgName << "' to '" << mSlashedNewPkgName << "'" << std::endl;

	std::string oldPackageFullPath = mProjectEntryPath + "/" + mJavaSourceEntryPath + "/" + mSlashedOldPkgName;
	std::string newPackageFullPath = mProjectEntryPath + "/" + mJavaSourceEntryPath + "/" + mSlashedNewPkgName;

	if (FileHelper::MakePath(newPackageFullPath))
	{
		bSuccess = FileHelper::CopyAllFiles(oldPackageFullPath, newPackageFullPath);
		
		HandleCleanUp(!bSuccess);
	}

	return bSuccess;
}

// When Revert is true is usually becouse something went 
// Wrong and want to revert files copied back 
void AppPnr::HandleCleanUp(bool bRevert)
{
	std::string deltaPath = mProjectEntryPath + "/" + mJavaSourceEntryPath;
	std::vector<std::string> oldPackageSplit;  getAllOldPackagesSplit(oldPackageSplit);
	std::vector<std::string> newPackageSplit;  getAllNewPackagesSplit(newPackageSplit);
	size_t minorSize = min(oldPackageSplit.size(), newPackageSplit.size());
	size_t ordIndx = minorSize - 1;

	while (ordIndx >= 0)
	{
		const std::string& currOldFold = oldPackageSplit[ordIndx];
		const std::string& currNewFold = newPackageSplit[ordIndx];

		if (!strcmp(currOldFold.c_str(), currNewFold.c_str()))
		{
			ordIndx++;
			break;
		}

		ordIndx--;
	}

	// Doing the actual cleanup

	if (ordIndx != -1)
	{

		std::string toCleanUpRootPathName = "";
		
		for (size_t i = 0; i < ordIndx; i++)
			deltaPath += ('/' + oldPackageSplit[i]);
		
		toCleanUpRootPathName = deltaPath + '/' + ((bRevert) ? newPackageSplit[ordIndx] : oldPackageSplit[ordIndx]);

		std::cout << "Cleaning Up '" << mOldPkgName << "' Directories" << std::endl;

		FileHelper::DeleteDirectory(toCleanUpRootPathName);
	}
}

bool AppPnr::Init()
{
	if (mNewPkgName.empty())
	{
		std::string tempNewPackageName = RAND_PACKAGE_PREFIX + RandomHelper::getRandomSenseString(RandomHelper::getRandomInBtw(5, 8)) + "." + RandomHelper::getRandomSenseString(RandomHelper::getRandomInBtw(5, 8));

		setNewPackageName(tempNewPackageName);
	}
	return true;
}

void AppPnr::Run()
{
	if (strcmp(mOldPkgName.c_str(), mNewPkgName.c_str()) || mbForce)
	{
		// Handle java package folders rename

		if (!HandlePackageFolders())
		{
			std::cout << "Error Moving the files, make sure another program doesnt have the files open" << std::endl;
			return;
		}
		
		std::cout << "Remplacing '" << mOldPkgName << "' With '" << mNewPkgName << "'" << std::endl;
		FileHelper::ReplaceAllOcurrInFilesFromRoot(mProjectEntryPath, mOldPkgName, mNewPkgName);

		if (mbReplaceSlashedPkgNames)
		{
			std::cout << "Remplacing '" << mSlashedOldPkgName << "' With '" << mSlashedNewPkgName << "'" << std::endl;
			FileHelper::ReplaceAllOcurrInFilesFromRoot(mProjectEntryPath, mSlashedOldPkgName, mSlashedNewPkgName);
		}

		if (mbReplaceUnderscoredPkgName)
		{
			std::cout << "Remplacing '" << mUnderscoredOldPkgName << "' With '" << mUnderscoredNewPkgName << "'" << std::endl;
			FileHelper::ReplaceAllOcurrInFilesFromRoot(mProjectEntryPath, mUnderscoredOldPkgName, mUnderscoredNewPkgName);
		}

		std::cout << "Successfully Executed all operations!" << std::endl;
	}
	else std::cout << "Alredy '" << mNewPkgName << "' As Package Name" << std::endl;
}

void AppPnr::setJavaSourceEntryPath(const std::string& _javaSourceEntryPath)
{
	mJavaSourceEntryPath = _javaSourceEntryPath;
}

void AppPnr::setProjectEntryPath(const std::string& _projEntryPath)
{
	mProjectEntryPath = _projEntryPath;
}

void AppPnr::setOldPackageName(const std::string& _oldPkgName)
{
	mUnderscoredOldPkgName = mSlashedOldPkgName = mOldPkgName = _oldPkgName;

	Utils::ReplaceAllOcurrences(mSlashedOldPkgName, ".", "/");
	Utils::ReplaceAllOcurrences(mUnderscoredOldPkgName, ".", "_");
}

void AppPnr::setNewPackageName(const std::string& _newPkgName)
{
	mUnderscoredNewPkgName = mSlashedNewPkgName = mNewPkgName = _newPkgName;

	Utils::ReplaceAllOcurrences(mSlashedNewPkgName, ".", "/");
	Utils::ReplaceAllOcurrences(mUnderscoredNewPkgName, ".", "_");
}

void AppPnr::setForce(bool bForce)
{
	mbForce = bForce;
}

void AppPnr::getAllNewPackagesSplit(std::vector<std::string>& outAllPackagesSplit)
{
	Utils::explode(mNewPkgName, '.', outAllPackagesSplit);
}

void AppPnr::getAllOldPackagesSplit(std::vector<std::string>& outAllPackagesSplit)
{
	Utils::explode(mOldPkgName, '.', outAllPackagesSplit);
}

void AppPnr::setReplaceSlashedPackageName(bool replaceSlashedPkgName)
{
	mbReplaceSlashedPkgNames = replaceSlashedPkgName;
}

void AppPnr::setReplaceUnderscoredPackageName(bool replaceUnderscoredPkgName)
{
	mbReplaceUnderscoredPkgName = replaceUnderscoredPkgName;
}
