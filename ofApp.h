#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	static const int wavetableSize = 256;
	array<float, wavetableSize> wavetable;
	static const int limit = 17;
	array<bool, limit> primes;
	constexpr void fillWavetable();
	constexpr static const int getBankSize(int order);
	ofShader shader;
	ofFbo frameBuffer;
	ofSoundStreamSettings streamSettings;
	const int sampleRate = 48000;
	const int bufferSize = 256;
	const int channels = 2;
	int bankIndex = 1;
	ofSoundStream stream;
	void hardwareSetup();
	float phase, phaseIncrement;
	void setup();
	float width, height;
	ofVec2f window;
	void setUniforms();
	void draw();
	void ofSoundStreamSetup(ofSoundStreamSettings settings);
	float averageTwo(float inA, float inB, float mix);
	float lookup(float phase);
	float oscillate(float phase, float phaseIncrement);
	array<float, 2> sample;
	void audioOut(ofSoundBuffer& audioBuffer);
};