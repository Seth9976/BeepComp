// MML.h /////////////////////////////////////
// MML Class - Definition ////////////////////

#ifndef MML_H
#define MML_H

// forward declared dependencies

class MData;
class DData;
class MPlayer;

// include dependencies

#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <string>
#include "MData.h"
#include "DData.h"
#include "MPlayer.h"

#ifdef __APPLE__
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif

class MML
{

public:

	static const int N_EVENT_TAGS = 256;

	// constructor
	MML();
	MML(double sampleRate, double tempo);
	
	void initialize(double sampleRate, double tempo);
	void calculateTiming();

	std::string setSource(std::string masterStr);
	std::string getSource();
	std::string takeOutComments(std::string masterStr);
	std::string takeOutSpaces(std::string str);
	std::string loadFile(std::string filename, MPlayer* player);
	std::string saveFile(std::string filename, MPlayer* player);
	std::string parse(MPlayer* player);
	std::string parseChannelSource(MPlayer* player, int channel);
	std::string parseDrumSource(MPlayer* player);
	void parseGlobalSource(MPlayer* player);
	
	double getFrequency(int toneNum);
	
	int countDigits(std::string snippet);
	
	void errLog(std::string errText1, std::string errText2="");
	std::string toString(int n);
	
	// destructor
	~MML();
	
	std::string eventTag[N_EVENT_TAGS];
	std::string eventTagLower[N_EVENT_TAGS]; // copy, in lowercase letters
	int eventTagLen[N_EVENT_TAGS];
	std::string eventTagDrum[N_EVENT_TAGS];
	std::string eventTagDrumLower[N_EVENT_TAGS]; // copy, in lowercase letters
	
	int eventTagLenDrum[N_EVENT_TAGS];
	
	// MData data[4];
	std::string originalSource;
	std::string source[9];
	std::string dsource;
	std::string gsource;
	
	double tempo;
	double sampleRate;
	int baseLength;  // length of 16th notes
	int quarterNoteLength;
	int measureLength;
	int halfMeasureLength;
	int octave;
	int noteLength;
	
	MData* output;
	DData* dOutput;
	int lengthCounter;
	
	double semitoneRatio;
	double middleC;
	double cZero;
	
private:
	
};

#endif