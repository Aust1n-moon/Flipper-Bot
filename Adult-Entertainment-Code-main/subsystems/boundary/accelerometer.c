#include "../../drivers/hitechnic-accelerometer.h"

#define ACCELEROMETER_PORT S3
#define ACCELEROMETER_CONVERSION_FACTOR 0.049 // m / s^2 / units
#define ACCELEROMETER_VERTICAL_AXIS 2 // z-axis is up-down
#define ACCELEROMETER_VERTICAL_AXIS_CALIBRATION_DIRECTION 1 // whether robot measures gravity as positive (1) or negative (-1) when upright
#define ACCELEROMETER_CALIBRATION_WAIT_DURATION 5 // seconds
#define ACCELEROMETER_CALIBRATION_DURATION 10 // seconds

// This is in m / s^2
float accelerometerOffsets[3] = {0, 0, 0};
tHTAC accelerometerData;

void initAccelerometer() {
	initSensor(&accelerometerData, ACCELEROMETER_PORT);
	wait1Msec(50);
}

void readAccelerometerAxes(short *outputArray) {
	readSensor(&accelerometerData);
	// apply offsets before return
	outputArray[0] = accelerometerData.axes[0];
	outputArray[1] = accelerometerData.axes[1];
	outputArray[2] = accelerometerData.axes[2];
}
void readAccelerometerAxesMetric(float *outputArray) {
	short offsetAccelerometerAxes[3];
	readAccelerometerAxes(offsetAccelerometerAxes);
	outputArray[0] = offsetAccelerometerAxes[0] * ACCELEROMETER_CONVERSION_FACTOR + accelerometerOffsets[0];
	outputArray[1] = offsetAccelerometerAxes[1] * ACCELEROMETER_CONVERSION_FACTOR + accelerometerOffsets[1];
	outputArray[2] = offsetAccelerometerAxes[2] * ACCELEROMETER_CONVERSION_FACTOR + accelerometerOffsets[2];
}

// This should be run right before boundary_manager
void calibrateAccelerometer() {
	playSound(soundBlip);
	displayCenteredTextLine(3, "To begin calibrating");
	displayCenteredTextLine(4, "press center button");
	displayCenteredTextLine(5, "then put down robot");
	displayCenteredTextLine(6, "until long beep");

	while (!getButtonPress(buttonEnter)) {}
	while (getButtonPress(buttonEnter)) {}
	playSound(soundBlip);

	eraseDisplay();
	clearTimer(T1);
	while (time1[T1] < ACCELEROMETER_CALIBRATION_WAIT_DURATION * 1000) {
		displayCenteredTextLine(4, "Starting in %d seconds", ACCELEROMETER_CALIBRATION_WAIT_DURATION - time1[T1] / 1000);
	}
	playSound(soundBlip);

	displayCenteredTextLine(4, "Calibrating...");
	// Calculate average reading
	float totalAccelerometerReadings[3] = {0, 0, 0};
	int totalReadings = 0;
	clearTimer(T1);
	while (time1[T1] < ACCELEROMETER_CALIBRATION_DURATION * 1000) {
		float axes[3];
		readAccelerometerAxesMetric(axes);
		totalAccelerometerReadings[0] += axes[0];
		totalAccelerometerReadings[1] += axes[1];
		totalAccelerometerReadings[2] += axes[2];
		totalReadings ++;
	}
	accelerometerOffsets[0] = -1.0 * totalAccelerometerReadings[0] / totalReadings;
	accelerometerOffsets[1] = -1.0 * totalAccelerometerReadings[1] / totalReadings;
	accelerometerOffsets[2] = -1.0 * totalAccelerometerReadings[2] / totalReadings;
	accelerometerOffsets[ACCELEROMETER_VERTICAL_AXIS] += ACCELEROMETER_VERTICAL_AXIS_CALIBRATION_DIRECTION * 9.81;
	displayCenteredTextLine(4, "Finished Calibrating");
	playSound(soundBeepBeep);
}
