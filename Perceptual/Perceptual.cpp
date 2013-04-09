#include "stdafx.h"
#include "util_render.h"
#include "util_pipeline.h"

int _tmain(int argc, _TCHAR* argv[]) 
{
	UtilPipeline pipeline;
	pipeline.EnableImage(PXCImage::COLOR_FORMAT_RGB32);
	pipeline.EnableImage(PXCImage::COLOR_FORMAT_RGB32, 640, 480); /* 2 */

	pipeline.Init();
	UtilRender color_render(L"Color Stream");
	for(;;) 
	{
		if(!pipeline.AcquireFrame(true)) 
			break;
		PXCImage *color_image = pipeline.QueryImage(PXCImage::IMAGE_TYPE_COLOR);
		if(!color_render.RenderFrame(color_image)) 
			break;
		pipeline.ReleaseFrame();
	}
	pipeline.Close();
	return 0;
}
