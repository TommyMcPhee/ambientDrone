#include "ofApp.h"

//--------------------------------------------------------------
constexpr void ofApp::fillWavetable() {
	for (int a = 0; a < wavetableSize; a++) {
		wavetable[a] = sin(TWO_PI * (float)a / wavetableSize);
	}
}

constexpr const int ofApp::getBankSize(int order) {
	int size = 0;
	for (int a = 3; a < degree + 1; a++) {
		size++;
		for (int b = 2; b < a; b++) {
			if (a % b != 0) {
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

void ofApp::setup(){
	fillWavetable();
	const int bankSize = getBankSize(degree);
	cout << bankSize << endl;
	array<float, bankSize> oscillators;
	oscillators[0] = 1.0;
	oscillators[1] = 2.0;
	oscillators[2] = 0.5;
	for (int a = 3; a < degree + 1; a++) {
		float aFloat = (float)a;
		oscillators[bankIndex] = aFloat;
		oscillators[bankIndex + 1] = 1.0 / aFloat;
		bankIndex += 2;
		for (int b = 2; b < a; b++) {
			float bFloat = (float)b;
			if (a % b != 0) {
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
void ofApp::draw(){
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

void ofApp::audioOut(ofSoundBuffer &buffer) {
	for (int a = 0; a < buffer.getNumFrames(); a++) {
		//cout << phase << endl;
		for (int b = 0; b < channels; b++){
			phase += phaseIncrement;
			phase = fmod(phase, 1.0);
			sample[b] = lookup(phase);
			buffer[a * channels + b] = sample[b];
		}
	}

}

