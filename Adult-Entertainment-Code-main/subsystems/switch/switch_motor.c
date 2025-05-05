#define SWITCH_MOTOR_PORT motorD
#define SWITCH_MOTOR_MOVE_DEG 150
#define SWITCH_MOTOR_SPEED 100
#define SWITCH_MOTOR_CALIB_SPEED 50

// Runs motor until it hits the back and stops moving
void initSwitchMotor() {
	nMotorEncoder[SWITCH_MOTOR_PORT] = 0;
	motor[SWITCH_MOTOR_PORT] = -SWITCH_MOTOR_CALIB_SPEED;
	int lastEncoderPos = 1;
	while (nMotorEncoder[SWITCH_MOTOR_PORT] != lastEncoderPos) {
		lastEncoderPos = nMotorEncoder[SWITCH_MOTOR_PORT];
		wait1Msec(50);
	}
	motor[SWITCH_MOTOR_PORT] = 0;

	nMotorEncoder[SWITCH_MOTOR_PORT] = 0;
}

// call as often as possible
// returns true if motor has reached destination
bool moveMotor(tMotor motorPort, int motorEncoderTarget, int motorPower) {
	if ((motorEncoderTarget - nMotorEncoder[motorPort]) * (abs(motorPower) / motorPower) > 0) {
		motor[motorPort] = motorPower;
		return false;
	}
	// else
	motor[motorPort] = 0;
	return true;
}

bool switchMotorExtending = true;
// call as often as possible when flipping switch
// returns true when done
bool flipSwitch() {
	//displayTextLine(10, "%d", switchMotorExtending);
	if (switchMotorExtending) {
		if (moveMotor(SWITCH_MOTOR_PORT, SWITCH_MOTOR_MOVE_DEG, SWITCH_MOTOR_SPEED)) {
			switchMotorExtending = false;
		}
		return false;
	}
	// else
	if (moveMotor(SWITCH_MOTOR_PORT, 0, -SWITCH_MOTOR_SPEED)) {
		switchMotorExtending = true;
	}
	// else
	return true;
}

// flip switch, locking thread
void flipSwitchSync() {
	while (moveMotor(SWITCH_MOTOR_PORT, SWITCH_MOTOR_MOVE_DEG, SWITCH_MOTOR_SPEED)) {}
	while (moveMotor(SWITCH_MOTOR_PORT, 0, -SWITCH_MOTOR_SPEED)) {}
}
