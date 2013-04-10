#include "stdafx.h"

#include "pxcsession.h"
#include "pxccapture.h"
#include "pxcsmartptr.h"

#include "util_render.h"
#include "util_pipeline.h"

#include "face_render.h"
#include "util_capture_file.h"
#include "util_cmdline.h"
#include "pxcface.h"


int wmain(int argc,wchar_t * argv[])
{
	
	// Создаем сессию
	PXCSmartPtr<PXCSession> session;
	pxcStatus sts = PXCSession_Create(&session);
	if(sts<PXC_STATUS_NO_ERROR) 
		wprintf_s(L"Failed to create the SDK session\n");return 3;

	// Инициализируем консоль для сессии
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

	// Подключаем камеру
	UtilCaptureFile capture(session,cmdl.m_recordedFile,cmdl.m_bRecord);
	if(cmdl.m_sdname)
		capture.SetFilter(cmdl.m_sdname); /*L"Integrated Camera"*/
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
	int fnum;
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
			faceRender->PrintLandmarkData(landmarkDetector, fid);
			wprintf_s(L"timestamp=%I64d, frame=%d\n", timeStamp, fnum);
		}
		if(!faceRender->RenderFrame(images[0]))break;
		
	}
	
	return 0;
}



/*#include "stdafx.h"
#include "util_render.h"
#include "util_pipeline.h"
#include "face_render.h"


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
		/* face * /
		PXCFaceAnalysis *faceAnalyzer = QueryFace();
		PXCFaceAnalysis::Landmark *landmark = faceAnalyzer->DynamicCast<PXCFaceAnalysis::Landmark>();
		// loop all faces
		m_face_render->ClearData();
		for(int fidx = 0; ; fidx++)
		{
			pxcUID fid = 0;
			pxcU64 timeStamp = 0;
			pxcStatus sts = faceAnalyzer->QueryFace(fidx, &fid, &timeStamp);
			if(sts < PXC_STATUS_NO_ERROR)
				break;
			// no more faces
			m_face_render->SetLandmarkData (landmark, fid);
		}
		return (m_face_render->RenderFrame(QueryImage(PXCImage::IMAGE_TYPE_COLOR)) );
	}

protected: 
	FaceRender* m_face_render;
};


int _tmain(int argc, _TCHAR* argv[]) 
{

	UtilPipeline pipeline;
	pipeline.EnableImage(PXCImage::COLOR_FORMAT_RGB32);
	pipeline.EnableImage(PXCImage::COLOR_FORMAT_RGB32, 640, 480); /* 2 * /
	pipeline.Init();

	UtilRender color_render(L"Color Stream");

	FacePipeline* pipeline1 = new FacePipeline();
	/*pipeline1->LoopFrames();
	delete pipeline1;
	return 0;* /

	for(;;) 
	{
		// Получаем картинку
		if(!pipeline.AcquireFrame(true)) 
			break;
		PXCImage *color_image = pipeline.QueryImage(PXCImage::IMAGE_TYPE_COLOR);

		

		// Выводим картинку
		if(!color_render.RenderFrame(color_image)) 
			break;
		pipeline.ReleaseFrame();
	}
	pipeline.Close();
	return 0;
}
*/