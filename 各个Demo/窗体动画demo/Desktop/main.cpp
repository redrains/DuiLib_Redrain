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

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetCurrentPath(CPaintManagerUI::GetInstancePath());
	CPaintManagerUI::SetResourcePath(L"Skin");

	CFrameWnd *pFrame = new CFrameWnd(_T("MainWnd.xml"));
	pFrame->Create(NULL, _T("Redrain´°Ìå¶¯»­demo"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pFrame->ShowModal();
	delete pFrame;

	::CoUninitialize();
	return 0;
}
