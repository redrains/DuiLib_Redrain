#include "duilib.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    ::CoInitialize(NULL);
	CWkeWebkitUI::WkeWebkit_Init();

    CPaintManagerUI::SetInstance(hInstance);
//	CPaintManagerUI::SetResourceZip(_T("Main_dlg.zip"));
	CFrameWnd *pFrame = CFrameWnd::MainWnd();
    pFrame->Create(NULL, _T("360度全景照相技术"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    pFrame->ShowModal();

	CWkeWebkitUI::WkeWebkit_Shutdown();
//    delete pFrame;
    ::CoUninitialize();
    return 0;
}
