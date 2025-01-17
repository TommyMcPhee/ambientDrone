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
	shader.setUniform1f("droneAmplitude", droneAmplitude);
}

//--------------------------------------------------------------
void ofApp::draw() {
	width = (float)ofGetWidth();
	height = (float)ofGetHeight();
	window.set(width, height);
	frameBuffer.allocate(width, height);
	frameBuffer.begin();
	ofClear(0, 0, 0, 255);
	shader.begin();
	setUniforms();
	frameBuffer.draw(0.0, 0.0);
	shader.end();
	frameBuffer.end();
	frameBuffer.draw(0.0, 0.0);
}

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings streamSettings) {

}

inline float ofApp::randomWalk(float initial, float delta) {
	float increment = delta * ofRandomf() * (1.0 - droneAmplitude) * initial * (1.0 - initial);
	float step = initial + increment;
	if (step <= delta) {
		return delta * abs(ofRandomf());
	}
	if (step >= 1.0 - delta) {
		return 1.0 - (delta * abs(ofRandomf()));
	}
	else {
		return initial + increment;
	}
}

inline float ofApp::averageTwo(float inA, float inB, float mix) {
	return (1.0 - mix) * inA + (inB * mix);
}

inline float ofApp::lookup(float phase) {
	float floatIndex = phase * (float)wavetableSize;
	float remainderIndex = fmod(floatIndex, 1.0);
	int intIndex = (int)floatIndex;
	if (intIndex >= wavetableSize - 1) {
		intIndex = wavetableSize - 2;
	}
	return averageTwo(wavetable[intIndex], wavetable[intIndex + 1], remainderIndex);
}

void ofApp::audioOut(ofSoundBuffer& buffer) {
	if (audioSetup) {
		for (int a = 2; a < limit; a++) {
			float aFloat = (float)a;
			bool aPrime = checkPrime(a);
			for (int b = 1; b < a; b++) {
				bool bPrime = checkPrime(b);
				if (aPrime || bPrime && a % b != 0) {
					float bFloat = (float)b;
					oscillators[bankIndex][6] = aFloat / bFloat;
					oscillators[bankIndex + 1][6] = bFloat / aFloat;
					oscillators[bankIndex][7] = aFloat;
					oscillators[bankIndex + 1][7] = aFloat;
					bankIndex += 2;
				}
			}
		}
		for (int a = 0; a < bankIndex; a++) {
			oscillators[a][4] = ofRandomuf() * (1.0 - (1.0 / oscillators[a][7]));
			for (int b = 0; b < 4; b++) {
				oscillators[a][b] = 0.5;
			}
		}
		audioSetup = false;
	}
	for (int a = 0; a < buffer.getNumFrames(); a++) {
		progress += progressIncrement;
		droneAmplitude = lookup(progress * 0.5);
		dronePhase += phaseIncrement;
		dronePhase = fmod(dronePhase, 1.0);
		droneSample = lookup(dronePhase) * droneAmplitude;
		sample = { 0.0, 0.0 };
		for (int b = 0; b < bankIndex; b++) {
			float activeOscillators = 0.0;
			if (progress > oscillators[b][4] && progress < oscillators[b][4] + (1.0 / oscillators[b][7])) {
				activeOscillators++;
				for (int c = 0; c < 2; c++) {
					oscillators[b][c] = lookup((progress - oscillators[b][4]) / 2.0);
				}
				for (int c = 0; c < channels; c++) {
					float increment = phaseIncrement * oscillators[b][6];
					float inverseIncrement = 1.0 - increment;
					oscillators[b][5] += increment + (droneSample * inverseIncrement * oscillators[b][c + 2]);
					oscillators[b][5] = fmod(oscillators[b][5], 1.0);
					sample[c] += sqrt(oscillators[b][c]) * lookup(oscillators[b][5]) * inverseIncrement / (oscillators[b][7] * activeOscillators);
				}
			}
		}
		for (int b = 0; b < channels; b++) {
			buffer[a * channels + b] = sample[b];
		}
	}

}

void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}