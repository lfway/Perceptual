#include "stdafx.h"
//#include <string>

#include "pxcsession.h"
#include "pxccapture.h"
#include "pxcsmartptr.h"

#include <math.h>

#include "PerceptualCore.h"

void CALLBACK MyCallbackFunc(std::string value)
{
    printf("callback: %s\n", value.c_str());
}

int _tmain(int argc, _TCHAR* argv[]) 
{
	ISuperClass* processor = Inittt();
	//Register_Callback(MyCallbackFunc);
	return 0;
}





/*




int wmain1(int argc,wchar_t * argv[])
{
	
	// ������� ������
	PXCSmartPtr<PXCSession> session;
	pxcStatus sts = PXCSession_Create(&session);
	if(sts<PXC_STATUS_NO_ERROR) 
	{
		wprintf_s(L"Failed to create the SDK session\n");
		return 3;
	}

	// �������������� ������� ��� ������
	UtilCmdLine cmdl(session);
	if(!cmdl.Parse(L"-sdname-nframes-file-record",argc,argv))
		return 3;
	
	// Init Face analyzer
	PXCSmartPtr<PXCFaceAnalysis> faceAnalyzer;
	sts=session->CreateImpl(cmdl.m_iuid, PXCFaceAnalysis::CUID,(void**)&faceAnalyzer);
	if(sts<PXC_STATUS_NO_ERROR) 
	{
		wprintf_s(L"Failed to locate a face module");
		return 3;
	}
	
	// Retrieve the input requirements
	PXCFaceAnalysis::ProfileInfo faInfo;
	faceAnalyzer->QueryProfile(0, &faInfo);

	// ���������� ������
	UtilCaptureFile capture(session,cmdl.m_recordedFile,cmdl.m_bRecord);
	if(cmdl.m_sdname)
		capture.SetFilter(cmdl.m_sdname); /*L"Integrated Camera"* /
	sts=capture.LocateStreams(&faInfo.inputs);
	if(sts<PXC_STATUS_NO_ERROR)
	{
		wprintf_s(L"Failed to locate an input device that matches input for face analysis\n");
		return 3;
	}
	faceAnalyzer->SetProfile(&faInfo);

	
	// Create detector instance
	PXCFaceAnalysis::Detection *faceDetector=faceAnalyzer->DynamicCast<PXCFaceAnalysis::Detection>();
	if(!faceDetector) 
	{
		wprintf_s(L"Failed to locate the face detection functionality\n");
		return 3;
	}
	
	// Create landmark instance
	PXCFaceAnalysis::Landmark* landmarkDetector = landmarkDetector = faceAnalyzer->DynamicCast<PXCFaceAnalysis::Landmark>();
	if(!landmarkDetector)
	{
		wprintf_s(L"Failed to locate the face landmark functionality\n");
		return 3;
	}
	
	// Set landmark profile
	PXCFaceAnalysis::Landmark::ProfileInfo lInfo={0};
	landmarkDetector->QueryProfile(0, &lInfo);
	landmarkDetector->SetProfile(&lInfo);

	
	// Create Renderer
	PXCSmartPtr<FaceRender> faceRender(new FaceRender(L"Landmark Detection Sample"));
	pxcU32 fnum;
	for(fnum=0;fnum<cmdl.m_nframes;fnum++)
	{
	
		PXCSmartArray<PXCImage> images;
		PXCSmartSPArray sps(2);
		///* read and process frame * /
		sts = capture.ReadStreamAsync(images,&sps[0]);
		if(sts<PXC_STATUS_NO_ERROR)
			break;
		// EOF
		sts = faceAnalyzer->ProcessImageAsync(images, &sps[1]);
		sts = sps.SynchronizeEx();
		if(sps[0]->Synchronize(0)<PXC_STATUS_NO_ERROR)
			break;
		// EOF
		// loop all faces
		faceRender->ClearData();
		for(int fidx = 0; ; fidx++) 
		{
			pxcUID fid = 0;
			pxcU64 timeStamp = 0;
			sts = faceAnalyzer->QueryFace(fidx, &fid, &timeStamp);
			if(sts < PXC_STATUS_NO_ERROR)
				break;
			// no more faces
			// Query face detection results
			PXCFaceAnalysis::Detection::Data face_data;
			faceDetector->QueryData(fid, &face_data);
			faceRender->SetDetectionData(&face_data);
			// Query landmark points
			faceRender->SetLandmarkData(landmarkDetector, fid);
			
			theLandMarkData* data_left_inner	= new theLandMarkData();
			theLandMarkData* data_left_outer	= new theLandMarkData();
			theLandMarkData* data_right_inner	= new theLandMarkData();
			theLandMarkData* data_right_outer	= new theLandMarkData();
			theLandMarkData* data_mouth_left	= new theLandMarkData();
			theLandMarkData* data_mouth_right	= new theLandMarkData();

			landmarkDetector->QueryLandmarkData(fid, PXCFaceAnalysis::Landmark::LABEL_LEFT_EYE_INNER_CORNER,	data_left_inner);
			landmarkDetector->QueryLandmarkData(fid, PXCFaceAnalysis::Landmark::LABEL_LEFT_EYE_OUTER_CORNER,	data_left_outer);
			landmarkDetector->QueryLandmarkData(fid, PXCFaceAnalysis::Landmark::LABEL_RIGHT_EYE_INNER_CORNER,	data_right_inner);
			landmarkDetector->QueryLandmarkData(fid, PXCFaceAnalysis::Landmark::LABEL_RIGHT_EYE_OUTER_CORNER,	data_right_outer);
			landmarkDetector->QueryLandmarkData(fid, PXCFaceAnalysis::Landmark::LABEL_MOUTH_LEFT_CORNER,		data_left_inner);
			landmarkDetector->QueryLandmarkData(fid, PXCFaceAnalysis::Landmark::LABEL_MOUTH_RIGHT_CORNER,		data_mouth_right);
			
			theCoords leftEye	= getCenter(data_left_inner, data_left_outer);
			theCoords rightEye	= getCenter(data_right_inner, data_right_outer);
			theCoords mouth		= getCenter(data_left_inner, data_mouth_right);

			faceRender->PrintLandmarkData(landmarkDetector, fid);
			
			//wprintf_s(L"Left eye = %d, %d\n", leftEye.first, leftEye.second);
			//wprintf_s(L"Right eye = %d, %d\n", rightEye.first, rightEye.second);
			//wprintf_s(L"Mouth = %d, %d\n", mouth.first, mouth.second);
			//wprintf_s(L"Eyes dist = %d\n", getDistance(leftEye, rightEye));

			//wprintf_s(L"timestamp=%I64d, frame=%d\n", timeStamp, fnum);
		}
		//Commit this to image output decine
		if(!faceRender->RenderFrame(images[0]))break;
		
	}
	
	return 0;
}

void easyPerceptual()
{
	UtilPipeline pipeline;
	pipeline.EnableImage(PXCImage::COLOR_FORMAT_RGB32);
	pipeline.EnableImage(PXCImage::COLOR_FORMAT_RGB32, 640, 480); // 2 
	pipeline.Init();

	UtilRender color_render(L"Color Stream");

	for(;;) 
	{
		// Image capturing
		if(!pipeline.AcquireFrame(true)) 
			break;
		PXCImage *color_image = pipeline.QueryImage(PXCImage::IMAGE_TYPE_COLOR);

		// Image output
		if(!color_render.RenderFrame(color_image)) 
			break;
		pipeline.ReleaseFrame();
	}
	pipeline.Close();
}

class MainClass
{
public:
	MainClass()
	{
		pipeline1 = new FacePipeline();
		pipeline1->EnableImage(PXCImage::COLOR_FORMAT_RGB32, 640, 480);
		pipeline1->LoopFrames();
	}
	~MainClass()
	{
		delete pipeline1;
	}
private:
	FacePipeline* pipeline1;
};

*/