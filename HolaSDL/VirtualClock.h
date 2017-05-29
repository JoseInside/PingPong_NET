/*
 * LTimer.h
 *
 *  Created on: May 24, 2017
 *      Author: genaim
 */

#ifndef SRC_SDL_EX9_VIRTUALCLOCK_H_
#define SRC_SDL_EX9_VIRTUALCLOCK_H_

#include <sdl.h>

class VirtualClock
{
    public:
        //Initializes variables
        VirtualClock();
        ~VirtualClock();

        //The various clock actions
        void start();
        void start(Uint32 latency);
        void stop();
        void pause();
        void unpause();
        Uint32 getLatency();

        //Gets the timer's time
        Uint32 getTicks();

        //Checks the status of the timer
        bool isStarted();
        bool isPaused();

    private:
        //The clock time when the timer started
        Uint32 mStartTicks;

        //The ticks stored when the timer was paused
        Uint32 mPausedTicks;

        // Latency
        Uint32 mLatency;

        //The timer status
        bool mPaused;
        bool mStarted;
};


#endif /* SRC_SDL_EX9_VIRTUALCLOCK_H_ */
