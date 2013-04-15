#include <vector>
#include <math.h>
using namespace std;

#define SEQUENCE_LENGTH 30
#define Z_CONSTANT_PERCENT 20

enum GesturesList
{
	/* <- .. OO << >> <- -> ^v v^ */
	GESTURE_ERROR, GESTURE_DEFAULT, GESTURE_FARTHER, GESTURE_NEARER, GESTURE_TURNLEFT_BACK, GESTURE_TURNRIGHT_BACK, GESTURE_INCLINELEFT_BACK, GESTURE_INCLINERIGHT_BACK, GESTURE_UPDOWN, GESTURE_DOWNUP
};

enum IntermediateGesturesList
{
	GESTURE_ERROR_, GESTURE_DEFAULT_, GESTURE_FARTHER_, GESTURE_NEARER_, GESTURE_TURNLEFT_, GESTURE_TURNRIGHT_, GESTURE_INCLINELEFT_, GESTURE_INCLINERIGHT_, GESTURE_UP_, GESTURE_DOWN_
};

double CalculateAngle(pair<int, int> point1, pair<int, int> point2)
{	
	int dx = point1.first - point2.first;
	int dy = point2.second - point1.second;
	/*
	double giptnz = sqrt( (double)dx*dx + (double)dy*dy);

	double a1 = dy/giptnz;
	double a2 = acos(a1);
	double a3 = a2/3.14*180;


	double a4 = asin(dy/giptnz)   /3.14*180;

	return atan(dy/dx)*180/3.14;
	return a4;

	return dx/giptnz/3.14*180;*/

	return atan((double)dy/(double)dx)*180/3.14;
}

/*
For every frame
1. create and calculste FacePosition struct
2. add it to face detector

Detect gesture:
1. One gesture consists of one or two gestures

Returning result:
1. Result generates from from two parts: GesturesList and IntermediateGesturesList

*/

struct FacePosition
{
	int mDX;
	int mDY;
	int mDXframe;
	int mDYframe;
	int mAngleLeftEyeToRightEye;
	int mAngleLeftEyeToMouth;
	int mAngleRightEyeToMouth;
};

class GestureDetector
{

	void AddPoision(FacePosition position)
	{

	}

	int Procces()
	{

	}

protected:
	std::vector<FacePosition> FacePositionsSequence;
	int mSecuenceLength;
};