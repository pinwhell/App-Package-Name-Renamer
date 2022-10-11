# App Package Name Renamer

## About
This Software basicly goes throw each one of your files in your project, and replaces the old package name by the new package name, this program supports differents format of package names ex. com/your/package, com.your.package, com_your_package

## Getting Started
Clone this project and open it using Visual Studio.

### Prerequisites

Visual Studio

## Usage
BEFORE USING THIS SOFTWARE IN YOUR PROJECT MAKE A BACKUP, also make sure you dont have any other program using the files, or it can interfere with this program.

Most simple example:

```
AppPnr -s [your_project_root] 
ex.
AppPnr -s Packge/Here/MyPackageName
```

To define a new package name:
```
-r [your_new_package_name]
```
When the new package name is not defined, then a random one will be created automaticly.

To manually define the old package name:
```
-old-pkg-name [old_package_name]
```
If you dont input an old package name, the software will find it automaticly, inside the build.grade taking that info from the argument 'applicationId', also notice that if this argument name is not this default one, you will need to manually define it with:

```
--application-id-arg-name [new_applicationId_arg_name]       # this is the arg containing the current package name inside the build.gradle
```

Example:
```
AppPnr -s Packge/Here/MyPackageName -r com.mynew.package
```
For projects that doesnt have the standart default path to the java source root, this is used for the automatic rename of the package directories, you will need to manually define it, using:
```
--java-source-path [new_path_relative_from_root] # Default is 'app/src/main/java'
```
same with the build.gradle, if not in default path 'app/build.gradle', you will need to redefine its location, to get automatic old packet name, using:
```
--build-gradle-path [new_path_to_build.gradle] # Default is 'app/build.gradle'
```
the help display:
```
AppPnr -h
Usage: AppPnr [OPTIONS...]
OPTIONS:
        --gradle: present when working project is gradle
        -f, --force: present when you need to bypass the checks and proceed to replace all
        --old-pkg-name: old package name present in the app project
        --no-replace-underscored-pkg-name: avoid replaces package names in undescored form ex. com_pkg_name, usually appear in JNI Native
        --no-replace-slashed-pkg-name: avoid replaces package names in slashed form ex. com/pkg/name
        --java-source-path: path of java source entry, used to replace folders names
        --build-gradle-path: path of the main build gradle
        --application-id-arg-name: name of the argument that refers to the package name in the build.gradle, def: 'applicationId'
        -r, --new-pkg-name: new package name to replace in the app project
        -h, --help: print this help message
        -s, --proj-path: where is the root of the project located
        -v, --version: print program version
```
