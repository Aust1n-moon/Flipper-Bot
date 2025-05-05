// functions for motors

// keep track of whether this file is being included or uploaded
#ifdef INCLUDE
#define MOTOR_INCLUDED
#else
#define INCLUDE
#endif

#include "../gyro.c"

// clean up definitions
#ifndef MOTOR_INCLUDED
#undef INCLUDE
#else
#undef MOTOR_INCLUDED
#endif

#ifndef MOTOR_LEFT
#define MOTOR_LEFT motorA
#endif
#ifndef MOTOR_RIGHT
#define MOTOR_RIGHT motorC
#endif

#ifndef CONVERSION_FACTOR
#define CONVERSION_FACTOR ((2 * PI * 2.75) / 360)
#endif

float globalTheta = 0;

int wrapGyroTo360(int rawGyro) {
	int wrappedValue = rawGyro % 360;
	if (wrappedValue < 0) {
		wrappedValue += 360;
	}
	return wrappedValue;
}

int get_robot_heading() {
	int rawGyroValue = getGyroDegrees();
	return wrapGyroTo360(rawGyroValue);
}


void move_left(float speed){ motor[MOTOR_LEFT] = speed; }
void move_right(float speed){ motor[MOTOR_RIGHT] = speed; }
void stop_motors(){ motor[MOTOR_LEFT] = 0; motor[MOTOR_RIGHT] = 0; }
float cfabs(float num) { return (num < 0) ? -num : num; }


int sgn(int num) {
	if (num > 0) {
		return 1;
	} else if (num < 0) {
		return -1;
	} else {
		return 0;
	}
}


float m_kp = 10;
float m_ki = 0.001;
float m_kd = 35;
float m_derivative = 0;
float m_integral = 0;
float m_error = 0;
float m_prev_error = 0;
float m_iterator = 0;
float m_max_speed = 80;
float m_failsafe = 0;

float r_kp = 3;
float r_ki = 0;
float r_kd = 45;
float r_derivative = 0;
float r_integral = 0;
float r_error = 0;
float r_prev_error = 0;
float r_iterator = 0;
float r_max_speed = 30;
float r_failsafe = 0;

float find_min_angle(float targetHeading, float currentRobotHeading) {
	float turnAngle = targetHeading - currentRobotHeading;
	if (turnAngle > 180 || turnAngle < -180) { turnAngle = turnAngle - (sgn(turnAngle) * 360); }
	return turnAngle;
}

void reset_movement_values() {
	m_derivative = 0;
	m_integral = 0;
	m_error = 0;
	m_prev_error = 0;
	m_iterator = 0;
	m_failsafe = 0;
}

void reset_rotation_values() {
	r_derivative = 0;
	r_integral = 0;
	r_error = 0;
	r_prev_error = 0;
	r_iterator = 0;
	r_failsafe = 0;
}

float compute_movement_pid(float current, float target) {
	m_error = target - current;
	m_derivative = m_error - m_prev_error;

	if (m_ki != 0) {
		m_integral += m_error;
	}

	float output = (m_kp * m_error) + (m_integral * m_ki) + (m_derivative * m_kd);
	if (output > m_max_speed) { output = m_max_speed; }
	if (output < -m_max_speed) { output = -m_max_speed; }
	m_prev_error = m_error;
	return output;
}

float compute_rotation_pid(float current, float target) {
	r_error = find_min_angle(target, current);
	r_derivative = r_error - r_prev_error;

	if (r_ki != 0) {
		r_integral += r_error;
	}

	float output = (r_kp * r_error) + (r_integral * r_ki) + (r_derivative * r_kd);
	if (output > r_max_speed) { output = r_max_speed; }
	if (output < -r_max_speed) { output = -r_max_speed; }
	r_prev_error = r_error;
	return output;
}

long initialMotorLeft;
long initialMotorRight;
bool is_backwards;
int cd;
float reference_angle;
void init_move_pid() {
	initialMotorLeft = nMotorEncoder[MOTOR_LEFT];
	initialMotorRight = nMotorEncoder[MOTOR_RIGHT];
	reset_movement_values();
	is_backwards = false;
	cd = 0;
	reference_angle = get_robot_heading();
}
// returns true once done
bool move_pid(float target, float speed) {
	if (target < 0) { is_backwards = true; } else { is_backwards = false; }
	m_max_speed = speed;
	float avgPos = CONVERSION_FACTOR * ((nMotorEncoder[MOTOR_LEFT] - initialMotorLeft + nMotorEncoder[MOTOR_RIGHT] - initialMotorRight) / 2.0);
	displayTextLine(7, "%d target: %d", avgPos, target);
	float turnAngleDiff = find_min_angle(reference_angle, get_robot_heading()) * 0;
	float avg_voltage_req; cd++;
	if (is_backwards) {avg_voltage_req = -compute_movement_pid(-avgPos, -target);}
	else {avg_voltage_req = compute_movement_pid(avgPos, target);}
	if (cd <= 2) { move_left(0); move_right(0); return false; }
	float l_output = avg_voltage_req;
	float r_output = avg_voltage_req;

	motor[MOTOR_LEFT] = l_output + (turnAngleDiff);
	motor[MOTOR_RIGHT] = r_output - turnAngleDiff;
	//displayTextLine(1, "turn angle error: %d", turnAngleDiff);
	//displayTextLine(2, "gyro: %d", get_robot_heading());
	//displayTextLine(3, "degrees: %d", getGyroDegrees());
	//displayTextLine(4, "motor speed: %d", l_output);

	if (cfabs(m_error) < 0.5) {m_iterator++;}
	else { m_iterator = 0; }
	if (cfabs(m_iterator) > 3) { stop_motors(); return true; }
	if (m_failsafe >= 1000) {
		m_failsafe = 0;
		stop_motors(); return true;
	}

	wait1Msec(10);
	return false;
}

void init_rotate_pid() {
	initialMotorLeft = nMotorEncoder[MOTOR_LEFT];
	initialMotorRight = nMotorEncoder[MOTOR_RIGHT];
	reset_rotation_values();
	cd = 0;
	r_max_speed = 50;
}
// returns true once done
bool rotate_pid(float target, float speed) {
	r_max_speed = speed;
	float avgPos = get_robot_heading();
	float avg_voltage_req = compute_rotation_pid(avgPos, target); cd++;
	if (cd <= 2) { move_left(0); move_right(0); return false; }
	float l_output = avg_voltage_req;
	float r_output = avg_voltage_req;

	//displayTextLine(2, "gyro: %d", get_robot_heading());

	move_left(l_output);
	move_right(-r_output);

	if (cfabs(r_error) < 3) {r_iterator++;}
	else { r_iterator = 0; }
	if (cfabs(r_iterator) > 3) { stop_motors(); return true; }
	r_failsafe++;
	if (r_failsafe >= (200)) {
		r_failsafe = 0;
		stop_motors(); return true;
	}


	wait1Msec(10);
	return false;
}

bool rotate_basic(float target, float motorPower) {
	long currentDegrees = getGyroDegrees();
	if (abs(currentDegrees - target) > 10) {
		motor[MOTOR_LEFT] = - motorPower * (abs(currentDegrees - target) / (currentDegrees - target));
		motor[MOTOR_RIGHT] = motorPower * (abs(currentDegrees - target) / (currentDegrees - target));
		return false;
	}
	// else
	motor[MOTOR_LEFT] = motor[MOTOR_RIGHT] = 0;
	return true;
}
long initMotorEncoder = 0;
void init_move_basic() {
	initMotorEncoder = nMotorEncoder[MOTOR_LEFT];
}
bool move_basic(long distance, float motorPower) {
	if (abs(distance) - abs(CONVERSION_FACTOR * (nMotorEncoder[MOTOR_LEFT] - initMotorEncoder)) > 0) {
		motor[MOTOR_LEFT] = abs(motorPower) * (abs(distance) / distance);
		motor[MOTOR_RIGHT] = abs(motorPower) * (abs(distance) / distance);
		return false;
	}
	// else
	motor[MOTOR_LEFT] = motor[MOTOR_RIGHT] = 0;
	return true;
}

#ifndef INCLUDE
task main() {
	initGyro();
	resetGyro();
	nMotorEncoder[MOTOR_LEFT] = 0;
	nMotorEncoder[MOTOR_RIGHT] = 0;
	init_move_pid();
	while (!move_pid(100, 20)) { wait1Msec(50); }
	init_move_pid();
	while (!move_pid(-100, 50)) { wait1Msec(50); }
	init_move_pid();
	while (!move_pid(-50, 50)) { wait1Msec(50); }
	init_move_pid();
	while (!move_pid(50, 50)) { wait1Msec(50); }
	//init_rotate_pid();
	//while (!rotate_pid(45, 50)) {}
	//init_rotate_pid();
	//while (!rotate_pid(42, 50)) {}
	//init_rotate_pid();
	//while (!rotate_pid(-63, 50)) {}
	//init_rotate_pid();
	//while (!rotate_pid(20, 50)) {}
	// float target = getGyroDegrees() + 90;
	// while (!rotate_basic(target, 50));
	// target = getGyroDegrees() - 90;
	// while (!rotate_basic(target, 50));
	// init_move_basic();
	// while (!move_basic(-10, 50));
}
#endif
