#define SWITCH_SENSOR_PORT S1
#define SWITCH_FLIPPED_STATE 0

void initSwitchSensor() {
	SensorType[SWITCH_SENSOR_PORT] = sensorEV3_Touch;
	wait1Msec(50);
}

bool isSwitchFlipped() {
	return SensorValue[SWITCH_SENSOR_PORT] == SWITCH_FLIPPED_STATE;
}
