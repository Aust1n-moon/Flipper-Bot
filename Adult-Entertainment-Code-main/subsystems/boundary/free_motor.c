// functions for allowing motors to spin freely
#ifndef MOTOR_LEFT
#define MOTOR_LEFT motorA
#endif
#ifndef MOTOR_RIGHT
#define MOTOR_RIGHT motorC
#endif

#ifndef CONVERSION_FACTOR
#define CONVERSION_FACTOR ((2 * PI * 2.75) / 360)
#endif

// Makes motor move freely
// call as often as possible
void freeMoveMotor(long &lastTime, long &lastEncoder, TTimers timer, tMotor motorPort) {
	if (nMotorEncoder[motorPort] != lastEncoder && time1[timer] != lastTime) {
		motor[motorPort] = 1000.0 * (nMotorEncoder[motorPort] - lastEncoder) / (time1[timer] - lastTime) / 15.0;

		lastEncoder = nMotorEncoder[motorPort];
		lastTime = time1[timer];
	}
}

#ifndef INCLUDE
task main() {
	long lastTimeA = time1[T1];
	long lastTimeC = time1[T1];
	long lastEncoderA = nMotorEncoder[MOTOR_LEFT];
	long lastEncoderC = nMotorEncoder[MOTOR_RIGHT];
	while (true) {
		// allow user to push bot around
		freeMoveMotor(lastTimeA, lastEncoderA, T1, MOTOR_LEFT);
		freeMoveMotor(lastTimeC, lastEncoderC, T1, MOTOR_RIGHT);
	}
}
#endif
