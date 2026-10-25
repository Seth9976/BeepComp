// GUI.h //////////////////////////////////
// GUI class - definition /////////////////

#ifndef GUI_H
#define GUI_H

#ifdef _WIN32
	#include <Windows.h>
#elif __APPLE__
	#include <unistd.h>
	#include <sys/types.h>
	#include <signal.h>
	#include <stdio.h>
	#include <fstream>
	#include <sstream>
#endif

#include <string>
#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <SFML/Main.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp> 
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include "MPlayer.h"
#include "MML.h"
#include "Kbd.h"
#include "Mouse.h"
#include "Dialog.h"
#include "Button.h"
#include "Knob.h"
#include "Config.h"
#include "Speech.h"

// class MPlayer;
// class MML;
// class Dialog;
// class Button;
// class Knob;
// class Config;

class Meter
{

public:

	static const int N_STRIPS = 10;

	sf::RectangleShape strip[10];

	int panelWidth;
	int panelHeight;
	int panelX;
	int panelY;
	int stripXStep;
	int stripWidth;
	float levelLimit;

	Meter()
	{}

	~Meter()
	{}

	// sets up where and how big the meter panel will be
	void initialize(int x, int y, int w, int h, float limit)
	{
		panelX = x;
		panelY = y;
		panelWidth = w;
		panelHeight = h;
		stripXStep = panelWidth / N_STRIPS;
		stripWidth = (panelWidth * 0.9 / N_STRIPS);
		levelLimit = limit;

		// default coloring for now...
		for(int i=0; i<N_STRIPS-1; i++)
		{
			strip[i].setFillColor(sf::Color(10,10,200));
		}
		strip[N_STRIPS-1].setFillColor(sf::Color(190,10,130));
	}

	// sets up a specific channel-gain meter rectangle GUI can use to display
	void set(int channel, float value)
	{
		value = std::max(0.0f, std::min(levelLimit, value));
		int sHeight = static_cast<int>(panelHeight * (value / levelLimit));
		strip[channel].setPosition(panelX + stripXStep * channel, panelY + panelHeight - sHeight);
		strip[channel].setSize(sf::Vector2f(stripWidth, sHeight));
	}

	// when drawing meters...
	// GUI should access strip[] member of Meter class instance
	// these are sf::RectangleShapes - easy to draw!

private:

};

const int HELP_N_ITEMS = 80;
const int HELP_FONTSIZE = 13;
const int HELP_QMESSAGE_X = 680;
const int HELP_QMESSAGE_Y = 0;
const int HELP_QMESSAGE_W = 160;
const int HELP_QMESSAGE_H = 19;

class Help
{
public:

	//static const int HELP_N_ITEMS;
	//static const int HELP_FONTSIZE;
	//static const int HELP_QMESSAGE_X;
	//static const int HELP_QMESSAGE_Y;
	//static const int HELP_QMESSAGE_W;
	//static const int HELP_QMESSAGE_H;
	
	sf::RenderWindow* w;
	
	bool active;
	sf::RectangleShape frames[HELP_N_ITEMS];
	sf::Text text[HELP_N_ITEMS];
	bool filled[HELP_N_ITEMS];
	sf::RectangleShape blackout;
	
	sf::RectangleShape quickMessageFrame;
	sf::Text quickMessageText;
	std::string strQuickMessage;
	bool quickMessageActive;
	sf::Clock quickMessageClock;
	
	sf::Text versionText;
	sf::Text authorText; // goes with version text (right below)
	bool versionTextActive;
	
	Help(){}
	~Help(){}
	
	// initialize help items with the passed window object and font
	void initialize(sf::RenderWindow* windowObj, const sf::Font &helpFont)
	{
		// target window object for drawing
		w = windowObj;
		
		// initialize help screen ...
		for(int i=0; i<HELP_N_ITEMS; i++)
		{
			filled[i] = false;
			text[i].setFont(helpFont);
			text[i].setCharacterSize(HELP_FONTSIZE);
			text[i].setFillColor(sf::Color(255,220,220));
			frames[i].setFillColor(sf::Color(100,10,10));
		}
		
		// initialize quickMessageActive
		quickMessageText.setFont(helpFont);
		quickMessageText.setCharacterSize(HELP_FONTSIZE);
		quickMessageText.setFillColor(sf::Color(255,220,220));
		quickMessageFrame.setFillColor(sf::Color(100,10,10));
		
		// initialize version text (author text goes with this)
		versionText.setFont(helpFont);
		versionText.setCharacterSize(HELP_FONTSIZE);
		versionText.setFillColor(sf::Color(200,200,200));
		authorText.setFont(helpFont);
		authorText.setCharacterSize(HELP_FONTSIZE);
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
		for(int i=0; i<HELP_N_ITEMS; i++)
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

const float BAR_X = 686.0;
const float BAR_Y = 214.0;
const float BAR_W = 140.0;
const float BAR_H = 4.0;
const float MARKER_W = 12.0;
const float MARKER_H = 12.0;

class Progress
{

public:

	//static const float BAR_X = 686.0;
	//static const float BAR_Y = 214.0;
	//static const float BAR_W = 140.0;
	//static const float BAR_H = 4.0;
	//static const float MARKER_W = 12.0;
	//static const float MARKER_H = 12.0;

	sf::RenderWindow* w;
	sf::RectangleShape bar;
	sf::ConvexShape marker;
	float progressRatio;
	float markerX;
	float markerY;
	
	Progress()
	{}
	~Progress()
	{}
	
	void initialize(sf::RenderWindow* wObj)
	{
		// target window object
		w = wObj;
		
		progressRatio = 0.0f;
		
		// set up the bar rectangle
		bar.setSize(sf::Vector2f(BAR_W, BAR_H));
		bar.setPosition(sf::Vector2f(BAR_X, BAR_Y));
		bar.setFillColor(sf::Color(30,30,190));
		
		// set up the marker triangle
		markerY = BAR_Y;
		marker.setPointCount(3);
		marker.setFillColor(sf::Color(30,30,190));
		
		progressRatio = 0.0f;
		markerX = BAR_X + (BAR_W * progressRatio);
		marker.setPoint(0, sf::Vector2f(markerX, markerY));
		marker.setPoint(1, sf::Vector2f(markerX - MARKER_W/2, markerY - MARKER_H));
		marker.setPoint(2, sf::Vector2f(markerX + MARKER_W/2, markerY - MARKER_H));
	}
	
	// update the marker position
	void update(float newProgressRatio)
	{
		progressRatio = newProgressRatio;
		if(progressRatio>1.0f) progressRatio = 1.0f;
		markerX = BAR_X + (BAR_W * progressRatio);
		marker.setPoint(0, sf::Vector2f(markerX, markerY));
		marker.setPoint(1, sf::Vector2f(markerX - MARKER_W/2, markerY - MARKER_H));
		marker.setPoint(2, sf::Vector2f(markerX + MARKER_W/2, markerY - MARKER_H));
	}
	
	void draw()
	{
		w->draw(bar);
		w->draw(marker);
	}

private:
	
};

const float TWOPI = 6.28318530718;
const int N_STARS = 50;
const int WINDOW_WIDTH = 840;
const int WINDOW_HEIGHT = 640;

#ifdef _WIN32
	const std::string SLOMAKE = "\\";
#elif __APPLE__
	const std::string SLOMAKE = "/";
#endif
	
class Omake
{

public:
	
	//static const float TWOPI = 6.28318530718;
	//static const int N_STARS = 50;
	//static const int WINDOW_WIDTH = 840;
	//static const int WINDOW_HEIGHT = 640;
	std::string currentDir;
	sf::RectangleShape stars[N_STARS];
	sf::Sprite fighter[2];
	sf::Texture fighterTexture[2];
	sf::RectangleShape laser;
	float sx[N_STARS];
	float sy[N_STARS];
	sf::RenderWindow* w;
	float scrollSpeed;
	bool active;
	float fx, fy;
	float fxDeviation, fyDeviation;
	int fighterState;
	float rad, radY;
	float lx, ly;
	int shootTimer;
	sf::Clock animationClock;
	float currentTime;
	float lastTime;
	float elapsed;
	float fps;
	float adjustRatio;
	float fighterStateDelta;
	
	Omake(){}
	~Omake(){}

	void initialize(sf::RenderWindow* wObj)
	{
		lastTime=0;
		currentTime=0;
		elapsed=0;
		fps=0;
		adjustRatio = 1.0f;
		fighterStateDelta = 0;
		animationClock.restart();
		active = false;
		fighterState = 0;
		fx = 350 - 36; // fighter dimention (72x88)
		fy = -88;
		
		// RenderWindow to draw animation to
		w = wObj;
		
		// initialize random seed
		srand(time(NULL));
		
		// load fighter images
		std::string fighter1FilePath = currentDir + SLOMAKE + "images" + SLOMAKE + "fighter_1.png";
		if(!fighterTexture[0].loadFromFile(fighter1FilePath))
			std::cout << "fighter_1.png: load error!\n";
		std::string fighter2FilePath = currentDir + SLOMAKE + "images" + SLOMAKE + "fighter_2.png";
		if(!fighterTexture[1].loadFromFile(fighter2FilePath))
			std::cout << "fighter_2.png: load error!\n";
		fighter[0].setTexture(fighterTexture[0]);
		fighter[1].setTexture(fighterTexture[1]);
		
		// initialize laser
		laser.setSize(sf::Vector2f(6, 70));
		laser.setFillColor(sf::Color(170, 110, 230));
		
		// initialize stars with random colors and positions
		for(int i=0;i<N_STARS;i++)
		{	
			stars[i].setSize(sf::Vector2f(4,4));
			int r = rand() % 256;
			int g = rand() % 256;
			int b = rand() % 256;
			stars[i].setFillColor(sf::Color(r,g,b));
			sx[i] = rand() % WINDOW_WIDTH;
			sy[i] = rand() % WINDOW_HEIGHT;
			stars[i].setPosition(sf::Vector2f(sx[i],sy[i]));
		}
		resetStars();
		resetFighter();
	}
	
	void setCurrentDir(const std::string &cDir)
	{
		currentDir = cDir;
	}
	
	void resetStars()
	{ scrollSpeed = 0; }
	
	void updateStars()
	{
		scrollSpeed += 0.15f * adjustRatio;
		if(scrollSpeed > (22.0f * adjustRatio))
			scrollSpeed = 22.0f * adjustRatio;
		for(int i=0; i<N_STARS; i++)
		{
			sy[i] += scrollSpeed;
			if(sy[i]>WINDOW_HEIGHT)
				sy[i] -= WINDOW_HEIGHT;
			stars[i].setPosition(sf::Vector2f(sx[i],sy[i]));			
		}
	}
	
	void resetFighter()
	{
		fx = 350 - 36;
		fy = -140.0f;
		fxDeviation = -250.0f;
		fyDeviation = 0;
		rad = TWOPI / 2.0f;
		radY = 0;
		shootTimer = rand() % 120; // keeps track of shoot timing
		lx = -10; // set to out of screen (no show)
		ly = -100;
	}
	
	void updateFighter()
	{
		// move fighter
		if(fy<470)
			fy+= 7.0f * adjustRatio;
		fxDeviation = cos(rad) * 250.0f;
		rad += 0.06f * adjustRatio;
		if(rad > TWOPI) rad -= TWOPI;
		fyDeviation = sin(radY) * 40.0f;
		radY += 0.074f * adjustRatio;
		if(radY > TWOPI) radY -= TWOPI;

		// timer determines laser's shooting time
		if(shootTimer <= 0)
		{
			shootTimer = rand() % 100;
			lx = (fx + fxDeviation) + 33.0f;
			ly = fy;
		}
		ly = ly - 25.0f * adjustRatio;
		
		// laser's y pos, if reached top screen, only then advance timer
		if(ly <= -100)
		{
			ly = -100;
			shootTimer--;
		}

		// flip fighter's character image, set position
		fighterStateDelta += elapsed;
		if(fighterStateDelta > 0.1f)
		{
			fighterStateDelta = 0;
			fighterState++;
		}
		if(fighterState>=2) fighterState = 0;
		// fx = 400; fy = 400;
		fighter[0].setPosition(sf::Vector2f(fx + fxDeviation, fy + fyDeviation));
		fighter[1].setPosition(sf::Vector2f(fx + fxDeviation, fy + fyDeviation));
		
		// set laser's position
		laser.setPosition(sf::Vector2f(lx, ly));
	}
	
	void draw()
	{
		currentTime = animationClock.getElapsedTime().asSeconds();
		elapsed = std::min(.1f, (currentTime - lastTime));
		fps = 1.f / elapsed;
		adjustRatio = elapsed / .02f;
		lastTime = currentTime;
		
		updateStars();
		updateFighter();
		
		// now, draw
		updateStars();
		for(int i=0; i<N_STARS; i++)
			w->draw(stars[i]);		
		w->draw(laser); // draw laster, too
		w->draw(fighter[fighterState]);		
	}
	
	void activate()
	{ active = true; }
	void deactivate()
	{ 
		active = false; 
		resetStars();
		resetFighter();
	}
	bool isActive()
	{ return active; }
};

const int TEXT_HEIGHT = 24;
const int MAX_UNDO_LEVEL = 32;

class GUI
{

public:

	static const double BC_SAMPLE_RATE;
	// static const int MAX_UNDO_LEVEL;
	static const double WINDOW_WIDTH;
	static const double WINDOW_HEIGHT;
	static const int TEXT_WIDTH;
	// static const int TEXT_HEIGHT;
	static const int TEXT_TOP_X;
	static const int TEXT_TOP_Y;
	static const int PANEL_TOP_X;
	static const int PANEL_TOP_Y;
	static const std::string STR_VERSION;
	static const std::string SL; // filepath slash separator used ('\\' for win, '/' for osx)
	static const std::string ROOT; // "C:" for win, "/" for osx
	Config config;

	sf::RenderWindow window;
	sf::RenderWindow* wPtr;
	sf::Image icon;
	sf::Texture logoTexture;
	sf::Sprite logo;
	
	Knob knob;
	Button playButton;
	Button pauseButton;
	Button rewindButton;
	Button forwardButton;
	Button keyButton;
	Button docButton;
	Button dlyButton;
	Button newButton;
	Button loadButton;
	Button saveButton;
	Button exportButton;
	
	sf::Event event;
	sf::Font font;
	sf::Font miniFont;
	sf::Text text[TEXT_HEIGHT];
	sf::Text helpText;
	sf::RectangleShape cursor;
	sf::RectangleShape highlighter[TEXT_HEIGHT];
	sf::Clock fireClock;
	// sf::Time fireClElapsed;
	sf::Clock blinkClock;
	sf::Clock meterClock;
	sf::Clock autoSaveClock;
	sf::Clock audioCheckClock;

	float charHeight;
	float charWidth;
	std::string windowTitle;
	bool windowFocused;

	Kbd kbd;
	Mouse mouse;

	static const char CH_NL = 10;
	static const char CH_EOF = 255;
	
	std::string strHelp;

	bool exitApp;
	std::string source;
	std::string strView[TEXT_HEIGHT];
	int nCharsLine[10000];
	int nLines;
	int cursorX, cursorY;
	int charPos;
	int currentLine;
	int posInLine;
	int topRenderLine;
	int pointedLine;

	char typedChar;
	std::string strTypedChar;
	bool typedWithShift;
	bool upKeyFiring;
	int upProcessedCount;
	bool downKeyFiring;
	int downProcessedCount;
	bool leftKeyFiring;
	int leftProcessedCount;
	bool rightKeyFiring;
	int rightProcessedCount;

	std::string installDir;
	std::string defaultPath;
	std::string currentPathAndFileName;
	std::string currentPath;
	std::string currentFileName;
	std::string lastSavedPathAndFileName;

	bool anotherThreadRunning;

	// undo-system related
	int historyLevel;
	std::string deleted[MAX_UNDO_LEVEL];
	std::string inserted[MAX_UNDO_LEVEL];
	int performedAt[MAX_UNDO_LEVEL];
	int charPosHist[MAX_UNDO_LEVEL];
	int topRenderLineHist[MAX_UNDO_LEVEL];

	bool selecting;
	bool selectFinished;
	int selectStart;
	int selectEnd;
	int selectStartLine;
	int selectEndLine;
	std::string strToCopy;

	int blinkState;

	bool mouseLPressed, mouseRPressed;
	bool mouseLReleased, mouseRReleased;
	bool mouseButtonChanged;
	double mouseX, mouseY;
	double prevMouseY;
	double mouseYDelta;
	int cx, cy;
	double adjustedWindowWidth, adjustedWindowHeight;
	int mouseWheelMoved;

	MPlayer mplayer;
	MPlayer* player;
	MML mml;
	Meter meter;
	Help help;
	Progress progress;
	Omake omake;
	Speech speech;
	
	sf::RectangleShape backPanel;
	// sf::RectangleShape progress; // DEBUG
	// sf::ConvexShape progressMarker; // DEBUG
	
	sf::Text message;
	bool messageShowing;
	
	Dialog dialog;

	std::string appInstallPath; // for helping MacOS version

    GUI()
	{}
	~GUI()
	{}

	void initialize();
	void run();
	void handleInputs();
	void handleTimedSaving();
	void updatePanel();
	void updateDisplay();
	void updateLineStats();
	void setStrView();
	void calculateCharPos();
	void updateCursorPos();
	void updateScreenCoordinates();
	void renewForNewSong();
	std::string getClipBd();
	void setClipBd(const std::string &str);
	void undo();
	void addHistory(std::string deletedStr, std::string insertedStr, int actionAtThen, int charPosThen, int topRenderThen);
	void eraseHistory();
	int getStartPosInLine(int line);
	int getLineNumber(int cPos);
	void emptySelection();
	void setHighlighter();
	void blinkCursor();
	void updateMeter();
    void startNewDialog();
	void quitDialog();
	void saveDialog();
	void loadDialog();
	void exportDialog();
	void setMessage(std::string strMessage);
	void unsetMessage();
	void adjustWindowSize();
	void autoSave();
	void quickSave();
	void handleAudioChecking();
	
	void dump(const std::string &dump); // debug utility function
	void setAppInstallPath(const std::string &aPath); // for helping MacOS version
	template<class T>
	std::string toString(T n);
	template<class T>
	std::string formatWithCommas(T n);

private:

};

#endif
