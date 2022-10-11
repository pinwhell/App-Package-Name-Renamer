#pragma once

#include <vector>
#include <string>

#define RAND_PACKAGE_PREFIX "com.your."

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

class GradleAppPnr : public AppPnr {

private:
	std::string mBuildGradlePath;
	std::string mAppIdArgName;

public:
	bool Init() override;

	void TryFindPackageName(std::string& outPkgName) override;

	void setBuildGradlePath(const std::string& _buildGradlePath);
	void setApplicationIdArgName(const std::string& _appIdArgName);
};

