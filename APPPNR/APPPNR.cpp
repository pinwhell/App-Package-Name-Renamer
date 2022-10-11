#pragma once

#include "AppPnr.h"
#include "Utils.h"
#include <algorithm>
#include "FileHelper.h"
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include "RandomHelper.h"

bool GradleAppPnr::Init()
{
	if (mOldPkgName.empty())
	{
		std::string oldPkgName = "";

		TryFindPackageName(oldPkgName);

		setOldPackageName(oldPkgName);
	}

	return AppPnr::Init();
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

void GradleAppPnr::setBuildGradlePath(const std::string& _buildGradlePath)
{
	mBuildGradlePath = _buildGradlePath;
}

void GradleAppPnr::setApplicationIdArgName(const std::string& _appIdArgName)
{
	mAppIdArgName = _appIdArgName;
}

bool AppPnr::HandlePackageFolders()
{
	bool bSuccess = false;
	std::cout << "Moving Folders from '" << mSlashedOldPkgName << "' to '" << mSlashedNewPkgName << "'" << std::endl;

	std::string oldPackageFullPath = mProjectEntryPath + "/" + mJavaSourceEntryPath + "/" + mSlashedOldPkgName;
	std::string newPackageFullPath = mProjectEntryPath + "/" + mJavaSourceEntryPath + "/" + mSlashedNewPkgName;

	if (FileHelper::MakePath(newPackageFullPath))
	{
		bSuccess = FileHelper::CopyAllFiles(oldPackageFullPath, newPackageFullPath, MAF_UNLINK_EMPTY_DIRS);
		
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
		std::string tempNewPackageName = RAND_PACKAGE_PREFIX + RandomHelper::getRandomSenseString(RandomHelper::getRandomInBtw(5, 8));

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
