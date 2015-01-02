#include "duilib.h"

CMusicInfoDialog::CMusicInfoDialog() :m_pMedia(NULL),m_hParentWnd(NULL)
{
	m_pt.x = 0;
	m_pt.y = 0;
}

CMusicInfoDialog::~CMusicInfoDialog()
{
}


LPCTSTR CMusicInfoDialog::GetWindowClassName() const 
{ 
	return _T("MusicInfoDialog");
}

UINT CMusicInfoDialog::GetClassStyle() const 
{ 
	return CS_DBLCLKS;
}

void CMusicInfoDialog::OnFinalMessage(HWND /*hWnd*/) 
{ 
	 delete this;
}


void CMusicInfoDialog::Init( LPCTSTR pszXMLPath, HWND hWndParent, LONG ptx)
{  
	m_strXMLPath = pszXMLPath;
	m_hParentWnd = hWndParent;
	m_pt.x = ptx;

	Create(hWndParent, _T("MusicInfo"), UI_WNDSTYLE_DIALOG /*& ~WS_VISIBLE*/, WS_EX_TOOLWINDOW);

}

void CMusicInfoDialog::ResetWindow(LONG pty, MEDIA* pMedia)
{
	m_pMedia = pMedia;
	if (m_pMedia == NULL)
		return;

	CDuiString strTmp;
	CControlUI* plogo = static_cast<CControlUI*>(m_pm.FindControl(kMusicInfoLogo));
	if (plogo != NULL)
	{
		plogo->SetBkImage(m_pMedia->logoPath.GetData());
	}

	CLabelUI* music_name = static_cast<CLabelUI*>(m_pm.FindControl(kMusicInfoMusicName));
	if (music_name != NULL )
	{
		music_name->SetText(m_pMedia->name.GetData());
	}

	CLabelUI* music_size = static_cast<CLabelUI*>(m_pm.FindControl(kMusicInfoMusicSize));
	if (music_size != NULL )
	{
		strTmp = _T("大小:");
		music_size->SetText(strTmp + m_pMedia->size.GetData());
	}

	CLabelUI* music_format = static_cast<CLabelUI*>(m_pm.FindControl(kMusicInfoMusicFormat));
	if (music_format != NULL )
	{
		strTmp.SmallFormat(_T("%s%s"),_T("文件格式:"),m_pMedia->exName.GetData());
		strTmp.Replace(_T("."),_T(""));
		music_format->SetText(strTmp);
	}

	CLabelUI* music_num = static_cast<CLabelUI*>(m_pm.FindControl(kMusicInfoMusicNum));
	if (music_num != NULL )
	{
		strTmp.SmallFormat(_T("%s%d"),_T("播放次数:"),m_pMedia->playNum);
		music_num->SetText(strTmp);
	}

	m_pt.y = pty;
	POINT pt = m_pt;
	::ClientToScreen(m_hParentWnd, &pt);
	::SetWindowPos(*this, NULL, pt.x, pt.y , 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
		
}

LRESULT CMusicInfoDialog::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	CDialogBuilder builder;

	CControlUI *pRoot = builder.Create(m_strXMLPath.GetData(), (UINT)0,  0, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);

	return 0;
}

LRESULT CMusicInfoDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CMusicInfoDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CMusicInfoDialog::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CMusicInfoDialog::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMusicInfoDialog::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMusicInfoDialog::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return HTCLIENT;
}


LRESULT CMusicInfoDialog::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
//	Close();
	return 0 ;
}

LRESULT CMusicInfoDialog::OnMouseLeave( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
//	Close();
	return 0 ;
}


LRESULT CMusicInfoDialog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
	case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break; 
	case WM_MOUSELEAVE:	   lRes = OnMouseLeave(uMsg, wParam, lParam, bHandled); break; 
	default:
		bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

