// code for sound sensor
#define SOUND_SENSOR_PORT S4
#define SOUND_SENSOR_CALIB_TIME 0.5 // second

float soundSensorOffset;
float readSoundSensor() {
	return SensorValue[S4] - soundSensorOffset;
}

// calibrate sound sensor to filter out background noise
void calibrateSoundSensor() {
	writeDebugStreamLine("Here1");
	eraseDisplay();
	displayCenteredTextLine(2, "Calibrating Sound Sensor");

	long initialTime = time1[T1];
	float totalSensorValue = 0;
	int numReadings = 0;
	writeDebugStreamLine("Here2");
	while (time1[T1] - initialTime < SOUND_SENSOR_CALIB_TIME * 1000) {
		totalSensorValue += readSoundSensor();
		numReadings ++;
	}
	writeDebugStreamLine("Here3");
	soundSensorOffset = -totalSensorValue / numReadings;
	writeDebugStreamLine("Here4");

	eraseDisplay();
}

#ifndef INCLUDE
task main() {
	calibrateSoundSensor();

	while (true) {
		displayTextLine(5, "%d", readSoundSensor());
	}
}
#endif
