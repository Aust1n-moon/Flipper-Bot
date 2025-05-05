#include "boundary_manager.h"

// keep track of whether this file is being included or uploaded
#ifdef INCLUDE
#define BOUNDARY_MANAGER_INCLUDED
#else
#define INCLUDE
#endif

#include "../switch/switch_manager.c"
#include "free_motor.c"
#include "location_tracker.c"
#include "boundary_math.c"
#include "../always_run.c"

// clean up definitions
#ifndef BOUNDARY_MANAGER_INCLUDED
#undef INCLUDE
#else
#undef BOUNDARY_MANAGER_INCLUDED
#endif

// I don't think there's a way around using fixed array sizes
// #define BOUNDARY_MAX_POINTS 20

// A 2d array of points for boundaries
// [[x, y], [x2, y2], ...]
// int numBoundaryPoints = 0;
// Point boundaries[BOUNDARY_MAX_POINTS];

void initBoundaryManagerSensors() {
	initGyro();
	resetGyro();
}

// returns true if boundary point added successfully
// false means that we should close boundary
bool addBoundaryPoint(int index) {
	// if we've reached the maximum amount of points
	if (index >= BOUNDARY_MAX_POINTS) {
		return false;
	}
	// if a point is repeated
	if (index > 0 && boundaries[index - 1].x == loc.x && boundaries[index - 1].y == loc.y) {
		return false;
	}
	boundaries[index].x = loc.x;
	boundaries[index].y = loc.y;
	numBoundaryPoints ++;
	return true;
}

// walks user through making boundary
void initBoundary() {
	if (isSwitchFlipped()) { flipSwitchSync(); }

	resetGyro(); // reseting gyro after this line will completely mess up position tracking
	initLocation();

	eraseDisplay();
	displayCenteredBigTextLine(1, "Init Boundary");
	displayCenteredTextLine(3, "Flick switch to");
	displayCenteredTextLine(4, "add boundary point.");
	displayCenteredTextLine(5, "Flick without moving");
	displayCenteredTextLine(6, "to close boundary");
	displayCenteredTextLine(7, "Total Points: %d / %d", numBoundaryPoints, BOUNDARY_MAX_POINTS);
	displayCenteredTextLine(8, "Position: %d, %d", loc.x, loc.y);

	int difficulty = 0;
	long lastSwitchFlipTime = time1[T1];
	// store info for freeMoveMotor
	long lastTimeA = time1[T1];
	long lastTimeC = time1[T1];
	long lastEncoderA = nMotorEncoder[motorA];
	long lastEncoderC = nMotorEncoder[motorC];
	// keep track of how many points we have
	int boundaryPoint = 0;
	// keep track of if switch was just flipped
	bool switchFlippedLastTick = isSwitchFlipped();

	while (true) {
		// allow user to push bot around
		freeMoveMotor(lastTimeA, lastEncoderA, T1, motorA);
		freeMoveMotor(lastTimeC, lastEncoderC, T1, motorC);
		bool switchFlipped = isSwitchFlipped();
		// detect if this is the first tick we're flipping
		if (switchFlipped && !switchFlippedLastTick) {
			// once we've reached the limit or added a location twice
			playSound(soundUpwardTones);
			if (!addBoundaryPoint(boundaryPoint ++)) {
				eraseDisplay();
				displayCenteredBigTextLine(1, "Init Boundary");
				displayCenteredBigTextLine(4, "Finished");
				displayCenteredTextLine(7, "Total Points: %d / %d", numBoundaryPoints, BOUNDARY_MAX_POINTS);
				playSound(soundDownwardTones);
				if (isSwitchFlipped()) { flipSwitchSync(); }
				// wait 5 seconds
				long timeBefore = time1[T1];
				while (time1[T1] - timeBefore < 5000) {
					// allow user to push bot around
					freeMoveMotor(lastTimeA, lastEncoderA, T1, motorA);
					freeMoveMotor(lastTimeC, lastEncoderC, T1, motorC);

					// flip switch if necessary
					alwaysRun(difficulty, false, lastSwitchFlipTime, switchFlippedLastTick, isSwitchFlipped());
				}
				eraseDisplay();
				break;
			}
		}

		alwaysRun(difficulty, false, lastSwitchFlipTime, switchFlippedLastTick, switchFlipped);

		displayCenteredTextLine(7, "Total Points: %d / %d", numBoundaryPoints, BOUNDARY_MAX_POINTS);
		displayCenteredTextLine(8, "Position: %0.3f, %0.3f", loc.x, loc.y);
	}

	motor[MOTOR_LEFT] = motor[MOTOR_RIGHT] = 0;
}

#ifndef INCLUDE
task main() {
	initBoundaryManagerSensors();
	initSwitchManagerSensors();

	//initBoundary();
	numBoundaryPoints = 4;
	boundaries[0].x = -15; boundaries[0].y = -15;
	boundaries[1].x = 15; boundaries[1].y = -15;
	boundaries[2].x = 15; boundaries[2].y = 15;
	boundaries[3].x = -15; boundaries[3].y = 15;

	int difficulty = 0;
	long lastSwitchFlipTime = time1[T1];
	// store info for freeMoveMotor
	long lastTimeA = time1[T1];
	long lastTimeC = time1[T1];
	long lastEncoderA = nMotorEncoder[motorA];
	long lastEncoderC = nMotorEncoder[motorC];
	// keep track of if switch was just flipped
	bool switchFlippedLastTick = isSwitchFlipped();

	while (true) {
		// allow user to push bot around
		freeMoveMotor(lastTimeA, lastEncoderA, T1, motorA);
		freeMoveMotor(lastTimeC, lastEncoderC, T1, motorC);

		alwaysRun(difficulty, false, lastSwitchFlipTime, switchFlippedLastTick, isSwitchFlipped());

		displayCenteredTextLine(1, "Facing Forward: %d", isFacingBoundary(loc, 5, true, getGyroDegrees(), boundaries, numBoundaryPoints));
		displayCenteredTextLine(2, "Facing Backward: %d", isFacingBoundary(loc, 5, false, getGyroDegrees(), boundaries, numBoundaryPoints));
		displayCenteredTextLine(3, "Distance Forward: %0.3f", getBoundaryDistance(loc, true, getGyroDegrees(), boundaries, numBoundaryPoints));
		displayCenteredTextLine(4, "Distance Forward 2: %0.3f", getSecondBoundaryDistance(loc, true, getGyroDegrees(), boundaries, numBoundaryPoints));
		displayCenteredTextLine(6, "In Boundary: %d", isInBoundary(loc, boundaries, numBoundaryPoints));
		displayCenteredTextLine(7, "Position: %0.3f, %0.3f", loc.x, loc.y);
		displayCenteredTextLine(8, "Gyro: %d", getGyroDegrees());
	}
}
#endif
