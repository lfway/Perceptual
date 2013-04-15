// example from intel site

#include "cstdlib"

#include "util_render.h"
#include "util_pipeline.h"

#include "face_render.h"
#include "util_capture_file.h"
#include "util_cmdline.h"
#include "pxcface.h"


#include "GestureDetector.h"

#define theLandMarkData PXCFaceAnalysis::Landmark::LandmarkData


theCoords getCenter(theLandMarkData* first, theLandMarkData* second)
{
	int x2 = max((int)first->position.x, (int)second->position.x);
	int x1 = min((int)first->position.x, (int)second->position.x);
	int y2 = max((int)first->position.y, (int)second->position.y);
	int y1 = min((int)first->position.y, (int)second->position.y);

	theCoords center;
	center.first	= x1 + (x2 - x1)/2;
	center.second	= y1 + (y2 - y1)/2;
	return center;
}

class FacePipeline: public UtilPipeline 
{
public:
	
	FacePipeline(void): UtilPipeline()
	{
		m_face_render = NULL;
		// Create a face renderer
		m_face_render =	new	FaceRender(L"Face Viewer");
		// Enable the face landmark detector
		EnableFaceLandmark();
	}
	~FacePipeline()
	{
		if(m_face_render != NULL)
			delete m_face_render;
	}
 
	virtual bool OnNewFrame(void)
	{
		//system("cls");

		//wprintf_s(L"SPbU for Intel, Head movement recognition using Perceptual SDK\r\n");

		// face
		PXCFaceAnalysis *faceAnalyzer = QueryFace();
		PXCFaceAnalysis::Landmark *landmark = faceAnalyzer->DynamicCast<PXCFaceAnalysis::Landmark>();
		
		// loop all faces
		m_face_render->ClearData();

		// face analyser
		// Create detector instance
		PXCFaceAnalysis::Detection *faceDetector=faceAnalyzer->DynamicCast<PXCFaceAnalysis::Detection>();
		if(!faceDetector) return false;
		
		for(int fidx = 0; ; fidx++)
		{
			pxcUID fid = 0;
			pxcU64 timeStamp = 0;
			pxcStatus sts = faceAnalyzer->QueryFace(fidx, &fid, &timeStamp);
			if(sts < PXC_STATUS_NO_ERROR)
				break;
			// no more faces

			// rect around the face
			PXCFaceAnalysis::Detection::Data face_data;
			faceDetector->QueryData(fid, &face_data);
			m_face_render->SetDetectionData(&face_data);

			// landmarks
			m_face_render->SetLandmarkData (landmark, fid);





			// work with gestures
			theLandMarkData* data_left_inner	= new theLandMarkData();
			theLandMarkData* data_left_outer	= new theLandMarkData();
			theLandMarkData* data_right_inner	= new theLandMarkData();
			theLandMarkData* data_right_outer	= new theLandMarkData();
			theLandMarkData* data_mouth_left	= new theLandMarkData();
			theLandMarkData* data_mouth_right	= new theLandMarkData();

			landmark->QueryLandmarkData(fid, PXCFaceAnalysis::Landmark::LABEL_LEFT_EYE_INNER_CORNER,	data_left_inner);
			landmark->QueryLandmarkData(fid, PXCFaceAnalysis::Landmark::LABEL_LEFT_EYE_OUTER_CORNER,	data_left_outer);
			landmark->QueryLandmarkData(fid, PXCFaceAnalysis::Landmark::LABEL_RIGHT_EYE_INNER_CORNER,	data_right_inner);
			landmark->QueryLandmarkData(fid, PXCFaceAnalysis::Landmark::LABEL_RIGHT_EYE_OUTER_CORNER,	data_right_outer);
			landmark->QueryLandmarkData(fid, PXCFaceAnalysis::Landmark::LABEL_MOUTH_LEFT_CORNER,		data_left_inner);
			landmark->QueryLandmarkData(fid, PXCFaceAnalysis::Landmark::LABEL_MOUTH_RIGHT_CORNER,		data_mouth_right);
			
			pair<double, double> left_o		= pair<double, double>(data_left_outer->position.x, data_left_outer->position.y);
			pair<double, double> left_i		= pair<double, double>(data_left_inner->position.x, data_left_inner->position.y);
			pair<double, double> right_i	= pair<double, double>(data_right_inner->position.x, data_right_inner->position.y);
			pair<double, double> right_o	= pair<double, double>(data_right_outer->position.x, data_right_outer->position.y);

			// centers on face tiles, strange bug
			theCoords leftEye	= left_o;//getCenter(data_left_inner, data_left_outer);
			theCoords rightEye	= pair<int, int>(right_i.first, right_o.second); //getCenter(data_right_inner, data_right_outer);
			theCoords mouth		= getCenter(data_left_inner, data_mouth_right);
				
			wprintf_s(L"Left eye: %d, %d\n", leftEye.first, leftEye.second);
			wprintf_s(L"Right eye: %d, %d\n", rightEye.first, rightEye.second);
			wprintf_s(L"Mouth: %d, %d\n", mouth.first, mouth.second);

			//double eye_angle = CalculateAngle(leftEye, rightEye);
			//wprintf_s(L"Eyes angle: %d\n", (int)eye_angle);

			// create face position
			FacePosition position_(leftEye, rightEye, mouth, theCoords(face_data.rectangle.x + face_data.rectangle.w/2, face_data.rectangle.y + face_data.rectangle.h/2));
			
			// add face position to gesturedetector
			mMyDetector.AddPoision(position_);
			mMyDetector.Process();
			// print amplit_incline
			int amplit_incline = mMyDetector.getAmplitudeIncline();
			wprintf_s(L"amplit_incline: %d\n", amplit_incline);
			//print incline_history
			string incline_history = mMyDetector.getInclineHistory();
			const char* szName = incline_history.c_str();
			printf(szName);
			wprintf_s(L"\n\n");

		}
		return (m_face_render->RenderFrame(QueryImage(PXCImage::IMAGE_TYPE_COLOR)) );
	}
 
protected: 
	GestureDetector mMyDetector;
	FaceRender* m_face_render;
};
