# Flipper Bot

An autonomous LEGO EV3 robot that flips its own switch back off whenever someone turns it on, then runs away so it is harder to catch. It stays inside a virtual boundary using odometry, drives with PID control, and adjusts its difficulty to how fast the player is.

Written in **RobotC**.

## Features

- **Switch flipping**: when the switch is turned on, a motor-driven arm pushes it back off after a randomized delay
- **Adaptive difficulty (0–4)**: quick repeated flips raise the difficulty, and 5 seconds without a flip lowers it. Higher difficulty means faster movement and less predictable timing.
- **Evasive maneuvers**: an ultrasonic sensor and a sound sensor detect an approaching person, and the bot backs away, turns randomly or flees depending on the difficulty
- **Odometry-based boundary tracking**: the gyro plus wheel encoders track the bot's position, and point-in-polygon and ray-casting math keep it inside the play area
- **PID motion control**: separate tuned PID controllers for straight-line distance and gyro-based rotation, with settle detection and failsafe timeouts

## Hardware

| Port | Device | Purpose |
|---|---|---|
| S1 | EV3 Touch Sensor | Detects whether the switch is on |
| S2 | EV3 Gyro Sensor | Heading for odometry and turns |
| S3 | EV3 Ultrasonic Sensor | Detects a person within 20 cm |
| S4 | Sound Sensor | Detects noise above a calibrated background level |
| Motor A | Left drive motor | |
| Motor C | Right drive motor | |
| Motor D | Switch arm | Self-calibrates by driving into its hard stop at startup |

Wheel conversion factor: `2π × 2.75 / 360` (encoder degrees to distance).

## Project Structure

```
Adult-Entertainment-Code-main/
  main.c                         # Entry point: sets up sensors and the boundary, then runs the main loop
  subsystems/
    always_run.c                 # Per-tick work: odometry, switch handling, difficulty, person detection
    difficulty.c                 # Adaptive difficulty logic
    gyro.c                       # Gyro wrapper
    sound_sensor.c               # Sound sensor with background-noise calibration
    ultrasonic_sensor.c          # Ultrasonic wrapper
    boundary/
      boundary_manager.c/.h      # Boundary storage and an interactive "push to draw" boundary setup
      boundary_math.c            # Segment intersection, point-in-polygon, ray-to-boundary distance
      location_tracker.c         # Odometry (encoders + gyro)
      free_motor.c               # Lets the bot be pushed by hand while drawing the boundary
      accelerometer.c            # HiTechnic accelerometer driver and calibration (not used in main)
    movement/
      motor.c                    # Movement and rotation PID controllers, plus basic move/turn helpers
      movement_manager.c         # Evade behavior for each difficulty level
    switch/
      switch_manager.c           # Decides when to flip, with a randomized delay
      switch_motor.c             # Non-blocking arm control
      switch_sensor.c            # Touch sensor wrapper
```

Each subsystem file has its own `task main()` behind `#ifndef INCLUDE`, so you can flash any one file on its own to test that subsystem.

## How It Works

**Main loop.** Each pass calls `alwaysRun()` (update position, handle the switch, update difficulty, check sensors) and then `evade()`. Every blocking move or turn also calls `alwaysRun()` in its wait loop, so the switch still gets flipped while the bot is driving.

**Boundary.** `main.c` sets a fixed 30 × 30 square centered on the start position. You can instead call `initBoundary()`, push the bot around by hand and flick the switch at each corner; flicking twice without moving closes the shape (up to 20 points).

**Evade behavior**

| Difficulty | Behavior |
|---|---|
| 0 | Flips the switch only |
| 1 | Backs up to 10 units away when the ultrasonic sensor sees someone, or turns 90° if the boundary is too close |
| 2 | Responds to ultrasonic or sound, backs up to 15 units, and turns randomly away from the boundary |
| 3+ | Keeps moving: drives forward when a person is detected and backward otherwise, turning randomly whenever it would cross the boundary |

If odometry puts the bot outside the boundary, it turns until it faces back inside and drives back in.

## Running

1. Install **RobotC for LEGO MINDSTORMS** and the EV3 firmware.
2. Put the HiTechnic driver suite in `drivers/` (only `accelerometer.c` needs it).
3. Open `Adult-Entertainment-Code-main/main.c`, then compile and download it to the EV3.
4. At startup, keep the area quiet while the sound sensor calibrates, and let the switch arm find its home position.

## Tuning

PID gains are in `subsystems/movement/motor.c`:

```c
// movement            // rotation
m_kp = 10;  r_kp = 3;
m_ki = 0.001; r_ki = 0;
m_kd = 35;  r_kd = 45;
```

Detection thresholds (20 cm ultrasonic, 20 sound) are in `always_run.c`, and the difficulty timing (1 s for a fast flip, 5 s before it decays) is in `difficulty.c`.
