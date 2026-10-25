#ifndef DIALOG_H
#define DIALOG_H

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Kbd.h"
#include "Mouse.h"
#include "Speech.h"

#ifdef _WIN32

	#include <Windows.h>

#elif __APPLE__

	#include <unistd.h>
	#include <pwd.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <dirent.h>

#endif

const int N_ITEMS = 80;
const int FONTSIZE = 13;
const int QMESSAGE_X = 680;
const int QMESSAGE_Y = 0;
const int QMESSAGE_W = 160;
const int QMESSAGE_H = 19;

// this is mostly a copy of the 'Help' class used within GUI class...
class HelpDialog
{
public:
	
	sf::RenderWindow* w;
	
	bool active;
	sf::RectangleShape frames[N_ITEMS];
	sf::Text text[N_ITEMS];
	bool filled[N_ITEMS];
	sf::RectangleShape blackout;
	
	sf::RectangleShape quickMessageFrame;
	sf::Text quickMessageText;
	std::string strQuickMessage;
	bool quickMessageActive;
	sf::Clock quickMessageClock;
	
	sf::Text versionText;
	sf::Text authorText; // goes with version text (right below)
	bool versionTextActive;
	
	HelpDialog(){}
	~HelpDialog(){}
	
	void bindWindow(sf::RenderWindow* windowObj)
	{
		w = windowObj;
	}
	
	// initialize help items with the passed window object and font
	void initialize(const sf::Font &helpFont)
	{
		// target window object for drawing
		// w = windowObj;
		
		// initialize help screen ...
		for(int i=0; i<N_ITEMS; i++)
		{
			filled[i] = false;
			text[i].setFont(helpFont);
			text[i].setCharacterSize(FONTSIZE);
			text[i].setFillColor(sf::Color(255,220,220));
			frames[i].setFillColor(sf::Color(100,10,10));
		}
		
		// initialize quickMessageActive
		quickMessageText.setFont(helpFont);
		quickMessageText.setCharacterSize(FONTSIZE);
		quickMessageText.setFillColor(sf::Color(255,220,220));
		quickMessageFrame.setFillColor(sf::Color(100,10,10));
		
		// initialize version text (author text goes with this)
		versionText.setFont(helpFont);
		versionText.setCharacterSize(FONTSIZE);
		versionText.setFillColor(sf::Color(200,200,200));
		authorText.setFont(helpFont);
		authorText.setCharacterSize(FONTSIZE);
		authorText.setFillColor(sf::Color(200,200,200));
		deactivateVersionText(); // begin inactive...
	}
	// set text for given index
	void set(int index, const std::string &helpText, float xx, float yy)
	{
		filled[index] = true;
		text[index].setPosition(sf::Vector2f(xx, yy));
		text[index].setString(helpText);
		sf::FloatRect fRect = text[index].getGlobalBounds();
		frames[index].setSize(sf::Vector2f(fRect.width+6, fRect.height+6));
		frames[index].setPosition(sf::Vector2f(fRect.left-3, fRect.top-3));
	}
	// set blackout area
	void setBlackOut(float xx, float yy, float ww, float hh)
	{
		blackout.setFillColor(sf::Color(100,10,10));
		blackout.setSize(sf::Vector2f(ww, hh));
		blackout.setPosition(sf::Vector2f(xx, yy));
	}
	bool isActive()
	{ return active; }
	void activate()
	{ active = true; }
	void deactivate()
	{ active = false; }
	void draw()
	{
		w->draw(blackout);
		for(int i=0; i<N_ITEMS; i++)
		{
			if(filled[i])
			{
				w->draw(frames[i]);
				w->draw(text[i]);
			}
		}
	}
	
	// sets up a quick message to be displayed for a few seconds
	void setQuickMessage(std::string qText)
	{
		strQuickMessage = qText;
		activateQuickMessage();
		
		// int wWidth = w->getSize().x;
		int wWidth = 840;
		int qmFrameX;

		quickMessageText.setString(strQuickMessage);
		sf::FloatRect fRect = quickMessageText.getGlobalBounds();
		quickMessageFrame.setSize(sf::Vector2f(fRect.width + 40, fRect.height + 10));
		qmFrameX = wWidth - fRect.width - 40;
		quickMessageFrame.setPosition(sf::Vector2f(qmFrameX, 0));
		quickMessageText.setPosition(sf::Vector2f(qmFrameX + 20, 3));
		
		// reset the timer...
		quickMessageClock.restart();
	}
	
	void activateQuickMessage()
	{ quickMessageActive = true;}
	
	void deactivateQuickMessage()
	{ quickMessageActive = false; }
	
	// called by GUI class - checks if 3 seconds have passed
	// if so, deactivates
	void checkQuickMessageExpiration()
	{
		if(quickMessageClock.getElapsedTime().asSeconds() > 3.00f)
			deactivateQuickMessage();
	}
	
	// draws the quick message to the target window
	void drawQuickMessage()
	{
		w->draw(quickMessageFrame);
		w->draw(quickMessageText);
		
		// while we're at it, let's check for expiration
		checkQuickMessageExpiration();
	}
	
	// version information to display at bottom of logo
	void setVersionText(const std::string &strVersion, const std::string &strAuthor)
	{
		versionText.setString(strVersion);
		sf::FloatRect fRect = versionText.getGlobalBounds();
		versionText.setPosition(sf::Vector2f(830 - fRect.width, 80 - fRect.height));
		authorText.setString(strAuthor);
		fRect = authorText.getGlobalBounds();
		authorText.setPosition(sf::Vector2f(830 - fRect.width, 96 - fRect.height));		
	}
	void activateVersionText()
		{ versionTextActive = true; }
	void deactivateVersionText()
		{ versionTextActive = false; }
	bool versionTextIsActive()
		{ return versionTextActive; }
	
	// draw the version text with number at the bottom of logo
	void drawVersionText()
	{
		w->draw(versionText);	
		w->draw(authorText);
	}
	
	
private:

};

const int DIALOG_TEXT_HEIGHT = 13;
const int DIALOG_HIST_SIZE = 32;

class Dialog
{
	
public:

	static const std::string SL; // filepath slash separator used ('\\' for win, '/' for osx)
	static const std::string ROOT; // "C:" for win, "/" for osx
	static const std::string VOLUMES; // osx only - "/Volumes"

	// static const int DIALOG_TEXT_HEIGHT;
	// static const int DIALOG_HIST_SIZE;
	static const int TEXT_WIDTH; // DEBUG
	static const int CWD_WIDTH; // DEBUG
	static const int INPUT_WIDTH;
	static const int WINDOW_WIDTH;
	static const int WINDOW_HEIGHT;
	static const int TEXT_TOP_X;
	static const int TEXT_TOP_Y;
	static const int INPUT_TOP_X;
	static const int INPUT_TOP_Y;

	Speech* speech; // accessibility
	
	int adjustedWindowWidth;
	int adjustedWindowHeight;

	Kbd kbd;
	Mouse mouse;
	sf::RenderWindow* w;
	sf::Event event;
	sf::Clock clickTimer;
	sf::Clock fireClock;
	sf::Font font;
	sf::Font miniFont;
	sf::Color dialogGreen;
	sf::Color dialogGreenLight;
	sf::Color dialogRed;
	sf::Color dialogRedLight;
	sf::Color dialogRedDark;
	sf::Color dialogBlue;
	sf::Text itemText[DIALOG_TEXT_HEIGHT];
	sf::Text cwd;
	sf::Text supportedFilesText;
	sf::Text basicText[10];
	sf::Text basicTextColor[10];
	
	std::vector<std::string> filesFolders;
	std::vector<int> itemTypes;
	std::string installDir;
	std::string currentDir;
	std::string defaultDir;
	std::string requestedStartFolder;
	int nItems;
	std::string strView[DIALOG_TEXT_HEIGHT];
	int itemTypesView[DIALOG_TEXT_HEIGHT];
	int topRenderLine;
	int selectedIndex;
	int selectedIndexPrevious;
	
	bool upFiring;
	int upProcessedCount;
	bool downFiring;
	int downProcessedCount;
	int cursorPos;
	int startRenderPos;
	
	bool exitDialog;
	bool windowClosed;
	bool windowFocused;
	std::string fileNameToReturn;
	int dialogMode;
	bool cancelChosen;
	bool atBaseDialogLevel;
	std::string fileFilter;

	sf::RectangleShape inputLine;
	sf::Text inputLineLabel;
	sf::Text inputLineText;
	std::string strInput;
	
	sf::RectangleShape cursor;
	sf::Clock blinkClock;
	int blinkState;
	sf::RectangleShape selectMask;
	
	struct Button
	{
		int type;
		int x;
		int y;
		int w;
		int h;
		int labelYOffset;
		int labelXOffset;
		int sizeWOffset;
		int sizeHOffset;
		bool hasHelpText;
		sf::RectangleShape rect;
		sf::Text label;
		std::string strLabel;
		sf::Text help;
	};
	
	Button loadButton;
	Button cancelButton;
	Button filterButton;
	Button saveButton;
	Button exportButton;
	Button okButton;
	Button yesButton;
	Button noButton;
	Button helpButton;
	Button goButton;
	Button driveButton[32];
	Button desktopButton;
	Button documentsButton;
	Button goToCancelButton;
	
	struct Slider
	{
		int topX;
		int topY;
		int w;
		int h;
		int pos;
		int range;
		sf::RectangleShape handle;
		sf::RectangleShape rail;
	};
	
	Slider slider;
	
	int charHeight;
	int charWidth;
	
	int mouseX, mouseY;
	int mouseXprev, mouseYprev, mouseXdelta, mouseYdelta;
	bool mouseLPressed;
	bool mouseLReleased;
	int mouseWheelMoved;
	bool dragging;
	int pageClickCount;
	
	char typedChar;
	std::string strTypedChar;
	
	std::string history[DIALOG_HIST_SIZE];
	int historyLevel;
	bool undoing;
	bool refreshing;
	bool startingOut;
	
	bool yesNoDialogResult;
	bool basicDialogCancelled;
	int basicDialogMode;
	std::string basicStrView[10];
	std::string basicStrViewColor[10];
	int strX[10];
	int strY[10];
	int strColorX[10];
	int strColorY[10];
	int nLinesBasicDialog;
	int messageTopY;
	int messageBottomY;
	bool exitBasicDialog;
	sf::FloatRect pathNameRect;
	bool calledExplorerAlready;
	
	HelpDialog help;
	
	bool exitGoToDialog;
	std::string goToDialogResult;
	std::vector<std::string> drives;
	int nDrives;
	int nLinesDrive;
	float driveButtonW;
	float driveButtonH;
	float driveButtonYStep;
	float driveButtonTop;
	sf::Text drivesGroupLabel;
	sf::Text foldersGroupLabel;
	
	std::string appInstallPath; // for OSX

	Dialog();
	~Dialog();

	void initialize(const std::string &aPath);
	void setDefaultDir(const std::string &defDir);
	std::string getCurrentDir();
	std::vector<std::string> getFileNamesInDir(std::string folder, std::string filter);
	std::vector<std::string> getDirNamesInDir(std::string folder);	
	void bindWindow(sf::RenderWindow* windowObj);
	std::string getLoadFileName();
	std::string getSaveFileName();
	std::string getExportFileName();
	void runFileDialog();
	void pollEvents();
	void handleInputFileDialog();
	void drawFileDialog();
	
	void makeButton(Button* b, const std::string& lbl, int type);
	void addHelp(Button* b, std::string helpStr, int position);
	void positionButton(Button* b, int xx, int yy);
	void highlightButton(Button* b);
	void dehighlightButton(Button* b);
	void activateButton(Button* b);
	void changeButtonText(Button* b, std::string newLabel);
	bool hovering(Button* b);
	
	void makeSlider(Slider* s, int ww, int hh, int range);
	void positionSlider(Slider* s, int xx, int yy);
	bool hovering(Slider* s);
	void moveSlider(Slider* s, int delta);
	void setSliderPos(Slider* s, int p);
	float sliderPosRatio(Slider* s);
	void highlightSlider(Slider* s);
	void dehighlightSlider(Slider* s);
	
	void readDir(std::string path, std::string strFilter);
	void initializeStrView();
	void setStrView();
	std::string trimRight(std::string str, int max);
	std::string trimLeft(std::string str, int max);
	std::string trimSpaceRight(std::string str);
	
	void updateCursor();
	void blinkCursor();

	bool overwriting(std::string file);
	
	void pushHistory(std::string path);
	std::string popHistory();
	void setStartFolder(std::string path);
	void unsetStartFolder();
	
	void messageDialog(std::string message);
	bool yesNoDialog(std::string question);
	void runBasicDialog();
	void parseBasicDialog(std::string str);
	void handleInputBasicDialog();
	void drawBasicDialog();
	
	bool hoveringOnPathName();
	std::vector<std::string> getDriveLetters();
	std::vector<int> getDriveTypes(std::vector<std::string> drives);
	
	std::string goToDialog();
	void runGoToDialog();
	void handleInputGoToDialog();
	void drawGoToDialog();
	std::string getUserDocPathFromSystem();
	bool dirExists(const std::string& dirName_in);
	void setAppInstallPath(const std::string &aPath);
	template<class T>
	std::string toString(T n);
	
	void bindSpeech(Speech* sp); // accessibility
};

#endif