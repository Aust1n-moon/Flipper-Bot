// keep track of whether this file is being included or uploaded
#define ALWAYS_RUN_DEBUG

#ifdef INCLUDE
#define INCLUDED
#else
#define INCLUDE
#endif

#include "subsystems/switch/switch_manager.c"
#include "subsystems/boundary/boundary_manager.c"
#include "subsystems/movement/movement_manager.c"
#include "subsystems/difficulty.c"
#include "subsystems/always_run.c"

// clean up definitions
#ifndef INCLUDED
#undef INCLUDE
#else
#undef INCLUDED
#endif

task main()
{
	clearDebugStream();
	initBoundaryManagerSensors();
	initSwitchManagerSensors();
	initMovementManagerSensors();

	// initBoundary();
	numBoundaryPoints = 4;
	boundaries[0].x = -15; boundaries[0].y = -15;
	boundaries[1].x = 15; boundaries[1].y = -15;
	boundaries[2].x = 15; boundaries[2].y = 15;
	boundaries[3].x = -15; boundaries[3].y = 15;

	// store info for difficulty calculations
	int difficulty = 3;
	bool doDifficultyUpdate = false;
	long lastSwitchFlipTime = time1[T1];
	// keep track of if switch was just flipped
	bool switchFlippedLastTick = isSwitchFlipped();

	while(true) {
		alwaysRun(difficulty, doDifficultyUpdate, lastSwitchFlipTime, switchFlippedLastTick, isSwitchFlipped());
		evade(difficulty, doDifficultyUpdate, lastSwitchFlipTime, switchFlippedLastTick);
	}
}
