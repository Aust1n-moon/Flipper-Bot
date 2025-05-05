// just some useful math functions

// taken from https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
typedef struct Point
{
	float x;
	float y;
} Point;

float max(float a, float b) {
	if (a > b) return a;
	return b;
}
float min(float a, float b) {
	if (a < b) return a;
	return b;
}

// returns distance between two points
float getDistance(Point p1, Point q1) {
	return sqrt(pow(p1.x - q1.x, 2) + pow(p1.y - q1.y, 2));
}

// Given three collinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool onSegment(Point p, Point q, Point r)
{
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
	return true;

	return false;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are collinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(Point p, Point q, Point r)
{
	// See https://www.geeksforgeeks.org/orientation-3-ordered-points/
	// for details of below formula.
	int val = (q.y - p.y) * (r.x - q.x) -
	(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // collinear

	return (val > 0)? 1: 2; // clock or counterclock wise
}

// The main function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
bool doesIntersect(Point p1, Point q1, Point p2, Point q2)
{
	// Find the four orientations needed for general and
	// special cases
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases
	// p1, q1 and p2 are collinear and p2 lies on segment p1q1
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and q2 are collinear and q2 lies on segment p1q1
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are collinear and p1 lies on segment p2q2
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are collinear and q1 lies on segment p2q2
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases
}

// function that returns interesction Point in output,
// of lines going through 'p1q1' and 'p2q2'
// Does nothing if liens do not intersect
void getLineIntersection(Point p1, Point q1, Point p2, Point q2, Point* output) {
	// Line p1q1 represented as a1x + b1y = c1
	float a1 = q1.y - p1.y;
	float b1 = p1.x - q1.x;
	float c1 = a1 * p1.x + b1 * p1.y;

	// Line p2q2 represented as a2x + b2y = c2
	float a2 = q2.y - p2.y;
	float b2 = p2.x - q2.x;
	float c2 = a2 * p2.x + b2 * p2.y;

	// Determinant of the system
	float determinant = a1 * b2 - a2 * b1;

	if (determinant == 0) {
		// Lines are parallel or coincident
		// Do nothing
	} else {
		// Calculate intersection point
		output->x = (b2 * c1 - b1 * c2) / determinant;
		output->y = (a1 * c2 - a2 * c1) / determinant;
	}
}


bool isInBoundary(Point loc, Point* boundaries, int boundariesSize) {
	// use ray casting algorithm from https://en.wikipedia.org/wiki/Point_in_polygon
	// loop through boundaries
	Point outsideBoundary;
	outsideBoundary.x = -1000000.0;
	outsideBoundary.y = 0;
	int totalIntersections = 0;
	for (int i = 0; i < boundariesSize; i ++) {
		Point boundaryPoint1;
		boundaryPoint1.x = boundaries[i].x;
		boundaryPoint1.y = boundaries[i].y;
		Point boundaryPoint2;
		boundaryPoint2.x = boundaries[(i + 1) % boundariesSize].x;
		boundaryPoint2.y = boundaries[(i + 1) % boundariesSize].y;
		if (doesIntersect(outsideBoundary, loc, boundaryPoint1, boundaryPoint2)) totalIntersections ++;
	}
	// if odd, then it's in boundaries
	return totalIntersections % 2 == 1;
}

// check if robot is going to hit the boundary within distance
bool isFacingBoundary(Point loc, float distance, bool forward, long gyroDegrees, Point* boundaries, int boundariesSize)
{
	for (int i = 0; i<boundariesSize; i++) {
		Point boundaryPoint1;
		boundaryPoint1.x = boundaries[i].x;
		boundaryPoint1.y = boundaries[i].y;
		Point boundaryPoint2;
		boundaryPoint2.x = boundaries[(i + 1) % boundariesSize].x;
		boundaryPoint2.y = boundaries[(i + 1) % boundariesSize].y;

		Point newLoc;
		newLoc.x = loc.x + (forward? 1 : -1) * distance*cosDegrees(gyroDegrees);
		newLoc.y = loc.y + (forward? 1 : -1) * distance*sinDegrees(gyroDegrees);

		if (doesIntersect(newLoc, loc, boundaryPoint1, boundaryPoint2)) {
			return true;
		}
	}
	return false;
}

// only valid if facing boundary
float getBoundaryDistance(Point loc, bool forward, long gyroDegrees, Point* boundaries, int boundariesSize){
	float minDistance = 1000000.0;
	for (int i = 0; i<boundariesSize; i++) {
		Point boundaryPoint1;
		boundaryPoint1.x = boundaries[i].x;
		boundaryPoint1.y = boundaries[i].y;
		Point boundaryPoint2;
		boundaryPoint2.x = boundaries[(i + 1) % boundariesSize].x;
		boundaryPoint2.y = boundaries[(i + 1) % boundariesSize].y;

		Point newLoc;
		newLoc.x = loc.x + (forward? 1 : -1) * 1000000.0*cosDegrees(gyroDegrees);
		newLoc.y = loc.y + (forward? 1 : -1) * 1000000.0*sinDegrees(gyroDegrees);

		if (doesIntersect(newLoc, loc, boundaryPoint1, boundaryPoint2)) {
			Point intersectionPoint;
			getLineIntersection(newLoc, loc, boundaryPoint1, boundaryPoint2, intersectionPoint);
			// calculate distance
			float distance = getDistance(loc, intersectionPoint);
			if (distance < minDistance) minDistance = distance;
		}
	}
	return minDistance;
}

// only valid if facing boundary
// returns second furthest boundary distance
float getSecondBoundaryDistance(Point loc, bool forward, long gyroDegrees, Point* boundaries, int boundariesSize){
	float minDistance = 1000000.0;
	float secondMinDistance = 1000000.0;
	for (int i = 0; i<boundariesSize; i++) {
		Point boundaryPoint1;
		boundaryPoint1.x = boundaries[i].x;
		boundaryPoint1.y = boundaries[i].y;
		Point boundaryPoint2;
		boundaryPoint2.x = boundaries[(i + 1) % boundariesSize].x;
		boundaryPoint2.y = boundaries[(i + 1) % boundariesSize].y;

		Point newLoc;
		newLoc.x = loc.x + (forward? 1 : -1) * 1000000.0*cosDegrees(gyroDegrees);
		newLoc.y = loc.y + (forward? 1 : -1) * 1000000.0*sinDegrees(gyroDegrees);

		if (doesIntersect(newLoc, loc, boundaryPoint1, boundaryPoint2)) {
			Point intersectionPoint;
			getLineIntersection(newLoc, loc, boundaryPoint1, boundaryPoint2, intersectionPoint);
			// calculate distance
			float distance = getDistance(loc, intersectionPoint);
			if (distance < minDistance) {
				secondMinDistance = minDistance;
				minDistance = distance;
			}
			else if (distance < secondMinDistance) {
				secondMinDistance = distance;
			}
		}
	}
	return secondMinDistance;
}
