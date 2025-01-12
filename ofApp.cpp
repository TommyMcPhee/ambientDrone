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

constexpr const int ofApp::getBankSize(int order) {
	int size = 0;
	for (int a = 2; a < limit; a++) {
		//size++;
		bool aPrime = true;
		for (int b = 2; b < a; b++) {
			if (a % b == 0) {
				aPrime = false;
			}
		}
		for (int b = 1; b < a; b++) {
			bool bPrime = true;
			for (int c = 2; c < b; c++) {
				if (b % c == 0) {
					bPrime = false;
					c = b;
				}
			}
			if (aPrime || bPrime && a % b != 0) {
				size++;
			}
		}
	}
	return size * 2 + 3;
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
	for (int a = 0; a < limit; a++) {
		//cout << endl;
	}
	//const array<bool, limit> primesConstant = primes;
	const int bankSize = getBankSize(limit);
	//cout << bankSize << endl;
	array<float, bankSize> oscillators;
	oscillators[0] = 1.0;
	//oscillators[1] = 2.0;
	//oscillators[2] = 0.5;
	for (int a = 2; a < limit; a++) {
		float aFloat = (float)a;
		//oscillators[bankIndex] = aFloat;
		//oscillators[bankIndex + 1] = 1.0 / aFloat;
		//bankIndex += 2;
		bool aPrime = true;
		for (int b = 2; b < a; b++) {
			if (a % b == 0) {
				aPrime = false;
				b = a;
			}
		}
		for (int b = 1; b < a; b++) {
			bool bPrime = true;
			for (int c = 2; c < b; c++) {
				if (b % c == 0) {
					bPrime = false;
					c = b;
				}
			}
			if (aPrime || bPrime && a % b != 0) {
				float bFloat = (float)b;
				//cout << a << " " << b << endl;
				oscillators[bankIndex] = aFloat / bFloat;
				oscillators[bankIndex + 1] = bFloat / aFloat;
				bankIndex += 2;
			}
		}
	}
	for (int a = 0; a < bankSize; a++) {
		cout << oscillators[a] << endl;
	}
	phase = 0.0;
	phaseIncrement = 0.001;
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
	for (int a = 0; a < buffer.getNumFrames(); a++) {
		//cout << phase << endl;
		for (int b = 0; b < channels; b++) {
			phase += phaseIncrement;
			phase = fmod(phase, 1.0);
			sample[b] = lookup(phase);
			buffer[a * channels + b] = sample[b];
		}
	}

}
