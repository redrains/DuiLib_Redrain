#include "stdafx.h"
#include "FrameWnd.h"

#ifdef _DEBUG
#pragma comment(lib, "DuiLib_d.lib")
#else
#pragma comment(lib, "DuiLib.lib")
#endif

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    ::CoInitialize(NULL);
	CWkeWebkitUI::WkeWebkit_Init();

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetCurrentPath(CPaintManagerUI::GetInstancePath());
	CPaintManagerUI::SetResourcePath(L"Skin");
//	CPaintManagerUI::SetResourceZip(_T("Main_dlg.zip"));

	CFrameWnd *pFrame = CFrameWnd::MainWnd();
    pFrame->Create(NULL, _T("Redrain wke demo"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    pFrame->ShowModal();

	CWkeWebkitUI::WkeWebkit_Shutdown();
    ::CoUninitialize();
    return 0;
}
