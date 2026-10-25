// DelayLine class - definition //////////////////////////////////////

#ifndef DELAYLINE_H
#define DELAYLINE_H

#include <vector>

class DelayLine
{

static const int DELAY_TABLE_SIZE;
static const int DELAY_SAMPLE_RATE;

public:

	std::vector<float> buffer1;
	std::vector<float> buffer2;
	int buffer1len;
	int buffer2len;
	int totalDelayFrames;
	int readIndex1;
	int writeIndex1;
	int readIndex2;
	int writeIndex2;
	float out1;
	float out2;
	float outGain1;
	float outGain2;

	DelayLine();
	~DelayLine();

	void clearBuffer();
	void setParameters(int firstDelayTime, int delayTime, float delayGain);
	float update(float input);
};

#endif