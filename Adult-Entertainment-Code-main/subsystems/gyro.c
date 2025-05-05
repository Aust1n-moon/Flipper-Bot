#define GYRO_PORT S2

void initGyro() {
	displayCenteredTextLine(1, "Initializing gyro");
	while (!SensorType[GYRO_PORT] == sensorEV3_Gyro) {
		SensorType[GYRO_PORT] = sensorEV3_Gyro;
	}
	eraseDisplay();
}

void resetGyro() {
	resetGyro(GYRO_PORT);
}

long getGyroDegrees() {
	return getGyroDegrees(GYRO_PORT);
}
