//Evade Code
#ifdef INCLUDE
#define MOVEMENT_MANAGER_INCLUDED
#else
#define INCLUDE
#endif

#include "../boundary/boundary_manager.c"
#include "../sound_sensor.c"
#include "../ultrasonic_sensor.c"
#include "../gyro.c"
#include "motor.c"
#include "../always_run.c"

// clean up definitions
#ifndef MOVEMENT_MANAGER_INCLUDED
#undef INCLUDE
#else
#undef MOVEMENT_MANAGER_INCLUDED
#endif

void initMovementManagerSensors() {
	writeDebugStreamLine("Here0");
	calibrateSoundSensor();
	initUltrasonicSensor();
	initGyro();
}

//random speed
float rand_speed (int set_difficulty)
{
	if (set_difficulty == 0) { return 20; }
	float speed = abs(rand()) % (30 * set_difficulty - 20 * set_difficulty) + 20 * set_difficulty;
	return speed;
}

//random turn
void rand_turn(bool clockwise, int &difficulty, bool doDifficultyUpdate, long &lastSwitchFlipTime, bool &switchFlippedLastTick)
{
	int speed = rand_speed(difficulty);
	int targetDegrees = (clockwise? 1: -1) * ((abs(rand()) % 90) + 30) + getGyroDegrees();
	displayTextLine(9, "Turn target: %d, %d", targetDegrees, speed);
	//init_rotate_pid();
	while (!rotate_basic(targetDegrees, speed)) {
		alwaysRun(difficulty, doDifficultyUpdate, lastSwitchFlipTime, switchFlippedLastTick, isSwitchFlipped());
	}
	long initTime = time1[T1];
	while (time1[T1] - initTime < 250) {
		alwaysRun(difficulty, doDifficultyUpdate, lastSwitchFlipTime, switchFlippedLastTick, isSwitchFlipped());
	}
}

bool detectedPersonWithinTime(float time, bool countUS, bool countSound) {
	return (countUS && (time1[T1] - lastUSDetectedTime < time * 1000)) || (countSound && (time1[T1] - lastSoundDetectedTime < time * 1000));
}

//Evade for ultrasonic sensor
int difficultyOneCounter = 0;
long lastDifficulty1Evade = time1[T1];
void evade (int &difficulty, bool doDifficultyUpdate, long &lastSwitchFlipTime, bool &switchFlippedLastTick)
{
	float speed = rand_speed(difficulty);

	if (!isInBoundary(loc, boundaries, numBoundaryPoints)) {
		displayTextLine(8, "Returning To Boundaries");
		bool direction = abs(rand()) % 2 == 0;
		while (!isFacingBoundary(loc, 1000000.0, true, getGyroDegrees(), boundaries, numBoundaryPoints)) {
			rand_turn(direction, difficulty, doDifficultyUpdate, lastSwitchFlipTime, switchFlippedLastTick);
		}
		if (isFacingBoundary(loc, 1000000.0, true, getGyroDegrees(), boundaries, numBoundaryPoints)) {
			float distanceToBoundary = getSecondBoundaryDistance(loc, true, getGyroDegrees(), boundaries, numBoundaryPoints) - 2;
			displayTextLine(8, "Return Dist: %0.3f", distanceToBoundary);
			init_move_pid();
			while (!move_pid(distanceToBoundary, speed)) {
				alwaysRun(difficulty, doDifficultyUpdate, lastSwitchFlipTime, switchFlippedLastTick, isSwitchFlipped());
			}
		}

		return; // stop code execution
	}

	if (difficulty == 1) {// WIP
		if (detectedPersonWithinTime(2.5, true, false))
		{
			float distanceToBoundary = getBoundaryDistance(loc, false, getGyroDegrees(), boundaries, numBoundaryPoints) - 2;
			displayTextLine(8, "Moving Dist: %0.3f", distanceToBoundary);

			if (time1[T1] - lastDifficulty1Evade > 2000) {
				if (distanceToBoundary > 5) {
					init_move_pid();
					while (!move_pid(-1 * min(distanceToBoundary, 10), 20)) {
						alwaysRun(difficulty, doDifficultyUpdate, lastSwitchFlipTime, switchFlippedLastTick, isSwitchFlipped());
					}
				}
				else { // we are too close to the boundary, need to turn
					long targetDegrees = getGyroDegrees() + 90;
					while (!rotate_basic(targetDegrees, 20)) {
						alwaysRun(difficulty, doDifficultyUpdate, lastSwitchFlipTime, switchFlippedLastTick, isSwitchFlipped());
					}
				}

				lastDifficulty1Evade = time1[T1];
			}
		}

		return;
	}

	if (difficulty == 2) {// WIP
		if (detectedPersonWithinTime(2.5, true, true))
		{
			float distanceToBoundary = getBoundaryDistance(loc, false, getGyroDegrees(), boundaries, numBoundaryPoints) - 2;
			displayTextLine(8, "Moving Dist: %0.3f", distanceToBoundary);

			if (time1[T1] - lastDifficulty1Evade > 2000) {
				if (distanceToBoundary > 2.5) {
					init_move_pid();
					while (!move_pid(-1 * min(distanceToBoundary, 15), speed)) {
						alwaysRun(difficulty, doDifficultyUpdate, lastSwitchFlipTime, switchFlippedLastTick, isSwitchFlipped());
					}
				}
				else { // we are too close to the boundary, need to turn
					bool rotateDirection = abs(rand()) % 2 == 0;
					while (isFacingBoundary(loc, 5, false, getGyroDegrees(), boundaries, numBoundaryPoints)) {
						displayTextLine(8, "Turning: %d", rotateDirection);
						rand_turn(rotateDirection, difficulty, doDifficultyUpdate, lastSwitchFlipTime, switchFlippedLastTick);
					}
				}

				lastDifficulty1Evade = time1[T1];
			}
		}

		return;
	}


	if (difficulty == 3)
	{
		bool moveForward = detectedPersonWithinTime(2.5, true, true);

		bool rotateDirection = abs(rand()) % 2 == 0;
		while (isFacingBoundary(loc, 5, moveForward, getGyroDegrees(), boundaries, numBoundaryPoints)) {
			displayTextLine(8, "Turning: %d", rotateDirection);
			rand_turn(rotateDirection, difficulty, doDifficultyUpdate, lastSwitchFlipTime, switchFlippedLastTick);
		}
		// stop running if we leave boundary (sometimes turning can cause robot to drift outside boundary)
		if (!isInBoundary(loc, boundaries, numBoundaryPoints)) { return; }

		if (!isFacingBoundary(loc, 5, moveForward, getGyroDegrees(), boundaries, numBoundaryPoints)) {
			float distanceToBoundary = getBoundaryDistance(loc, moveForward, getGyroDegrees(), boundaries, numBoundaryPoints) - 2;
			displayTextLine(8, "Moving Dist: %0.3f", distanceToBoundary);
			init_move_pid();
			while (!move_pid((moveForward? 1 : -1) * distanceToBoundary, speed)) {
				alwaysRun(difficulty, doDifficultyUpdate, lastSwitchFlipTime, switchFlippedLastTick, isSwitchFlipped());

				// immediately stop when we detect a change in whether we should move forward
				if ((bool) moveForward != (bool) detectedPersonWithinTime(2.5, true, true)) {
					motor[MOTOR_LEFT] = motor[MOTOR_RIGHT] = 0;
					return;
				}
			}
		}
	}

	// THIS IS THE OLD CODE ONLY FOR REFERENCE DO NOT USE
	//if (difficulty == 2 && 1+1 == 3) // random degrees turning, moves straight back 15cm
	//{
	//	if (getUltrasonicDistance() < 20 || readSoundSensor() > 10)
	//	{
	//		displayTextLine(7, "facing %d", isFacingBoundary(loc, 3, false, getGyroDegrees(), boundaries, numBoundaryPoints));
	//		if (isFacingBoundary(loc, 3, false, getGyroDegrees(), boundaries, numBoundaryPoints) == true) {
	//			rand_turn(difficulty, doDifficultyUpdate, lastSwitchFlipTime, switchFlippedLastTick);
	//			return; // stop code execution
	//		}

	//		init_move_pid();
	//		while (!move_pid(-15, speed)) {
	//			alwaysRun(difficulty, doDifficultyUpdate, lastSwitchFlipTime, switchFlippedLastTick, isSwitchFlipped());
	//		}
	//	}
	//}
}
