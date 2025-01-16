#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	static const int wavetableSize = 256;
	array<float, wavetableSize> wavetable;
	static const int limit = 11;
	static const int pseudoLimit = limit - 1;
	array<bool, limit> primes;
	constexpr void fillWavetable();
	constexpr const int getBankSize(int order);
	array<array<float, 7>, pseudoLimit* pseudoLimit> oscillators;
	bool checkPrime(int number);
	ofShader shader;
	ofFbo frameBuffer;
	ofSoundStreamSettings streamSettings;
	const int sampleRate = 48000;
	const int bufferSize = 4096;
	const int channels = 2;
	const float length = (float)sampleRate * 1200.0;
	const float progressIncrement = 1.0 / length;
	const float phaseIncrement = (float)sampleRate * 0.5 / (float)limit;
	int bankIndex = 0;
	ofSoundStream stream;
	void hardwareSetup();
	void setup();
	float width, height;
	ofVec2f window;
	void setUniforms();
	void draw();
	void ofSoundStreamSetup(ofSoundStreamSettings settings);
	bool audioSetup = true;
	float randomWalk(float initial, float averageTwo);
	float averageTwo(float inA, float inB, float mix);
	float lookup(float phase);
	float minimumFloat, progress = 0.0, droneAmplitude = 0.0, dronePhase = 0.0, droneSample = 0.0;
	array<float, 2> sample;
	void audioOut(ofSoundBuffer& audioBuffer);
};