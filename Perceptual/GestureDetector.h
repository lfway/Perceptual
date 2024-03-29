#include <vector>
#include <queue>
#include <math.h>
using namespace std;


typedef void (CALLBACK * fnCallBackFunc)(std::string value);



#define SEQUENCE_LENGTH 30
#define Z_CONSTANT_PERCENT 20

#define theCoords std::pair<int, int>

int getDistance(theCoords point1, theCoords point2)
{
	return (int)sqrt( double((point1.first-point2.first)*(point1.first-point2.first) + (point1.second-point2.second)*(point1.second-point2.second)));
}

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
		mAngleLeftEyeToRightEye = (int)CalculateAngle(mEyeLeft, mEyeRight);
		// angle between eye - mouth
		mAngleLeftEyeToMouth	= (int)CalculateAngle(mEyeLeft, mMouth);
		mAngleRightEyeToMouth	= (int)CalculateAngle(mEyeRight, mMouth);
		//center face
		mCenterFace = theCoords((mEyeLeft.first + mEyeRight.first)/2, (mEyeLeft.second + mEyeRight.second)/2);
		//eyes distance
		mEyesDist = getDistance(mEyeLeft, mEyeRight);
	}
	int getAndleEyes() const { return mAngleLeftEyeToRightEye; }
	int getCenter() const { return mCenterFace.first; }
	int getEyesDist() const { return mEyesDist; }

protected:
	int mEyesDist;
	theCoords mEyeLeft;
	theCoords mEyeRight;
	theCoords mMouth;
	theCoords mCenterFrame;
	theCoords mCenterFace;

	int mAngleLeftEyeToRightEye;
	int mAngleLeftEyeToMouth;
	int mAngleRightEyeToMouth;

	int m_DeltaIncline;
	int m_DeltaTurnLeftRight;
	int m_DeltaZMovement;

public:
	string m_incline_to;
	string m_turn_to;
	string m_z_to;
};







#include <sstream>
class GestureDetector
{
public:
	fnCallBackFunc func;
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
		if( mFacePositionsSequence.size() == 1 )
		{
			mAmplitudeTurnHorizontal_center = mFacePositionsSequence[0].getCenter();
			int qwe = 234;
		}
		// incline delta
		if(mFacePositionsSequence.size() < 2)
			return;

		// incline amplitude & history
		int angle_eyes_prelast = mFacePositionsSequence[mFacePositionsSequence.size()-2].getAndleEyes();
		int angle_eyes_last = mFacePositionsSequence[mFacePositionsSequence.size()-1].getAndleEyes();	
		int delta_angle_eyes = angle_eyes_last - angle_eyes_prelast;
		std::string sss111222333 = "";
		if(delta_angle_eyes < 0)
		{mFacePositionsSequence[mFacePositionsSequence.size()-1].m_incline_to = "<";sss111222333="<";}
		if(delta_angle_eyes > 0)
		{mFacePositionsSequence[mFacePositionsSequence.size()-1].m_incline_to = ">";sss111222333=">";}
		if(delta_angle_eyes == 0)
		{mFacePositionsSequence[mFacePositionsSequence.size()-1].m_incline_to = ".";sss111222333=".";}
		if(abs(delta_angle_eyes) > 100)
		{mFacePositionsSequence[mFacePositionsSequence.size()-1].m_incline_to = "E";sss111222333="E";}


		func(sss111222333);

		// turn amplitude & history
		int center_face_turn_prelast = mFacePositionsSequence[mFacePositionsSequence.size()-2].getCenter();
		int center_face_turn_last = mFacePositionsSequence[mFacePositionsSequence.size()-1].getCenter();
		int delta_center_face_turn = center_face_turn_last - center_face_turn_prelast;
		if(delta_center_face_turn < 0)
			mFacePositionsSequence[mFacePositionsSequence.size()-1].m_turn_to = "<";
		if(delta_center_face_turn > 0)
			mFacePositionsSequence[mFacePositionsSequence.size()-1].m_turn_to = ">";
		if(delta_center_face_turn == 0)
			mFacePositionsSequence[mFacePositionsSequence.size()-1].m_turn_to = ".";
		if(abs(delta_center_face_turn) > 100)
			mFacePositionsSequence[mFacePositionsSequence.size()-1].m_turn_to = "E";

		// z movement history
		int eyes_dist_prelast	= mFacePositionsSequence[mFacePositionsSequence.size()-2].getEyesDist();
		int eyes_dist_last		= mFacePositionsSequence[mFacePositionsSequence.size()-1].getEyesDist();
		int delta_eyes_dist = eyes_dist_last - eyes_dist_prelast;
		if(delta_eyes_dist < 0)
			mFacePositionsSequence[mFacePositionsSequence.size()-1].m_z_to = "o";
		if(delta_eyes_dist > 0)
			mFacePositionsSequence[mFacePositionsSequence.size()-1].m_z_to = "O";
		if(delta_eyes_dist == 0)
			mFacePositionsSequence[mFacePositionsSequence.size()-1].m_z_to = ".";
		if(abs(delta_eyes_dist) > 100)
			mFacePositionsSequence[mFacePositionsSequence.size()-1].m_z_to = "E";

		CalcAmplitudes();
		CalcHistory();
	}

	int getAmplitudeIncline() const { return mAmplitudeIncline; }
	string getInclineHistory() const { return mInclineHistory; }
	int getAmplitudeTurn() const { return mAmplitudeTurnHorizontal; }
	string getTurnHistory() const { return mTurnHistory; }
	string getZHistory() const { return mZHistory; }
protected:
	void CalcHistory()
	{
		mInclineHistory.clear();
		mTurnHistory.clear();
		mZHistory.clear();
		for(unsigned int i = 0; i < mFacePositionsSequence.size(); i++)
		{
			mInclineHistory	+= mFacePositionsSequence[i].m_incline_to;
			mTurnHistory	+= mFacePositionsSequence[i].m_turn_to;
			mZHistory		+= mFacePositionsSequence[i].m_z_to;
		}
	}
	int CalcAmplitudes()
	{
		if( mFacePositionsSequence.size() < 2 )
			return -1;
		int angle_min = mFacePositionsSequence[0].getAndleEyes(), angle_max = mFacePositionsSequence[0].getAndleEyes();
		int turn_min = mFacePositionsSequence[0].getCenter(), turn_max = mFacePositionsSequence[0].getCenter();
		for(unsigned int i = 0; i < mFacePositionsSequence.size(); i++)
		{
			int angle_ = mFacePositionsSequence[i].getAndleEyes();
			if(angle_ < angle_min) angle_min = angle_;
			if(angle_ > angle_max) angle_max = angle_;

			//int turn_ = mFacePositionsSequence[i].getCenter() - mAmplitudeTurnHorizontal_center;
			int turn_ = abs(mAmplitudeTurnHorizontal_center - abs(mAmplitudeTurnHorizontal_center - mFacePositionsSequence[i].getCenter()));
			if(turn_ < turn_min) turn_min = turn_;
			if(turn_ > turn_max) turn_max = turn_;
		}
		mAmplitudeIncline = angle_max - angle_min;
		mAmplitudeTurnHorizontal = abs(abs(turn_max + turn_min)/2 - mAmplitudeTurnHorizontal_center);

		if( abs(mAmplitudeTurnHorizontal) < 3)
			mAmplitudeTurnHorizontal_center = (turn_max + turn_min)/2;

		//mAmplitudeTurnHorizontal; -= mAmplitudeTurnHorizontal_center;
		return 0;
	}
public:
	string mInclineHistory;
	string mTurnHistory;
	string mZHistory;
protected:
	int mAmplitudeTurnHorizontal;
	int mAmplitudeTurnHorizontal_center;

	int mAmplitudeIncline;
	int mAmplitudeUpDown;
	int mAmplitudeZdistance;

	

	std::vector<FacePosition> mFacePositionsSequence;
	int mSecuenceLength;
};