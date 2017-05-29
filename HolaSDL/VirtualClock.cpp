/*
 * LTimer.cpp
 *
 *  Created on: May 24, 2017
 *      Author: genaim
 */

#include "VirtualClock.h"

VirtualClock::VirtualClock() {
	//Initialize the variables
	mStartTicks = 0;
	mPausedTicks = 0;
	mLatency = 0;

	mPaused = false;
	mStarted = false;
}

VirtualClock::~VirtualClock() {
}

void VirtualClock::start() {
	//Start the timer
	mStarted = true;

	//Unpause the timer
	mPaused = false;

	//Get the current clock time
	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void VirtualClock::start(Uint32 latency) {
	//Start the timer
	mStarted = true;

	//Unpause the timer
	mPaused = false;

	//Get the current clock time
	mStartTicks = SDL_GetTicks() - latency;
	mPausedTicks = 0;
	mLatency = latency;
}

void VirtualClock::stop() {
	//Stop the timer
	mStarted = false;

	//Unpause the timer
	mPaused = false;

	//Clear tick variables
	mStartTicks = 0;
	mPausedTicks = 0;
}

void VirtualClock::pause() {
	//If the timer is running and isn't already paused
	if (mStarted && !mPaused) {
		//Pause the timer
		mPaused = true;

		//Calculate the paused ticks
		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}

void VirtualClock::unpause() {
	//If the timer is running and paused
	if (mStarted && mPaused) {
		//Unpause the timer
		mPaused = false;

		//Reset the starting ticks
		mStartTicks = SDL_GetTicks() - mPausedTicks;

		//Reset the paused ticks
		mPausedTicks = 0;
	}
}

Uint32 VirtualClock::getTicks() {
	//The actual timer time
	Uint32 time = 0;

	//If the timer is running
	if (mStarted) {
		//If the timer is paused
		if (mPaused) {
			//Return the number of ticks when the timer was paused
			time = mPausedTicks;
		} else {
			//Return the current time minus the start time
			time = SDL_GetTicks() - mStartTicks;
		}
	}

	return time;
}

bool VirtualClock::isStarted() {
	//Timer is running and paused or unpaused
	return mStarted;
}

Uint32 VirtualClock::getLatency() {
	return mLatency;
}

bool VirtualClock::isPaused() {
	//Timer is running and paused
	return mPaused && mStarted;
}

