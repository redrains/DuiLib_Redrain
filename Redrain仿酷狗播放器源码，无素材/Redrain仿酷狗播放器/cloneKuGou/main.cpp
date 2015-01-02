/********************************************************************
	created:	2014/11/12
	author:		Redrain
	e-mail:		redrains@qq.com
	qq:			491646717
	website:	http://blog.csdn.net/zhuhongshu?viewmode=contents
	purpose:	这是Redrain仿酷狗音乐播放器的完整代码，目的是帮助更多使用DuiLib的朋友学习这个库，
				如果代码有bug，或者对程序有疑问，可以联系我个人QQ或者QQ群，我经常在DuiLib QQ群活动，
				这个代码中包含了webkit内核浏览器、音乐播放类、菜单类、换肤功能等等
*********************************************************************/

#include "duilib.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	::OleInitialize(NULL);
    ::CoInitialize(NULL);
	CWkeWebkitUI::WkeWebkit_Init();

    CPaintManagerUI::SetInstance(hInstance);
	CFrameWnd *pFrame = new CFrameWnd(_T("xmls\\Main_dlg.xml"));
    pFrame->Create(NULL, _T("Redrain音乐盒"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    pFrame->ShowModal();

	CWkeWebkitUI::WkeWebkit_Shutdown();
    delete pFrame;
    ::CoUninitialize();
	::OleUninitialize();
    return 0;
}
