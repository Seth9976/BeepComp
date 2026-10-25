#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

#ifdef _WIN32

	#include <Windows.h>
	#include <shlobj.h>
	
#elif __APPLE__

	#include <unistd.h>
	#include <pwd.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <dirent.h>
	#include <fstream>

#endif

class Config
{

public:

	static const std::string SL; // filepath slash separator used ('\\' for win, '/' for osx)
	static const std::string ROOT; // "C:" for win, "/" for osx

	bool versionDifferent;
	std::string version;
	std::string appDataPath;
	std::string userDocPath;
	std::string userdataParentPath;
	std::string userdataPath;
	std::string currentDir;
	std::string userdataPathOrigin;
	std::string configFilePath;
	std::string appInstallPath; // for OSX

	Config()
	{
		userDocPath = "";
		userdataParentPath = "";
		userdataPath = "";
		currentDir = "";
		userdataPathOrigin = "";
		configFilePath = "";
	}
	~Config(){}

	void setVersion(const std::string &v);
	std::string setup();
	
	std::string getAppDataPath();	
	std::string getCurrentDir();
	std::vector<std::string> getFileNamesInFolder(const std::string &folder);
	std::string getUserDocPathFromSystem();
	std::string getUserDocPath();
	std::string getUserdataPath();
	bool configFileExists(std::string cfgPath);
	bool readConfigFile();
	bool writeConfigFile();
	bool dirExists(const std::string& dirName_in);
	bool fileExists(const std::string& filePath);
	bool makeUninstallBatchFile(const std::string &batchFileName, const std::string &targetFolder);
	
	void setAppInstallPath(const std::string &aPath); // for OSX
};

#endif