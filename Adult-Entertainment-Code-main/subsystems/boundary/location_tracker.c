// keep track of whether this file is being included or uploaded
#ifdef INCLUDE
#define LOCATION_TRACKER_INCLUDED
#else
#define INCLUDE
#endif

#include "boundary_math.c"
#include "free_motor.c"
#include "../gyro.c"

// clean up definitions
#ifndef LOCATION_TRACKER_INCLUDED
#undef INCLUDE
#else
#undef LOCATION_TRACKER_INCLUDED
#endif

Point loc;
void initLocation() {
	loc.x = 0;
	loc.y = 0;
}

float heading;
float prev_right_pos = 0;
float prev_middle_pos = 0;
float prev_heading = 0;

float tpi = 1 / CONVERSION_FACTOR;
float middle_tpi = 1;

void updateLocation() {
  float right_pos = (nMotorEncoder[MOTOR_RIGHT] + nMotorEncoder[MOTOR_LEFT]) / 2.0;
  float middle_pos = nMotorEncoder[MOTOR_LEFT];
  middle_pos = 0;

  float delta_right = (right_pos - prev_right_pos) / tpi;
  float delta_middle = (middle_pos - prev_middle_pos) / middle_tpi;

  float delta_angle;
  heading = getGyroDegrees() * 3.14159 / 180.0;
  delta_angle = heading - prev_heading;

  prev_right_pos = right_pos;
  prev_middle_pos = middle_pos;
  prev_heading = heading;

  float local_x;
  float local_y;

  if (delta_angle != 0) {
    float i = sin(delta_angle / 2.0) * 2.0;
    local_x = (delta_right / delta_angle) * i;
    local_y = (delta_middle / delta_angle) * i;
  } else {
    local_x = delta_right;
    local_y = delta_middle;
  }

  float p = heading - delta_angle / 2.0;
  loc.x += cos(p) * local_x - sin(p) * local_y;
  loc.y += cos(p) * local_y + sin(p) * local_x;
}

#ifndef INCLUDE
// Testing
task main() {
	initLocation();
	initGyro();
	resetGyro();

	long lastTimeA = time1[T1];
	long lastTimeC = time1[T1];
	long lastEncoderA = nMotorEncoder[motorA];
	long lastEncoderC = nMotorEncoder[motorC];
	while (true) {
		updateLocation();
		displayTextLine(3, "%0.3f, %0.3f", loc.x, loc.y);
		displayTextLine(4, "%d", getGyroDegrees());
		freeMoveMotor(lastTimeA, lastEncoderA, T1, motorA);
		freeMoveMotor(lastTimeC, lastEncoderC, T1, motorC);
	}
}
#endif
