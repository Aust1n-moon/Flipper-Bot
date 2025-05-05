// function of things that should always be running
#ifdef INCLUDE
#define ALWAYS_RUN_INCLUDED
#else
#define INCLUDE
#endif

#include "./sound_sensor.c"
#include "./boundary/boundary_manager.h"
#include "./boundary/location_tracker.c"
#include "./switch/switch_manager.c"
#include "./difficulty.c"
#include "./ultrasonic_sensor.c"

// clean up definitions
#ifndef ALWAYS_RUN_INCLUDED
#undef INCLUDE
#else
#undef ALWAYS_RUN_INCLUDED
#endif

// initially, there are no people detected
long lastUSDetectedTime = time1[T1] - 10000;
long lastSoundDetectedTime = time1[T1] - 10000;
void updatePersonDetectedTime() {
	if (getUltrasonicDistance() < 20) lastUSDetectedTime = time1[T1];
	if (readSoundSensor() > 20) lastSoundDetectedTime - time1[T1];
}

void alwaysRun(int &difficulty, bool doDifficultyUpdate, long &lastSwitchFlipTime, bool &switchFlippedLastTick, bool switchFlipped) {
	updatePersonDetectedTime();

	updateLocation();
	bool switchWasJustFlipped = switchFlipped && !switchFlippedLastTick;
	manageSwitchFlipper(switchFlipped, switchWasJustFlipped , difficulty);

	if (doDifficultyUpdate) {
		difficultyUpdate(difficulty, lastSwitchFlipTime, switchWasJustFlipped);
	}

	switchFlippedLastTick = switchFlipped;

	if (!isInBoundary(loc, boundaries, numBoundaryPoints)) {
	//	playSound(soundBlip);
	}

	#ifdef ALWAYS_RUN_DEBUG
	displayCenteredTextLine(2, "Distance Forward: %0.3f", getBoundaryDistance(loc, true, getGyroDegrees(), boundaries, numBoundaryPoints));
	displayCenteredTextLine(3, "Distance Backward: %0.3f", getBoundaryDistance(loc, false, getGyroDegrees(), boundaries, numBoundaryPoints));
	displayCenteredTextLine(4, "In Boundary: %d", isInBoundary(loc, boundaries, numBoundaryPoints));
	displayCenteredTextLine(5, "Position: %0.3f, %0.3f", loc.x, loc.y);
	displayCenteredTextLine(6, "Gyro: %d", getGyroDegrees());
	displayTextLine(1, "Difficulty: %d", difficulty);
	#endif
}
