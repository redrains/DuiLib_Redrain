// UiLib_Demos.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MainWnd.h"

#include <GdiPlus.h>
#pragma comment( lib, "GdiPlus.lib" )
using namespace Gdiplus;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	EnableMemLeakCheck();
	CPaintManagerUI::SetInstance(hInstance);

	GdiplusStartupInput   gdiplusStartupInput;
	ULONG_PTR             gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	if( ::LoadLibrary(_T("d3d9.dll")) == NULL ) 
		::MessageBox(NULL, _T("加载 d3d9.dll 失败，一些特效可能无法显示！"), _T("信息提示"),MB_OK|MB_ICONWARNING);

	CMainWnd* pFrame = new CMainWnd();
	if(pFrame == NULL)
		return 0;

	pFrame->Create(NULL,_T("UiLib Demos"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 900, 600);
	pFrame->CenterWindow();

	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	GdiplusShutdown(gdiplusToken);

	_CrtDumpMemoryLeaks();
	return 0;
}