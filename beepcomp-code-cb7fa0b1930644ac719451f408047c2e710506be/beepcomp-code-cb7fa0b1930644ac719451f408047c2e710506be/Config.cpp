#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "Config.h"

#ifdef _WIN32
	
	const std::string Config::SL = "\\";
	const std::string Config::ROOT = "C:\\";	
	
#elif __APPLE__

	const std::string Config::SL = "/";
	const std::string Config::ROOT = "/";

#endif

using namespace std;

// takes the version number from GUI class
// if installed version is different from the number written in config file...
// then we're updating the program - we should rewrite everything in userdata folder!
void Config::setVersion(const std::string &v)
{
	version = v;
}

// this is used to do initial setup of BeepComp USERDOC userdata folder
// if config file is not found in the installation directory that means program is being run
// for the first time <- if so, copy all the txt files in userdata origin to USERDOC destination

std::string Config::setup()
{
	versionDifferent = false;

	bool configFileFound = false;
	bool configFileValid = false;
	bool firstTime = false;
	bool setupDone = false;
	string strSetupResult = "";
	
	// get currentDir path (executable install path)
	currentDir = getCurrentDir(); // current directory
	cout << "currentDir = " << currentDir << endl;
	
	// determine where config file should be saved
	// WINDOWS -> get ENV's appdata path PLUS 'BeepComp'
	// OSX -> the parent folder of the executable (easy!)
	appDataPath = getAppDataPath();

	// following procedure APPDATA folder creation is for WINDOWS only
	
	#ifdef _WIN32
	
		bool appDataCreateResult;
		//
		// create the parent folder in appdata
		//
		appDataCreateResult = CreateDirectory(appDataPath.c_str(), NULL);
		cout << "Create %APPDATA%\\BeepComp folder:  " << appDataPath << endl;
	
		// success-fail check
		if(appDataCreateResult)
			cout << "success!\n";
		else
		{
			cout << "fail.\n";
			if(GetLastError()==ERROR_ALREADY_EXISTS) cout << "The folder already exists.\n";
		}

	#elif __APPLE__

		bool appDataCreateResult;
		//
		// create the parent folder in appdata
		//
		//	create directory! at userdataParentPath unix style
		int intResAppFolder = mkdir(appDataPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
		appDataCreateResult = (intResAppFolder>=0); // negative means fail

		cout << "Create ~/Library/Application Support/BeepComp folder:\n" << appDataPath << endl;
	
		// success-fail check
		if(appDataCreateResult)
			cout << "success!\n";
		else
		{
			cout << "fail.\n";
			if(dirExists(appDataPath)) cout << "Folder already exists.\n";
		}		
	
	#endif
	
	// now check for the config file's existance
	cout << "check if beepcomp.config exists...\n";
	configFilePath = appDataPath + SL + "beepcomp.config";
	cout << "configFilePath = " << configFilePath << endl;
	
	// if config file found ... get Path information from that
	configFileFound = configFileExists(configFilePath);

	
	// if config file found -> the program has already run once after installation	
	// we will check for the validity of config file...
	
	// if config file was found, let's try to get path data from it 
	if(configFileFound)
	{
		cout << "Config file exists, reading contents and check data validity...\n";
		configFileValid = readConfigFile();
		
		// read config file ... you get boolean result for data validity
		if(configFileValid)
		{
			cout << "Config file's data is good!.\n";
			cout << "Path information is now:\n";
			cout << "userDocPath: " << userDocPath << endl;
			cout << "userdataParentPath: " << userdataParentPath << endl;
			cout << "userdataPath: " << userdataPath << endl;
			setupDone = true; // congrats, all done here
		}
		else
			cout << "Config file's data is NOT good...\n";
	}
	// config file NOT found. first time here
	else
	{
		cout << "Config file NOT found. first time here...\n";
		firstTime = true;
	}
	
	if(versionDifferent)
		cout << "Installed version is different - we should overwrite userdata.\n";
	
	// if config file is found and also its data is valid... 
	// we have read all the valid path information - we will exit without setup
	if(setupDone)
	{
		strSetupResult = "SETUP SKIPPED - Config file found and is valid";
		return strSetupResult;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	
	// File not found - means program is being run for the first time after installation
	// - will do initial setup!
	
	setupDone = false;
	if(firstTime)
	{
		cout << "Config file does not exist. Must be fresh after installation!\n";
		strSetupResult = "Initial setup - ";
	}
	else
	{
		cout << "Config file corrupt - need to rebuild! (or possibly dest folders erased?)\n";
		strSetupResult = "Rebuild corrupt config file (or reconstructing dest folders) - ";
	}

	// get the userDoc path from env var
	userDocPath = getUserDocPathFromSystem();
	cout << "User's 'Documents' path = " << userDocPath << endl;
	
	// if this EVER fails... safeguard: set it to USERPROFILE folder...
	if(userDocPath.empty())
	{
		#ifdef _WIN32
			userDocPath = string(getenv("USERPROFILE"));
		#elif __APPLE__
			//
			//
			//	TODO
			//	Is there a way to get user path by getenv in OSX?
			//
		#endif
		
		cout << "User's Documents path corrected to USERPROFILE folder:\n";
		cout << userDocPath << endl;
	}
	
	bool result;
	
	//
	// create the parent folder in userdata
	//
	userdataParentPath = userDocPath + SL + "BeepComp";
	
	#ifdef _WIN32
	
		result = CreateDirectory(userdataParentPath.c_str(), NULL);
		
	#elif __APPLE__

		//	create directory! at userdataParentPath unix style
		int intResult = mkdir(userdataParentPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
		result = (intResult>=0); // negative means fail

	#endif
	
	cout << "Create parent folder:  " << userdataParentPath << endl;

	// success-fail check
	if(result)
		cout << "success!\n";
	else
	{
		cout << "fail.\n";
		
		#ifdef _WIN32
			if(GetLastError()==ERROR_ALREADY_EXISTS) cout << "The folder already exists.\n";
		#elif __APPLE__
			if(dirExists(userdataParentPath))
				cout << "The folder already exists.\n";
		#endif
	}	
	
	//
	// create the userdata folder in the parent folder
	//
	userdataPath = userdataParentPath + SL + "userdata";
	string exportFolderPath = userdataPath + SL + "export";
	
	#ifdef _WIN32
	
		result = CreateDirectory(userdataPath.c_str(), NULL);

		// extra step .. create "export" folder inside userdata
		result = CreateDirectory(exportFolderPath.c_str(), NULL);		
		
	#elif __APPLE__

		//	create directory! at userdataParentPath unix style
		int intResMkdir = mkdir(userdataPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

		// extrastep for osx.. create "exports" folder inside userdata
		int intResMkdir2 = mkdir(exportFolderPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
		result = ( (intResMkdir>=0) | (intResMkdir2>=0) ); // negative means fail
	
	#endif
	
	cout << "Create userdata folder:  " << userdataPath << endl;
	cout << "Create userdata/export folder:  " << exportFolderPath << endl;

	// success-fail check
	if(result)
		cout << "success!\n";
	else
	{
		cout << "fail.\n";
		#ifdef _WIN32
			if(GetLastError()==ERROR_ALREADY_EXISTS) cout << "The folder already exists.\n";
		#elif __APPLE__
			if(dirExists(userdataPath) || dirExists(exportFolderPath))
				cout << "One or more of these folders already exists.\n";
		#endif
	}
	
	// now get ready to copy all the txt files to destination...
	cout << "Okay, first time here. Let's set up your userdata folder...\n";
	
	// get ready to copy all the txt files from userdata source
	userdataPathOrigin = currentDir + SL + "userdata";
	vector<string> dataFiles = getFileNamesInFolder(userdataPathOrigin);
	
	int nFiles = dataFiles.size();
	
	cout << "Listing files to copy...\n";
	for(int i=0; i<nFiles; i++)
	{
		if(dataFiles[i].find(".txt")!=string::npos) // list only .txt files
			cout << dataFiles[i] << endl;
	}
	
	// now copy all these files to to the newly created userdata directory!
	for(int i=0; i<nFiles; i++)
	{
		string source = userdataPathOrigin + SL + dataFiles[i];
		string destination = userdataPath + SL + dataFiles[i];

		if(source.find(".txt")!=string::npos) // only for files with .txt extension
		{
			cout << "Copying...\n";
			cout << "From: " << source << endl;
			cout << "To:   " << destination << endl;
			
			#ifdef _WIN32
			
				int copyResult;
				copyResult = CopyFile(source.c_str(), destination.c_str(), FALSE); // will force overwrite
			
				if(copyResult==0)
					cout << "...failed\n";
				else
					cout << "...success!\n";
				
			#elif __APPLE__
	
				// copy using std library
	
				ifstream sourceFile(source.c_str(), std::ifstream::binary);
				ofstream destinationFile(destination.c_str(), std::ofstream::binary | std::ofstream::trunc);
				bool allOkay = true;
		
				if(sourceFile.fail())
				{
					cout << "Error opening: " << source << endl;
					allOkay = false;
				}
				if(destinationFile.fail())
				{
					cout << "Error accessing: " << destination << endl;
					allOkay = false;
				}
		
				if(allOkay)
					destinationFile << sourceFile.rdbuf();
			
				if(sourceFile.fail()||destinationFile.fail())
				{
					cout << "something went wrong in the copying process..?? (check)\n";
					allOkay = false;
				}
		
				sourceFile.close();
				destinationFile.close();
	
			#endif
		}
	}
	
	// write all path information to config file...
	
	result = writeConfigFile();
	if(!result)
	{
		cout << "Unable to write: " << configFilePath << endl;
		strSetupResult += "error writing config file";
	}
	else
	{
		cout << "beepcomp.config file created!\n";
		strSetupResult += "SUCCESS";
	}
	
	return strSetupResult;
}

// get current directory
std::string Config::getCurrentDir()
{
	#ifdef _WIN32
	
		char buffer[MAX_PATH];
		GetModuleFileName( NULL, buffer, MAX_PATH );
		string::size_type pos = string( buffer ).find_last_of( "\\/" );
		return string( buffer ).substr( 0, pos);
	
	#elif __APPLE__
	
		return appInstallPath;
	
	#endif	
}

// get all file names in folder
vector<std::string> Config::getFileNamesInFolder(const std::string &folder)
{
    vector<string> names;
	
	#ifdef _WIN32
	
    char search_path[200];
    sprintf(search_path, "%s/*.*", folder.c_str());
    WIN32_FIND_DATA fd; 
    HANDLE hFind = ::FindFirstFile(search_path, &fd); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names.push_back(fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    }
	
	#elif __APPLE__
	
		chdir(folder.c_str());
		DIR *dp;
		struct dirent *ep;
		dp = opendir(string(folder).c_str());
	
		if(dp!=NULL)
		{
			while( (ep=readdir(dp)) )
			{
				struct stat buf;
				stat(ep->d_name, &buf);
				if(S_ISREG(buf.st_mode))
					names.push_back(string(ep->d_name));
			}
			closedir(dp);
		}
		else
			cout << "Error opening directory: " << folder << endl;
		chdir(appInstallPath.c_str());
	#endif
	
    return names;
}


// get current user's documents path from system...
std::string Config::getUserDocPathFromSystem()
{
	#ifdef _WIN32
	
		CHAR my_documents[MAX_PATH];
		HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
	
		string strResult = "";
		
		if (result != S_OK)
		{
			cout << "Error getting the User Documents path...\n";
			return strResult;
		}
		stringstream ss;
		ss << my_documents;
		strResult = ss.str();
		return strResult;
	
	#elif __APPLE__
	
		// osx specific ... get current user's home directory
		string userHomeDir;
		struct passwd* pwd = getpwuid(getuid());
		userHomeDir = string(pwd->pw_dir);
		
		// then you know the user's Documents folder
		string userDocumentsDir = userHomeDir + SL + "Documents";
		
		return userDocumentsDir;		
	
	#endif
	
}

// WINDOWS -> get the appdata path plus 'BeepComp'
// OSX -> set to the parent folder of executable
std::string Config::getAppDataPath()
{
	#ifdef _WIN32
	
		string pathToReturn = string(getenv("APPDATA"));
		pathToReturn += SL + "BeepComp";
		return pathToReturn;
		
	#elif __APPLE__
	
		string pathToReturn = string(getenv("HOME")) + SL + "Library/Application Support/BeepComp";
		return pathToReturn;
		
	#endif
}

// get current user's documents path
std::string Config::getUserDocPath()
{
	return userDocPath;
}

std::string Config::getUserdataPath()
{
	return userdataPath;
}

// checks if config file is present in current dir
bool Config::configFileExists(std::string cfgPath)
{
	bool exists = false;
	
	#ifdef _WIN32
	
		GetFileAttributes(cfgPath.c_str()); // from winbase.h
		if(INVALID_FILE_ATTRIBUTES == GetFileAttributes(cfgPath.c_str()) && GetLastError()==	ERROR_FILE_NOT_FOUND)
		{
			//File not found
			exists = false;
		}
		else
		{
			// File found!
			exists = true;
		}
	
	#elif __APPLE__
	
		string parentDir = ".";
		size_t found = cfgPath.find_last_of("/");
		if(found!=string::npos && found>0)
			parentDir = cfgPath.substr(0,found);
		chdir(parentDir.c_str());
		struct stat sb;
		if(stat(cfgPath.c_str(), &sb)==0 && S_ISREG(sb.st_mode))
			exists = true;
		chdir(appInstallPath.c_str()); // change cwd back..
			
	#endif
	
	return exists;
}

// read path data from config file...
bool Config::readConfigFile()
{
	ifstream inFile;
	inFile.open(configFilePath.c_str());
	string line;
	
	string tempAppDataPath = "";
	string tempDataParentPath = "";
	string tempUserdataPath = "";
	string tempVersion = "";
	size_t found;
	
	if(inFile.is_open())
	{
		// read a line from file, and try parsing
		while( getline(inFile, line) )
		{
			int lineLen = line.length();
			int searchStrLen = 0;
			int nCharsToRead = 0;
			
			string searchStr;
			searchStr = "USERDOCPATH=";
			
			found = line.find(searchStr);
			if(found!=string::npos) // found!
			{
				// extract the data part following '='
				searchStrLen = searchStr.length();
				nCharsToRead = lineLen - searchStrLen;
				tempAppDataPath = line.substr(searchStrLen, nCharsToRead);
				
				cout << "USERDOCPATH found:\n" << tempAppDataPath << endl;
			}
			
			searchStr = "USERDATAPARENTPATH=";
			found = line.find(searchStr);
			if(found!=string::npos) // found!
			{
				// extract the data part following '='
				searchStrLen = searchStr.length();
				nCharsToRead = lineLen - searchStrLen;
				tempDataParentPath = line.substr(searchStrLen, nCharsToRead);
				
				cout << "USERDATAPARENTPATH found:\n" << tempDataParentPath << endl;
			}
			
			searchStr = "USERDATAPATH=";

			found = line.find(searchStr);
			if(found!=string::npos) // found!
			{
				// extract the data part following '='
				searchStrLen = searchStr.length();
				nCharsToRead = lineLen - searchStrLen;
				tempUserdataPath = line.substr(searchStrLen, nCharsToRead);
				
				cout << "USERDATAPATH found:\n" << tempUserdataPath << endl;
			}

			searchStr = "VERSION=";

			found = line.find(searchStr);
			if(found!=string::npos) // found!
			{
				// extract the data part following '='
				searchStrLen = searchStr.length();
				nCharsToRead = lineLen - searchStrLen;
				tempVersion = line.substr(searchStrLen, nCharsToRead);
				
				cout << "VERSION found:\n" << tempVersion << endl;
			}			

			
		}
	}
	else
	{
		cout << "Error opening config file: " << configFilePath << endl;
		return false;
	}
	
	// file op done - close
	inFile.close();
	
	// let's check the data validity...
	
	bool allDataValid = false;
	bool userDocPathValid, userdataParentPathValid, userdataPathValid;
	// bool versionCurrent;
	
	userDocPathValid = dirExists(tempAppDataPath);
	userdataParentPathValid = dirExists(tempDataParentPath);
	userdataPathValid = dirExists(tempUserdataPath);
	
	if(tempUserdataPath.find("userdata")==string::npos)
	{
		cout << "USERDATAPATH does not contain the world 'userdata'!\n";
		userdataPathValid = false;
	}
	
	if(version==tempVersion)
	{
		cout << "Installed version and version number in config match!\n";
		versionDifferent = false;
	}
	else
		versionDifferent = true;
	
	// check validity of all path data... if good, go ahead and rewrite.
	if(userDocPathValid && userdataParentPathValid && userdataPathValid && !versionDifferent)
	{
		allDataValid = true;
		userDocPath = tempAppDataPath;
		userdataParentPath = tempDataParentPath;
		userdataPath = tempUserdataPath;
		version = tempVersion;
	}
	else // not valid, set all paths to empty
	{
		allDataValid = false;
		userDocPath = "";
		userdataParentPath = "";
		userdataPath = "";
	}
	
	return allDataValid;
}

// write path data from config file... based on currently set path variables
bool Config::writeConfigFile()
{
	string userDocPathField = "USERDOCPATH=" + userDocPath;
	string userdataParentPathField = "USERDATAPARENTPATH=" + userdataParentPath;
	string userdataPathField = "USERDATAPATH=" + userdataPath;
	string versionField = "VERSION=" + version;

	// create the config file now...
	ofstream outFile;
	outFile.open(configFilePath.c_str(), ios::trunc);
	
	if (outFile.is_open())
	{
		outFile << userDocPathField << endl;
		outFile << userdataParentPathField << endl;
		outFile << userdataPathField << endl;
		outFile << versionField << endl;
		outFile.close();
	}
	else
		return false; // unable to open config file!

	return true;
}

// check if a directory of the passed name exists
bool Config::dirExists(const std::string& dirName_in)
{
	#ifdef _WIN32
	
		DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
		if (ftyp == INVALID_FILE_ATTRIBUTES)
			return false;  //something is wrong with your path!
	
		if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
			return true;   // this is a directory!
	
		return false;    // this is not a directory!

	#elif __APPLE__
	
		string parentDir = ".";
		size_t found = dirName_in.find_last_of("/");
		if(found!=string::npos && found>0)
			parentDir = dirName_in.substr(0,found);
		chdir(parentDir.c_str());
		bool ret = false;
		struct stat sb;
		if(stat(dirName_in.c_str(), &sb)==0 && S_ISDIR(sb.st_mode))
			ret = true;
		chdir(appInstallPath.c_str()); // change cwd back..
		return ret;

	#endif
}

bool Config::fileExists(const std::string& filePath)
{
	#ifdef _WIN32
	
	#elif __APPLE__

		string parentDir = ".";
		size_t found = filePath.find_last_of("/");
		if(found!=string::npos && found>0)
			parentDir = filePath.substr(0,found);
		chdir(parentDir.c_str());
		bool ret = false;
		struct stat sb;
		if(stat(filePath.c_str(), &sb)==0 && S_ISREG(sb.st_mode))
			ret = true;
		chdir(appInstallPath.c_str()); // change cwd back..
		return ret;
	
	#endif
}

// make a batch file to let user completely remove the ghost folder
bool Config::makeUninstallBatchFile(const std::string &batchFileName, const std::string &targetFolder)
{
	ofstream outFile;
	outFile.open(batchFileName.c_str(), ios::trunc);
	
	if(!outFile.is_open())
	{
		cout << "Error opening: " << batchFileName << endl;
		return false;
	}
	
	string strToWrite = "@RD /S /Q \"" + targetFolder + "\"";
	outFile << strToWrite << endl;
	outFile.close();
	
	return true;
}

// for OSX... app's path comes from argv to main func
void Config::setAppInstallPath(const std::string &aPath)
{
	appInstallPath = aPath;
}