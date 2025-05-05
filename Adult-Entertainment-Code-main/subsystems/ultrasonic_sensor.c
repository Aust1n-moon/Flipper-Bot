#define ULTRASONIC_SENSOR_PORT S3

void initUltrasonicSensor() {
	displayCenteredTextLine(1, "Initializing Ultrasonic Sensor");
	while (!SensorType[ULTRASONIC_SENSOR_PORT] == sensorEV3_Ultrasonic) {
		SensorType[ULTRASONIC_SENSOR_PORT] = sensorEV3_Ultrasonic;
	}
	eraseDisplay();
}

long getUltrasonicDistance() {
	return getUSDistance(ULTRASONIC_SENSOR_PORT);
}

#ifndef INCLUDE
task main() {
	initUltrasonicSensor();

	while (true) {
		displayTextLine(5, "%d", getUltrasonicDistance());
	}
}
#endif
