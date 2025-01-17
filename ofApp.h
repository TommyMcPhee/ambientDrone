#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	static const int wavetableSize = 2048;
	array<float, wavetableSize> wavetable;
	static const int limit = 64;
	static const int pseudoLimit = limit - 1;
	array<bool, limit> primes;
	constexpr void fillWavetable();
	array<array<float, 8>, pseudoLimit * pseudoLimit> oscillators;
	bool checkPrime(int number);
	ofShader shader;
	ofFbo frameBuffer;
	ofSoundStreamSettings streamSettings;
	const int sampleRate = 48000;
	const int bufferSize = 4096;
	const int channels = 2;
	const float length = (float)sampleRate * 120.0;
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
	float progress = 0.0, droneAmplitude = 0.0, dronePhase = 0.0, droneSample = 0.0;
	array<float, 2> sample;
	void audioOut(ofSoundBuffer& audioBuffer);
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
};