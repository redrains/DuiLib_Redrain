#include "duilib.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    ::CoInitialize(NULL);

	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);


	HMONITOR hMonitor;
	POINT    pt;
	UINT     dpix = 0, dpiy = 0;
	HRESULT  hr = E_FAIL;

	// Get the DPI for the main monitor, and set the scaling factor
	pt.x = 1;
	pt.y = 1;
	hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
	hr = GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy);

	if (hr != S_OK)
	{
		MessageBox(NULL, (LPCWSTR)L"GetDpiForMonitor failed", (LPCWSTR)L"Notification", MB_OK);
		return FALSE;
	}
	/*

	96 DPI = 100% scaling
	120 DPI = 125% scaling
	144 DPI = 150% scaling
	192 DPI = 200% scaling
	*/


	g_Dpi.SetScale(dpix);
	g_Dpi.SetScale(192);
	g_Dpi.GetAwareness();

    CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));
	//CPaintManagerUI::SetResourcePath(L"skin");
	CFrameWnd *pFrame = new CFrameWnd(_T("Main_dlg.xml"));
	
    pFrame->Create(NULL, _T("Redrain²Ëµ¥Demo"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    pFrame->ShowModal();

    delete pFrame;
    ::CoUninitialize();
    return 0;
}
