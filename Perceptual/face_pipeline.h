// example from intel site

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
		// face
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
