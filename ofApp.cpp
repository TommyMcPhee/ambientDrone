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
	minimumFloat = std::numeric_limits<float>::min();
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
	//return 0.5;
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
					oscillators[bankIndex][5] = aFloat / bFloat;
					oscillators[bankIndex + 1][5] = bFloat / aFloat;
					oscillators[bankIndex][6] = aFloat;
					oscillators[bankIndex + 1][6] = aFloat;
					bankIndex += 2;
				}
			}
		}
		for (int a = 0; a < bankIndex; a++) {
			for (int b = 0; b < 4; b++) {
				oscillators[a][b] = 0.5;
			}
		}
		audioSetup = false;
	}
	else {
		for (int a = 0; a < buffer.getNumFrames(); a++) {
			droneAmplitude = lookup(progress * 0.5);
			for (int b = 0; b < bankIndex; b++) {
				for (int c = 0; c < 4; c++) {
					oscillators[b][c] = randomWalk(oscillators[b][c], 1.0 / (float)pow(limit - (int)oscillators[b][6], limit));
				}
			}
			for (int b = 0; b < channels; b++) {
				progress += progressIncrement;
				sample[b] = 0.0;
				dronePhase += phaseIncrement;
				dronePhase = fmod(dronePhase, 1.0);
				droneSample = lookup(dronePhase) * droneAmplitude;
				for (int c = 0; c < bankIndex; c++) {
					if (oscillators[c][b] >= minimumFloat) {
						oscillators[c][4] += phaseIncrement * oscillators[c][5] + (droneSample * oscillators[c][b + 2]);
						oscillators[c][4] = fmod(oscillators[c][4], 1.0);
						sample[b] += sqrt(oscillators[c][b]) * lookup(oscillators[c][4]) / (oscillators[c][6] * (float)bankIndex);
					}
				}
				buffer[a * channels + b] = sample[b];
			}
		}
	}

}