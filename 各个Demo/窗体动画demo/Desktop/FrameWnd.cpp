#include "stdafx.h"


CFrameWnd::CFrameWnd( LPCTSTR pszXMLPath )
	: m_strXMLPath(pszXMLPath)
	, m_pAnimLayout(NULL)
{}

CFrameWnd::~CFrameWnd()
{}

LPCTSTR CFrameWnd::GetWindowClassName() const
{
	return _T("DesktopMainWnd");
}


CDuiString CFrameWnd::GetSkinFile()
{
	return m_strXMLPath;
}

CDuiString CFrameWnd::GetSkinFolder()
{
	return _T("Skin");
}

void CFrameWnd::InitWindow()
{
	m_pAnimLayout = static_cast<AnimLayout*>(m_PaintManager.FindControl(_T("vlayout_background")));
	ASSERT(m_pAnimLayout != NULL);

	CenterWindow();

}

void CFrameWnd::OnPrepare(TNotifyUI& msg)
{
	if (IDOK == MessageBoxW(m_hWnd, L"����ȷ����ʼ��ʾ79������Ч��", L"��ʾ", MB_OKCANCEL))
	{
		m_pAnimLayout->StartEffect();
	}
}

void CFrameWnd::Notify( TNotifyUI& msg )
{
	CDuiString strControlName = msg.pSender->GetName();

	if( msg.sType == _T("windowinit") ) 
	{	
		OnPrepare(msg);
	}
	else if( msg.sType == _T("click") ) 
	{	
	}

	__super::Notify(msg);
}

CControlUI* CFrameWnd::CreateControl( LPCTSTR pstrClassName )
{
	if (_tcsicmp(pstrClassName, AnimLayout::kAnimLayoutInterface) == 0)
	{
		return new AnimLayout;
	}
	return NULL;
}

LRESULT CFrameWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	if( !::IsZoomed(*this) )
	{
		RECT rcSizeBox = m_PaintManager.GetSizeBox();
		if( pt.y < rcClient.top + rcSizeBox.top )
		{
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
			return HTTOP;
		}
		else if( pt.y > rcClient.bottom - rcSizeBox.bottom )
		{
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}

		if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
		if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
	}

	RECT rcCaption = m_PaintManager.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(pt));
			if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
				_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
				_tcsicmp(pControl->GetClass(), _T("SliderUI")) != 0 &&
				_tcsicmp(pControl->GetClass(), _T("EditUI")) != 0 &&
				_tcsicmp(pControl->GetClass(), _T("RichEditUI")) != 0)
				return HTCAPTION;
	}

	return HTCLIENT;
}