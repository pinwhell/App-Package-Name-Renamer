#include <command_line_interface.h>
#include <iostream>
#include "AppPnr.h"
#include "RandomHelper.h"

cli_main(
	// program information
	"AppPnr",
	"v1.0",
	"Application Package Name Renamer",

	cli::FlagArgument gradle(
		"gradle",
		"present when working project is gradle"
	);

	cli::FlagArgument force(
		"force",
		"present when you need to bypass the checks and proceed to replace all"
	);

	cli::FlagArgument noReplaceSlashedPkgNames(
		"no-replace-slashed-pkg-name",
		"avoid replaces package names in slashed form ex. com/pkg/name"
	);

	cli::FlagArgument noReplaceUnderscoredPkgNames(
		"no-replace-underscored-pkg-name",
		"avoid replaces package names in undescored form ex. com_pkg_name, usually appear in JNI Native"
	);

	cli::OptionalArgument<std::string> buildGradlePathArg(
		"app/build.gradle",
		"build-gradle-path",
		"path of the main build gradle"
	);

	cli::OptionalArgument<std::string> javaSourcePathPathArg(
		"app/src/main/java",
		"java-source-path",
		"path of java source entry, used to replace folders names"
	);

	cli::OptionalArgument<std::string> gradlePkgNameArgNameArg(
		"applicationId",
		"application-id-arg-name",
		"name of the argument that refers to the package name in the build.gradle, def: 'applicationId'"
	);
	
	cli::OptionalArgument<std::string> oldPkgNameArg(
		"",
		"old-pkg-name",
		"old package name present in the app project"
	);

	cli::OptionalArgument<std::string> newPkgNameArg(
		"",
		"new-pkg-name",
		"new package name to replace in the app project"
	);

	cli::OptionalArgument<std::string> projectPath(
		".",
		"proj-path",
		"where is the root of the project located"
	);

	cli::OptionalArgument<std::string> appPathArg(
		"app/",
		"app-path",
		"where is the root of the project located"
	);

	cli::create_alias("proj-path", 's');
	cli::create_alias("new-pkg-name", 'r');
	cli::create_alias("force", 'f');
) {
	RandomHelper::InitRandomization();

	struct stat st;
	if (!stat((*projectPath).c_str(), &st))
	{
		AppPnr* pAppPnrTool = nullptr;

		if (/*gradle.is_present()*/ true) // By Default gradle
		{
			GradleAppPnr* pGradleAppPnr = (GradleAppPnr*) (pAppPnrTool = new GradleAppPnr());
			
			pGradleAppPnr->setAppPath(*appPathArg);
			pGradleAppPnr->setBuildGradlePath(*buildGradlePathArg);
			pGradleAppPnr->setApplicationIdArgName(*gradlePkgNameArgNameArg);
		}

		if (pAppPnrTool)
		{
			pAppPnrTool->setProjectEntryPath(*projectPath);
			pAppPnrTool->setJavaSourceEntryPath(*javaSourcePathPathArg);
			pAppPnrTool->setOldPackageName(*oldPkgNameArg);
			pAppPnrTool->setNewPackageName(*newPkgNameArg);
			pAppPnrTool->setReplaceSlashedPackageName(!noReplaceSlashedPkgNames.is_present());
			pAppPnrTool->setReplaceUnderscoredPackageName(!noReplaceUnderscoredPkgNames.is_present());
			pAppPnrTool->setForce(force.is_present());

			if (pAppPnrTool->Init())
				pAppPnrTool->Run();

			delete pAppPnrTool;
		}
		else printf("Not Project type Selected\n");

	}
	else printf("Invalid Project Path\n");
	std::cout << "Press Any key to Exit.";
	getchar();

} end_cli_main