#include <vector>
#include <queue>
#include <math.h>
using namespace std;

#define SEQUENCE_LENGTH 30
#define Z_CONSTANT_PERCENT 20

#define theCoords std::pair<int, int>

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

class FacePosition
{
public:
	FacePosition(theCoords eyeLeft, theCoords eyeRight, theCoords mouth, theCoords centerFrame):
		mEyeLeft(eyeLeft), mEyeRight(eyeRight), mMouth(mouth), mCenterFrame(centerFrame)
	{
		// angle between eyes
		mAngleLeftEyeToRightEye = CalculateAngle(mEyeLeft, mEyeRight);
		// angle between eye - mouth
		mAngleLeftEyeToMouth	= CalculateAngle(mEyeLeft, mMouth);
		mAngleRightEyeToMouth	= CalculateAngle(mEyeRight, mMouth);
		//center face
		mCenterFace = theCoords((mEyeLeft.first + mEyeRight.first + mMouth.first)/3, (mEyeLeft.second + mEyeRight.second + mMouth.second)/3);
	}
	int getAndleEyes() const { return mAngleLeftEyeToRightEye; }
protected:
	theCoords mEyeLeft;
	theCoords mEyeRight;
	theCoords mMouth;
	theCoords mCenterFrame;
	theCoords mCenterFace;
	int mDX;
	int mDY;
	int mDXframe;
	int mDYframe;
	int mAngleLeftEyeToRightEye;
	int mAngleLeftEyeToMouth;
	int mAngleRightEyeToMouth;

	int m_DeltaIncline;

public:
	string m_incline_to;
};

class GestureDetector
{
public:

	void AddPoision(FacePosition position)
	{
		mFacePositionsSequence.push_back(position);
		if(mFacePositionsSequence.size() >= SEQUENCE_LENGTH)
		{
			mFacePositionsSequence.erase(mFacePositionsSequence.begin());
		}
	}
	void Process()
	{
		// incline delta
		if(mFacePositionsSequence.size() < 2)
			return;
		int angle_eyes_prelast = mFacePositionsSequence[mFacePositionsSequence.size()-2].getAndleEyes();
		int angle_eyes_last = mFacePositionsSequence[mFacePositionsSequence.size()-1].getAndleEyes();	
		int delta_angle_eyes = angle_eyes_last - angle_eyes_prelast;

		if(delta_angle_eyes < 0)
			mFacePositionsSequence[mFacePositionsSequence.size()-1].m_incline_to = "<";
		if(delta_angle_eyes > 0)
			mFacePositionsSequence[mFacePositionsSequence.size()-1].m_incline_to = ">";
		if(delta_angle_eyes == 0)
			mFacePositionsSequence[mFacePositionsSequence.size()-1].m_incline_to = ".";
		if(abs(delta_angle_eyes) > 100)
			mFacePositionsSequence[mFacePositionsSequence.size()-1].m_incline_to = "E";
		// incline amplitude

		CalcAmplitudes();
		CalcInclineHistory();
	}

	int getAmplitudeIncline() const { return mAmplitudeIncline; }
	string getInclineHistory() const { return mInclineHistory; }
protected:
	void CalcInclineHistory()
	{
		mInclineHistory.clear();
		for(unsigned int i = 0; i < mFacePositionsSequence.size(); i++)
		{
			mInclineHistory += mFacePositionsSequence[i].m_incline_to;
		}
	}
	int CalcAmplitudes()
	{
		if( mFacePositionsSequence.size() < 2 )
			return -1;
		int angle_min = mFacePositionsSequence[0].getAndleEyes(), angle_max = mFacePositionsSequence[0].getAndleEyes();
		for(unsigned int i = 0; i < mFacePositionsSequence.size(); i++)
		{
			int angle_ = mFacePositionsSequence[i].getAndleEyes();
			if(angle_ < angle_min) angle_min = angle_;
			if(angle_ > angle_max) angle_max = angle_;
		}
		mAmplitudeIncline = angle_max - angle_min;
	}
public:
	string mInclineHistory;
protected:
	int mAmplitudeTurnHorizontal;
	int mAmplitudeIncline;
	int mAmplitudeUpDown;
	int mAmplitudeZdistance;

	

	std::vector<FacePosition> mFacePositionsSequence;
	int mSecuenceLength;
};