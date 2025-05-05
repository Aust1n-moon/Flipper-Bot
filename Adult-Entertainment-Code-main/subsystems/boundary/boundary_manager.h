#ifndef BOUDARY_MANAGER_H
#define BOUNDARY_MANAGER_H

#include "boundary_math.c"

// I don't think there's a way around using fixed array sizes
#define BOUNDARY_MAX_POINTS 20

// A 2d array of points for boundaries
// [[x, y], [x2, y2], ...]
int numBoundaryPoints = 0;
Point boundaries[BOUNDARY_MAX_POINTS];

void initBoundaryManagerSensors();

// returns true if boundary point added successfully
// false means that we should close boundary
bool addBoundaryPoint(int index);

// walks user through making boundary
void initBoundary();

#endif