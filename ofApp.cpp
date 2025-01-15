#include "ofApp.h"

//--------------------------------------------------------------
constexpr void ofApp::fillWavetable() {
	for (int a = 0; a < wavetableSize; a++) {
		wavetable[a] = sin(TWO_PI * (float)a / wavetableSize);
	}
	primes[0] = true;
	primes[1] = true;
	for (int a = 2; a < limit; a++) {
		primes[a] = true;
		for (int b = a + 1; b < limit; b++) {
			if (b % a == 0) {
				primes[b - 1] = false;
			}
		}
	}
}

bool ofApp::checkPrime(int number) {
	bool prime = true;
	for (int c = 2; c < number; c++) {
		if (number % c == 0) {
			prime = false;
			c = number;
		}
	}
	return prime;
}

void ofApp::hardwareSetup() {
	shader.load("ambientDrone");
	frameBuffer.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	frameBuffer.begin();
	ofClear(0, 0, 0, 255);
	frameBuffer.end();
	streamSettings.setOutListener(this);
	streamSettings.setApi(ofSoundDevice::Api::MS_WASAPI);
	streamSettings.sampleRate = sampleRate;
	streamSettings.bufferSize = bufferSize;
	streamSettings.numOutputChannels = channels;
	stream.setup(streamSettings);
}

void ofApp::setup() {
	fillWavetable();
	hardwareSetup();
}

void ofApp::setUniforms() {
	shader.setUniform2f("window", window);
}

//--------------------------------------------------------------
void ofApp::draw() {
	width = (float)ofGetWidth();
	height = (float)ofGetHeight();
	frameBuffer.allocate(width, height);
	ofClear(0, 0, 0, 255);
	window.set(width, height);
	frameBuffer.begin();
	shader.begin();
	setUniforms();
	frameBuffer.draw(0, 0);
	shader.end();
	frameBuffer.end();
	frameBuffer.draw(0.0, 0.0);
}

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings streamSettings) {

}

float ofApp::averageTwo(float inA, float inB, float mix) {
	return (1.0 - mix) * inA + (inB * mix);
}

float ofApp::lookup(float phase) {
	float floatIndex = phase * (float)wavetableSize;
	float remainderIndex = fmod(floatIndex, 1.0);
	int intIndex = (int)floatIndex;
	if (intIndex >= wavetableSize - 1) {
		intIndex = wavetableSize - 2;
	}
	return averageTwo(wavetable[intIndex], wavetable[intIndex + 1], remainderIndex);
}

float ofApp::oscillate(float phase, float phaseIncrement) {
	return 0.0;
}

void ofApp::audioOut(ofSoundBuffer& buffer) {
	if (audioSetup) {
		oscillators[0][2] = 1.0;
		for (int a = 2; a < limit; a++) {
			float aFloat = (float)a;
			bool aPrime = checkPrime(a);
			for (int b = 1; b < a; b++) {
				bool bPrime = checkPrime(b);
				if (aPrime || bPrime && a % b != 0) {
					float bFloat = (float)b;
					oscillators[bankIndex][2] = aFloat / bFloat;
					oscillators[bankIndex + 1][2] = bFloat / aFloat;
					bankIndex += 2;
				}
			}
		}
		bankSize = bankIndex + 1;
		for (int a = 0; a < bankSize; a++) {
			oscillators[a][0] = 0.707;
			oscillators[a][1] = 0.707;
			cout << oscillators[a][2] << endl;
		}
		audioSetup = false;
	}
	for (int a = 0; a < buffer.getNumFrames(); a++) {
		for (int b = 0; b < channels; b++) {
			sample[b] = 0.0;
			for (int c = 0; c < bankSize; c++) {
				oscillators[c][3] += phaseIncrement * oscillators[c][2];
				oscillators[c][3] = fmod(oscillators[c][3], 1.0);
				sample[b] += oscillators[c][b] * lookup(oscillators[c][3]) / (float)bankSize;
			}
			buffer[a * channels + b] = sample[b];
		}
	}

}