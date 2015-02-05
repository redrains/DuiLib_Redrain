#include "stdafx.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	::CoInitialize(NULL);

	CPaintManagerUI::SetInstance(hInstance);
	//	CPaintManagerUI::SetResourceZip(_T("Skin.zip"));
	CFrameWnd *pFrame = new CFrameWnd(_T("MainWnd.xml"));
	pFrame->Create(NULL, _T("Redrain°ëÍ¸Ã÷´°Ìådemo"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pFrame->ShowModal();

	delete pFrame;
	::CoUninitialize();
	return 0;
}
