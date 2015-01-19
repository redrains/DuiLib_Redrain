#include "duilib.h"
#include "Controls\PopupWidget.h"

CSkinPickerDialog::CSkinPickerDialog(LPCTSTR pszXMLPath, CFrameWnd *pMainWnd) :
	m_strXMLPath(pszXMLPath),
	m_pMainWnd(pMainWnd),
	m_pSkinTab(NULL),
	m_pBtnRestore(NULL),
	m_pBtnListTrans(NULL),
	m_pOptSkinTrans(NULL),
	m_pBtnSkinTrans(NULL),
	m_pBtnPattet(NULL),
	m_pBtnCustom(NULL)
{
}

CSkinPickerDialog::~CSkinPickerDialog()
{

}

LPCTSTR CSkinPickerDialog::GetWindowClassName() const 
{ 
	return _T("UISkinPickerDialog");
}

UINT CSkinPickerDialog::GetClassStyle() const 
{ 
	return CS_DBLCLKS;
}


void CSkinPickerDialog::OnFinalMessage(HWND /*hWnd*/) 
{ 

	return;
}

void CSkinPickerDialog::Init()
{
	m_pSkinTab = static_cast<CAnimationTabLayoutUI*>(m_pm.FindControl(kSkinTab));
	m_pBtnRestore = static_cast<CButtonUI*>(m_pm.FindControl(kBtnRestore));
	m_pBtnListTrans = static_cast<CButtonUI*>(m_pm.FindControl(kBtnListTrans));
	m_pOptSkinTrans = static_cast<COptionUI*>(m_pm.FindControl(kOptSkinTrans));
	m_pBtnSkinTrans = static_cast<CButtonUI*>(m_pm.FindControl(kBtnSkinTrans));
	m_pBtnPattet = static_cast<CButtonUI*>(m_pm.FindControl(kBtnPattet));
	m_pBtnCustom = static_cast<CButtonUI*>(m_pm.FindControl(kBtnCustom));

	SetBkColor(m_pMainWnd->GetBkColor());
	SetBkImage(m_pMainWnd->GetBkImage());
	CDuiString szValue;
	szValue.SmallFormat(_T("%d%%"),m_pMainWnd->GetListTrans()* 100 / 255 );
	m_pBtnListTrans->SetText(szValue);
	m_pBtnListTrans->SetTag(m_pMainWnd->GetListTrans());

	szValue.SmallFormat(_T("%d%%"),m_pMainWnd->GetSkinTrans() * 100 / 150);
	m_pBtnSkinTrans->SetText(szValue);
	m_pBtnSkinTrans->SetTag(m_pMainWnd->GetSkinTrans());

	m_pOptSkinTrans->Selected(m_pMainWnd->GetIsSkinTrans());

	CWndShadow::Initialize(m_pm.GetInstance());
	m_WndShadow.Create(m_hWnd , true);
	m_WndShadow.SetSize(4);
	m_WndShadow.SetPosition(1, 1);

}

CControlUI* CSkinPickerDialog::CreateControl(LPCTSTR pstrClass) 
{
	if (_tcsicmp(pstrClass, kSkinPickerPictureItemInterface) == 0)
		return	new CSkinPikerPictureItemUI();

	return NULL;
}

DWORD CSkinPickerDialog::GetBkColor() const
{
	CControlUI* background = m_pm.FindControl(kWndBackGround);
	if (background != NULL)
		return background->GetBkColor();
	return 0;
}

void CSkinPickerDialog::SetBkColor(DWORD dwBackColor)
{
	CControlUI* background = m_pm.FindControl(kWndBackGround);
	if (background != NULL)
	{
		background->SetBkImage(_T(""));
		background->SetBkColor(dwBackColor);
		background->NeedUpdate();
	}

}

void CSkinPickerDialog::SetBkImage(CDuiString strBkImage)
{
	CControlUI* background = m_pm.FindControl(kWndBackGround);
	if (background != NULL)
	{
		TCHAR szBuf[MAX_PATH] = {0};
		_stprintf_s(szBuf, MAX_PATH - 1, _T("file='%s' source='0,0,1000,30'"), strBkImage.GetData());
		background->SetBkImage(szBuf);
	}
}

LPCTSTR CSkinPickerDialog::GetBkImage()
{
	CControlUI* background = m_pm.FindControl(kWndBackGround);
	if (background != NULL)
		return background->GetBkImage();
	return _T("");
}

void CSkinPickerDialog::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click") ) 
	{
		if( msg.pSender->GetName() == _T("closebtn")) {
			Close(IDOK);
			return; 
		}
		else if (msg.pSender->GetName() == kBtnListTrans)
		{
			CPopupWidget* pWidget = new CPopupWidget;
			CPoint point;
			RECT rc	= m_pBtnListTrans->GetPos();
			point.x = rc.left;
			point.y = rc.top;			
			pWidget->Init(_T("xmls\\Skin\\ListTrans.xml"), m_hWnd, m_pMainWnd->GetMainWndPaintManager(), point, CHANGE_LIST_TRANS, m_pBtnListTrans);
			 
			return;
		}
		else if (msg.pSender->GetName() == kBtnSkinTrans)
		{
			CPopupWidget* pWidget = new CPopupWidget;
			CPoint point;
			RECT rc	= m_pBtnSkinTrans->GetPos();
			point.x = rc.left;
			point.y = rc.top;			
			pWidget->Init(_T("xmls\\Skin\\ListTrans.xml"), m_hWnd, m_pMainWnd->GetMainWndPaintManager(), point, CHANGE_MAINWND_TRANS, m_pBtnSkinTrans);

			return;
		}
		else if (msg.pSender->GetName() == kBtnCustom)
		{
			vector<CDuiString> names;
			if (CFileUtil::OpenFile(L"JEPG Files(*.jpg)\0*.jpg\0BMP Files\0*.bmp\0PNG Files\0*.png\0\0",GetHWND(),names, false))
			{
				SetBkImage(names[0]);
				m_pMainWnd->SetBkImage(names[0]);
			}	
			
		}
		else if (msg.pSender->GetName() == kSkinPickerPicutreItem)
		{
			CSkinPikerPictureItemUI * pItem = static_cast<CSkinPikerPictureItemUI*>(msg.pSender);
			CDuiString strBkImage = pItem->GetBkImage();
			strBkImage.Replace(_T("small"), _T(""));
			SetBkImage(strBkImage);
			m_pMainWnd->SetBkImage(strBkImage);
		}
		else if (msg.pSender->GetName() == kBtnRestore)
		{
			m_pMainWnd->LoadDefaultState();
			SetBkColor(m_pMainWnd->GetBkColor());
			SetBkImage(m_pMainWnd->GetBkImage());

			m_pBtnListTrans->SetTag(kMainWndDefaultListTrans);
			m_pBtnListTrans->SetText(_T("30%"));

			m_pBtnSkinTrans->SetEnabled(false);
			m_pMainWnd->SetIsSkinTrans(false);
			m_pBtnSkinTrans->SetText(_T("0%"));
			m_pBtnSkinTrans->SetTag(0);
			CenterWindow();
		}
		else if (msg.pSender->GetName() == kBtnPattet)
		{
			CPalletDialog *pPalletDlg = new CPalletDialog(_T("xmls\\PalletDialog.xml"),this);
			pPalletDlg->Create(m_hWnd,_T("Redran调色板"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
			pPalletDlg->CenterWindow();
			if(pPalletDlg->ShowModal() == IDOK)
			{
				SetBkColor(pPalletDlg->GetSelectColor());
				m_pMainWnd->SetBkColor(pPalletDlg->GetSelectColor());
			}			
			delete	pPalletDlg;
		}
		

	}
	else if(msg.sType == _T("selectchanged"))
	{
		if( msg.pSender->GetName() == kOfficialSkin) {
			 m_pSkinTab->SelectItem(0);
		}
		else if( msg.pSender->GetName() == kMySkin) {
			m_pSkinTab->SelectItem(1);
		}
		else if( msg.pSender->GetName() == kOptSkinTrans) {
			if(static_cast<COptionUI*>(msg.pSender)->IsSelected())
			{
				m_pBtnSkinTrans->SetEnabled(true);
				m_pMainWnd->SetIsSkinTrans(true);
			}
			else
			{
				m_pBtnSkinTrans->SetEnabled(false);
				m_pMainWnd->SetIsSkinTrans(false);
				m_pBtnSkinTrans->SetText(_T("0%"));
				m_pBtnSkinTrans->SetTag(0);
			}
		}
	}

}

LRESULT CSkinPickerDialog::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
//	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	CDialogBuilder builder;

	CControlUI *pRoot = builder.Create(m_strXMLPath.GetData(), (UINT)0, this, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CSkinPickerDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CSkinPickerDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CSkinPickerDialog::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if( ::IsIconic(*this) ) bHandled = FALSE;
    return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CSkinPickerDialog::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CSkinPickerDialog::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CSkinPickerDialog::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CSkinPickerDialog::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CSkinPickerDialog::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CSkinPickerDialog::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CSkinPickerDialog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

