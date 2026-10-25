#include <iostream>
#include <string>
#include "GUI.h"

#ifdef __APPLE__
	#include <unistd.h>
	#include <sys/stat.h>
#endif

using namespace std;

#ifdef __APPLE__

// check if a directory of the passed name exists
bool dirExists(const std::string& dirName_in)
{
	char buf[1024];
	string savedCwd = "./";
	char* savedCwdCh = getcwd(buf, sizeof(buf));
	if(savedCwdCh)
		savedCwd = savedCwdCh;
	string parentDir = ".";
	size_t found = dirName_in.find_last_of("/");
	if(found!=string::npos && found>0)
		parentDir = dirName_in.substr(0,found);
	chdir(parentDir.c_str());
	bool ret = false;
	struct stat sb;
	if(stat(dirName_in.c_str(), &sb)==0 && S_ISDIR(sb.st_mode))
		ret = true;
	chdir(savedCwd.c_str()); // change back cwd
	return ret;
}

#endif

int main(int argc, char *argv[])
{
	GUI gui;

	#ifdef __APPLE__
	
		// get the current working dir name from main argument
		string appPath(argv[0]);
		string appInstallPath = ".";
	
		if(appPath.at(0)=='.' && appPath.at(1)=='/')
		{
			appInstallPath = getcwd(0, (size_t)NULL);
		}
		else
		{
			size_t found = appPath.find_last_of("/");
			if(found!=string::npos)
				appInstallPath = appPath.substr(0, found);
		}

		// see if you should adjust path to ./Contents/MacOS
		if(dirExists(string(appInstallPath+"/Contents/MacOS")))
		{
			appInstallPath = appInstallPath + "/Contents/MacOS";
			chdir(appInstallPath.c_str());
			cout << "Running inside Mac app bundle, path adjusted.]n";
		}

		gui.setAppInstallPath(appInstallPath);
		cout << "App's install path ... " << appInstallPath << endl;
	
	#endif	

	// initialize GUI
	// the GUI class is the main controller of this app
	gui.initialize();

	// let the GUI do the main work
	gui.run();

	return 0;
}












