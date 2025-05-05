// keep track of whether this file is being included or uploaded
#ifdef INCLUDE
#define SWITCH_MANAGER_INCLUDED
#else
#define INCLUDE
#endif

#include "switch_sensor.c"
#include "switch_motor.c"

// clean up definitions
#ifndef SWITCH_MANAGER_INCLUDED
#undef INCLUDE
#else
#undef SWITCH_MANAGER_INCLUDED
#endif

void initSwitchManagerSensors() {
	initSwitchMotor();
	initSwitchSensor();
}

long calculateSwitchFlipDelay(int difficulty) {
	if (difficulty == 0) return 250;
	return 250 + rand() % (200 * difficulty + 50 * difficulty) - 50 * difficulty;
}

// call as often as possible
// flips switch when necessary
bool flippingSwitch = false;
long lastSwitchFlipTime = time1[T1];
long switchFlipDelay = 0;
void manageSwitchFlipper(bool switchFlipped, bool switchJustFlipped, int difficulty) {
	if (flippingSwitch) {
		flippingSwitch = !flipSwitch();
	}
	else if (switchFlipped) {
		if (switchJustFlipped) {
			lastSwitchFlipTime = time1[T1];
			switchFlipDelay = calculateSwitchFlipDelay(difficulty);
		}
		else if (time1[T1] - lastSwitchFlipTime > switchFlipDelay) {
			flippingSwitch = true;
			switchMotorExtending = true;
		}
	}
	//displayTextLine(9, "%d, %d, %d, %d, %d", switchFlipDelay, lastSwitchFlipTime, time1[T1], flippingSwitch, switchFlipped);
}

#ifndef INCLUDE
// Testing
task main() {
	initSwitchManagerSensors();

	bool switchFlippedLastTick = isSwitchFlipped(); // this is used to remember if switchFlipped is new
	while (true) {
		manageSwitchFlipper(isSwitchFlipped(), isSwitchFlipped() && !switchFlippedLastTick, 2);

		switchFlippedLastTick = isSwitchFlipped();
	}
}
#endif
