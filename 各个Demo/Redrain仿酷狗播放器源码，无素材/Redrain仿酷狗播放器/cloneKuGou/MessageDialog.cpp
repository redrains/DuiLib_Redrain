#include "duilib.h"


CMessageDialog::CMessageDialog() :
	m_pButtonOK(NULL),
	m_pButtonCancel(NULL)
{
}

CMessageDialog::~CMessageDialog()
{

}

LPCTSTR CMessageDialog::GetWindowClassName() const 
{ 
	return _T("UIMessageDialog");
}

UINT CMessageDialog::GetClassStyle() const 
{ 
	return CS_DBLCLKS;
}


void CMessageDialog::OnFinalMessage(HWND /*hWnd*/) 
{ 
	return;
}


DWORD CMessageDialog::GetBkColor() const
{
	CControlUI* background = m_pm.FindControl(kWndBackGround);
	if (background != NULL)
		return background->GetBkColor();
	return 0;
}

void CMessageDialog::SetBkColor(DWORD dwBackColor)
{
	CControlUI* background = m_pm.FindControl(kWndBackGround);
	if (background != NULL)
	{
		background->SetBkImage(_T(""));
		background->SetBkColor(dwBackColor);
		background->NeedUpdate();
	}

}

void CMessageDialog::SetBkImage(CDuiString strBkImage)
{
	CControlUI* background = m_pm.FindControl(kWndBackGround);
	if (background != NULL)
	{
		TCHAR szBuf[MAX_PATH] = {0};
		_stprintf_s(szBuf, MAX_PATH - 1, _T("file='%s' source='0,0,1000,30'"), strBkImage.GetData());
		background->SetBkImage(szBuf);
	}
}

LPCTSTR CMessageDialog::GetBkImage()
{
	CControlUI* background = m_pm.FindControl(kWndBackGround);
	if (background != NULL)
		return background->GetBkImage();
	return _T("");
}

void CMessageDialog::Init()
{
	m_pButtonOK = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
	m_pButtonCancel = static_cast<CButtonUI*>(m_pm.FindControl(_T("cancelbtn")));

	SetBkColor(m_pMainWnd->GetBkColor());
	SetBkImage(m_pMainWnd->GetBkImage());

	CWndShadow::Initialize(m_pm.GetInstance());
	m_WndShadow.Create(m_hWnd , true);
	m_WndShadow.SetSize(4);
	m_WndShadow.SetPosition(1, 1);

}

void CMessageDialog::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click") ) 
	{
		if( msg.pSender->GetName() == _T("closebtn")) {
			Close(IDOK);
			return; 
		}
		else if( msg.pSender->GetName() == _T("cancelbtn")) {
			Close(IDCANCEL);
			return; 
		}
	}

}

LRESULT CMessageDialog::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
//	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	CDialogBuilder builder;

	CControlUI *pRoot = builder.Create(m_strXMLPath.GetData(), (UINT)0,  0, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CMessageDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CMessageDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CMessageDialog::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if( ::IsIconic(*this) ) bHandled = FALSE;
    return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CMessageDialog::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMessageDialog::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMessageDialog::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = m_pm.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
			if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
				_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
				_tcscmp(pControl->GetClass(), _T("TextUI")) != 0 )
				return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT CMessageDialog::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_pm.GetRoundCorner();
#if defined(WIN32) && !defined(UNDER_CE)
	if( !::IsIconic(*this) /*&& (szRoundCorner.cx != 0 || szRoundCorner.cy != 0)*/ ) {
		CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		rcWnd.right++; rcWnd.bottom++;
		HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this, hRgn, TRUE);
		::DeleteObject(hRgn);
	}
#endif
	bHandled = FALSE;
    return 0;
}

LRESULT CMessageDialog::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	lpMMI->ptMaxSize.x		= rcWork.right;
	lpMMI->ptMaxSize.y		= rcWork.bottom;

	bHandled = FALSE;
	return 0;
}

LRESULT CMessageDialog::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
	if( wParam == SC_CLOSE ) {
		bHandled = TRUE;
		SendMessage(WM_CLOSE);
		return 0;
	}
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if( ::IsZoomed(*this) != bZoomed ) {
	}
	return lRes;
}

LRESULT CMessageDialog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
	case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
	case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
	case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
	case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
	case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
	default:
	bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

UINT CMessageDialog::DuiMessageBox(CFrameWnd *pMainWnd, LPCTSTR lpText, LPCTSTR lpCaption ,UINT uType, UINT uIcon)
{
	m_pMainWnd = pMainWnd;
	m_strXMLPath = _T("xmls\\MessageBoxDialog.xml");
	Create(m_pMainWnd->GetHWND(), _T(""), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);

	if ( uType == MB_OK)
	{
		if ( m_pButtonCancel != NULL )
		{
			m_pButtonCancel->SetVisible(false);
		}

	}
	else if( uType == MB_OKCANCEL )
	{	
		//保持默认xml配置
	}
	else
		return 0;

	CContainerUI* pTextLayout = static_cast<CContainerUI*>(m_pm.FindControl(_T("TextLayout")));
	if (pTextLayout != NULL)
	{
		switch(uIcon)
		{
		case MB_ICONASTERISK:
			{
				pTextLayout->SetBkImage(_T("file='UI\\MessageDialog\\BK.png' source='222,0,333,120' corner='111,0,0,0'"));
				break;
			}
		case MB_ICONQUESTION:
			{
				pTextLayout->SetBkImage(_T("file='UI\\MessageDialog\\BK.png' source='111,0,222,120' corner='111,0,0,0'"));
				break;
			}
		case MB_ICONWARNING:
			{
				pTextLayout->SetBkImage(_T("file='UI\\MessageDialog\\BK.png' source='0,0,111,120' corner='111,0,0,0'"));
				break;
			}
		case MB_ICONERROR:
			{
				pTextLayout->SetBkImage(_T("file='UI\\MessageDialog\\BK.png' source='222,0,333,120' corner='111,0,0,0'"));
				break;
			}
		default:
			break;				
		}
	}

	CTextUI* pCaption_control = static_cast<CTextUI*>(m_pm.FindControl( _T("caption")));
	if (pCaption_control != NULL)
	{
		pCaption_control->SetText(lpCaption);
	}

	CTextUI* pText_control = static_cast<CTextUI*>(m_pm.FindControl( _T("text")));
	if (pText_control != NULL)
	{
		SIZE m_szClient = m_pm.GetClientSize();
		int nCTextUIWidth = m_szClient.cx - pTextLayout->GetChildPadding() - pTextLayout->GetInset().left - 15;
		pText_control->SetText(lpText);
		RECT rect;
		GetClientRect(m_hWnd, &rect);
		SIZE szSpace = { 0 };
		HFONT hOldFont =(HFONT)::SelectObject(m_pm.GetPaintDC(),m_pm.GetFont(0));
		::GetTextExtentPoint32(m_pm.GetPaintDC(), lpText, _tcslen(lpText), &szSpace);
		::SelectObject(m_pm.GetPaintDC(),(HGDIOBJ)hOldFont);
		rect.right = rect.right + szSpace.cx - nCTextUIWidth;
		if((rect.right - rect.left) < m_szClient.cx )
			rect.right = rect.left + m_szClient.cx;
		SetWindowPos (m_hWnd, NULL, rect.left, rect.top, rect.right , rect.bottom, SWP_SHOWWINDOW );

	}

	CenterWindow();
	UINT uRet = ShowModal();
	return uRet;
}


void CMessageDialog::DuiAboutBox(CFrameWnd *pMainWnd, LPCTSTR lpXMLPath)
{
	if (lpXMLPath == NULL)
		return;
	m_pMainWnd = pMainWnd;
	m_strXMLPath = lpXMLPath;

	Create(m_pMainWnd->GetHWND(), _T("关于此程序"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
	CenterWindow();
	ShowModal();
	
}