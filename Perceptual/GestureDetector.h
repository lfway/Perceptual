enum GesturesList
{
	/* <- .. OO << >> <- -> ^v v^ */
	GESTURE_ERROR, GESTURE_DEFAULT, GESTURE_FARTHER, GESTURE_NEARER, GESTURE_TURNLEFT_BACK, GESTURE_TURNRIGHT_BACK, GESTURE_INCLINELEFT_BACK, GESTURE_INCLINERIGHT_BACK, GESTURE_UPDOWN, GESTURE_DOWNUP
};

enum IntermediateGesturesList
{
	GESTURE_ERROR_, GESTURE_DEFAULT_, GESTURE_FARTHER_, GESTURE_NEARER_, GESTURE_TURNLEFT_, GESTURE_TURNRIGHT_, GESTURE_INCLINELEFT_, GESTURE_INCLINERIGHT_, GESTURE_UP_, GESTURE_DOWN_
};

#define SEQUENCE_LENGTH 30
#define Z_CONSTANT_PERCENT 20

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

	GestureDetector():mSecuenceLength(30){};
	
	int getSecuenceLength(){return mSecuenceLength;};

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