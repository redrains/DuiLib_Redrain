#include "duilib.h"

CFrameWnd::CFrameWnd( LPCTSTR pszXMLPath )
	:  m_strXMLPath(pszXMLPath)
{

}

LPCTSTR CFrameWnd::GetWindowClassName() const
{
	return _T("MainWnd");
}

CDuiString CFrameWnd::GetSkinFile()
{
	return m_strXMLPath;
}

CDuiString CFrameWnd::GetSkinFolder()
{
	return _T("Skin");
}

// 	UILIB_RESOURCETYPE CFrameWnd::GetResourceType() const
// 	{
// 		return UILIB_ZIPRESOURCE;
// 	}
// 
// 
// 	LPCTSTR CFrameWnd::GetResourceID() const
// 	{
// 		return MAKEINTRESOURCE(IDR_ZIPRES1);
// 	}

void CFrameWnd::InitWindow()
{
	CenterWindow();
}

LRESULT CFrameWnd::OnDPIChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	///The HIWORD of the wParam contains the Y-axis value of the new dpi of the window. 
	//The LOWORD of the wParam contains the X-axis value of the new DPI of the window. 
	//For example, 96, 120, 144, or 192. 
	//The values of the X-axis and the Y-axis are identical for Windows apps.

	g_Dpi.SetScale(LOWORD(wParam));  // Set the new DPI, retrieved from the wParam

	int g_dpi = HIWORD(wParam);


	RECT* const prcNewWindow = (RECT*)lParam;
	SetWindowPos(m_hWnd,
		NULL,
		prcNewWindow->left,
		prcNewWindow->top,
		prcNewWindow->right - prcNewWindow->left,
		prcNewWindow->bottom - prcNewWindow->top,
		SWP_NOZORDER | SWP_NOACTIVATE);
	bHandled = false;

	/*INT iBorderWidth100 = 5;
	FLOAT fscale = (float)g_dpi / USER_DEFAULT_SCREEN_DPI;
	INT iBorderWidth = iBorderWidth100 * fscale;
	double zoomLevel = log(fscale) / log(1.2);
	m_pClientHandler->GetBrowser()->GetHost()->SetZoomLevel(zoomLevel);


	if (fscale > 1) {
		m_pClientHandler->GetBrowser()->GetMainFrame()->ExecuteJavaScript("enableHiDPIStyle(true)", "", 0);
	}
	else {
		m_pClientHandler->GetBrowser()->GetMainFrame()->ExecuteJavaScript("enableHiDPIStyle(false)", "", 0);
	}

*/


	return 0;
}

void CFrameWnd::Notify( TNotifyUI& msg )
{

	if( msg.sType == _T("click") ) 
	{	
		if( msg.pSender->GetName() == _T("btnMenu") ) 
		{
			CPoint point(0, 0);
			GetCursorPos(&point);
			point.x += 5;
			point.y -= 5;
			CMenuWnd* pMenu = CMenuWnd::CreateMenu(NULL, _T("menutest.xml"), point, &m_PaintManager, &m_MenuCheckInfo, eMenuAlignment_Bottom);

			//左侧打开菜单
			//CMenuWnd* pMenu = CMenuWnd::CreateMenu(NULL, _T("menutest.xml"), point, &m_PaintManager, &m_MenuCheckInfo, eMenuAlignment_Right );
			//左上侧打开菜单
			//CMenuWnd* pMenu = CMenuWnd::CreateMenu(NULL, _T("menutest.xml"), point, &m_PaintManager, &m_MenuCheckInfo, eMenuAlignment_Right | eMenuAlignment_Bottom);

			// 先获取到根项，然后就可以使用rootMenu插到到菜单内的任意子菜单项，然后做添加删除操作
			CMenuUI* rootMenu = pMenu->GetMenuUI();
			if (rootMenu != NULL)
			{
				CMenuElementUI* pNew = new CMenuElementUI;
				pNew->SetName(_T("Menu_Dynamic"));
				pNew->SetText(_T("动态一级菜单"));
				pNew->SetShowExplandIcon(true);
				pNew->SetIcon(_T("WebSit.png"));
				pNew->SetIconSize(16,16);


				CMenuElementUI* pSubNew = new CMenuElementUI;
				pSubNew->SetText(_T("动态二级菜单"));
				pSubNew->SetName(_T("Menu_Dynamic"));
				pSubNew->SetIcon(_T("Virus.png"));
				pSubNew->SetIconSize(16,16);
				pNew->Add(pSubNew);
				

				rootMenu->Add(pNew);


				CMenuElementUI* pNew2 = new CMenuElementUI;
				pNew2->SetName(_T("Menu_Dynamic"));
				pNew2->SetText(_T("动态一级菜单2"));
				rootMenu->AddAt(pNew2,2);
			}

			// 动态添加后重新设置菜单的大小
			pMenu->ResizeMenu();
		}
		else if (msg.pSender->GetName() == _T("DPI96") )
		{
			
			setDPI(96);
			
		}
		else if (msg.pSender->GetName() == _T("DPI144"))
		{
			setDPI(144);
			
			
		}
		else if (msg.pSender->GetName() == _T("DPI192"))
		{
			setDPI(192);
			
		}

		
	}

	__super::Notify(msg);
}

void CFrameWnd::setDPI(int DPI) {


	int scale1=g_Dpi.GetScale();
	g_Dpi.SetScale(DPI);
	int scale2= g_Dpi.GetScale();



	RECT rcWnd;
	::GetWindowRect(m_hWnd, &rcWnd);
	RECT rc = rcWnd;
	
	rc.right = rcWnd.left + (rcWnd.right - rcWnd.left) * scale2 / scale1;
	rc.bottom = rcWnd.top + (rcWnd.bottom - rcWnd.top) * scale2 / scale1;
	
	
	
	RECT* const prcNewWindow = &rc;
	SetWindowPos(m_hWnd,
		NULL,
		prcNewWindow->left,
		prcNewWindow->top,
		prcNewWindow->right - prcNewWindow->left,
		prcNewWindow->bottom - prcNewWindow->top,
		SWP_NOZORDER | SWP_NOACTIVATE);

	if (m_PaintManager.GetRoot() != NULL) m_PaintManager.GetRoot()->NeedUpdate();
}

 LRESULT CFrameWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
 {
	 if (uMsg == WM_MENUCLICK)
	 {
		 CDuiString *strMenuName = (CDuiString*)wParam;
		 BOOL bChecked = (BOOL)lParam;		 

		 if ( *strMenuName == _T("Menu_Test1")) 
		 {
			 if (bChecked)
			 {
				 MessageBox(m_hWnd, L"你选中Menu_Test1", L"", 0);
			 } 
			 else
			 {
				 MessageBox(m_hWnd, L"你取消Menu_Test1", L"", 0);
			 }			 
		 }
		 else if ( *strMenuName == _T("Menu_Test2")) 
		 {
				MessageBox(m_hWnd, L"你单击了Menu_Test2", L"", 0);		 
		 }
		 else if ( *strMenuName == _T("Menu_Test3")) 
		 {
			 if (bChecked)
			 {
				 MessageBox(m_hWnd, L"你选中Menu_Test3", L"", 0);
			 } 
			 else
			 {
				 MessageBox(m_hWnd, L"你取消Menu_Test3", L"", 0);
			 }			 
		 }
		 else if ( *strMenuName == _T("Menu_Dynamic")) 
		 {
			 MessageBox(m_hWnd, L"你单击了动态添加菜单", L"", 0);		 
		 }

	 }

	 if (uMsg == WM_DPICHANGED) {

		 OnDPIChanged(uMsg, wParam, lParam, bHandled);
	 }


	 bHandled = false;
	 return 0;
 }