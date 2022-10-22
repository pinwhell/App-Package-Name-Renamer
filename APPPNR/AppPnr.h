#pragma once

#include <vector>
#include <string>

#define RAND_PACKAGE_PREFIX "com."

class Cache {
public:
	virtual bool HasCache() = 0;
	virtual bool CleanCache() = 0;
};

class AppPnr
{
protected:
	std::string mProjectEntryPath;
	std::string mJavaSourceEntryPath;
	std::string mOldPkgName;
	std::string mNewPkgName;
	std::string mSlashedOldPkgName;
	std::string mSlashedNewPkgName;
	std::string mUnderscoredOldPkgName;
	std::string mUnderscoredNewPkgName;
	bool mbReplaceSlashedPkgNames;
	bool mbReplaceUnderscoredPkgName;
	bool mbForce;

	bool HandlePackageFolders();
	void HandleCleanUp(bool bRevert);

public:
	virtual bool Init();
	virtual void Run();

	void setJavaSourceEntryPath(const std::string& _javaSourceEntryPath);
	void setProjectEntryPath(const std::string& _projEntryPath);
	void setOldPackageName(const std::string& _oldPkgName);
	void setNewPackageName(const std::string& _newPkgName);
	void setForce(bool bForce);
	void getAllNewPackagesSplit(std::vector<std::string>& outAllPackagesSplit);
	void getAllOldPackagesSplit(std::vector<std::string>& outAllPackagesSplit);

	virtual void TryFindPackageName(std::string& outPkgName) {};
	void setReplaceSlashedPackageName(bool replaceSlashedPkgName);
	void setReplaceUnderscoredPackageName(bool replaceUnderscoredPkgName);
};

class GradleAppPnr : public AppPnr , public Cache {

private:
	std::string mGradlePath;
	std::string mBuildGradlePath;
	std::string mAppIdArgName;
	std::string mAppPath;

public:
	bool Init() override;
	void Run() override;

	void TryFindPackageName(std::string& outPkgName) override;

	void setAppPath(const std::string& _appPath);
	void setBuildGradlePath(const std::string& _buildGradlePath);
	void setApplicationIdArgName(const std::string& _appIdArgName);

	bool HasCache() override;
	bool CleanCache() override;
};

