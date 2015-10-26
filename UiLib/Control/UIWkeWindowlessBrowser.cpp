#include "StdAfx.h"
#include "UIWkeWindowlessBrowser.h"


namespace UiLib
{
CWkeWindowlessUI::CWkeWindowlessUI(void):m_webView(NULL)
{
}


CWkeWindowlessUI::~CWkeWindowlessUI(void)
{
}

LPCTSTR CWkeWindowlessUI::GetClass() const
{
	return _T("WkeWindowlessUI");
}
LPVOID CWkeWindowlessUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T(DUI_CTR_WKEWINDOWLESSBROWSER)) == 0 ) return static_cast<CWkeWindowlessUI*>(this);
	return CControlUI::GetInterface(pstrName);
}


void CWkeWindowlessUI::DoEvent(TEventUI& event)
{

	bool handled = true;
	if (!m_webView)
	{
		return CControlUI::DoEvent(event);
	}
	if (event.Type == UIEVENT_TIMER)
	{
		m_webView->tick();
	}

	else if (event.Type == UIEVENT_BUTTONDOWN)
	{
		m_webView->focus();
		int x = GET_X_LPARAM(event.lParam);
		int y = GET_Y_LPARAM(event.lParam);

		unsigned int flags = 0;

		if (event.wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (event.wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		if (event.wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (event.wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (event.wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;

		//flags = wParam;


		x -=m_rcPaint.left;
		y -=m_rcPaint.top;


		handled = m_webView->mouseEvent(WM_LBUTTONDOWN, x, y, flags);
	}else if (event.Type == UIEVENT_BUTTONUP)
	{
		int x = GET_X_LPARAM(event.lParam);
		int y = GET_Y_LPARAM(event.lParam);

		unsigned int flags = 0;

		if (event.wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (event.wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		if (event.wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (event.wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (event.wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;

		//flags = wParam;


		x -=m_rcPaint.left;
		y -=m_rcPaint.top;


		handled = m_webView->mouseEvent(WM_LBUTTONUP, x, y, flags);
	}else if (event.Type == UIEVENT_MOUSEMOVE)
	{
		int x = GET_X_LPARAM(event.lParam);
		int y = GET_Y_LPARAM(event.lParam);

		unsigned int flags = 0;

		if (event.wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (event.wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		if (event.wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (event.wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (event.wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;

		//flags = wParam;


		x -=m_rcPaint.left;
		y -=m_rcPaint.top;
		handled = m_webView->mouseEvent(WM_MOUSEMOVE, x, y, flags);
		
	}else if (event.Type == UIEVENT_RBUTTONDOWN)
	{
		m_webView->focus();
		int x = GET_X_LPARAM(event.lParam);
		int y = GET_Y_LPARAM(event.lParam);

		unsigned int flags = 0;

		if (event.wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (event.wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		if (event.wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (event.wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (event.wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;
		x -=m_rcPaint.left;
		y -=m_rcPaint.top;
		handled = m_webView->mouseEvent(WM_RBUTTONDOWN, x, y, flags);

	
	}
	else if (event.Type == UIEVENT_RBUTTONUP)
	{
		int x = GET_X_LPARAM(event.lParam);
		int y = GET_Y_LPARAM(event.lParam);

		unsigned int flags = 0;

		if (event.wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (event.wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		if (event.wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (event.wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (event.wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;
		x -=m_rcPaint.left;
		y -=m_rcPaint.top;
		handled = m_webView->mouseEvent(WM_RBUTTONUP, x, y, flags);
	}
	else if (event.Type == UIEVENT_SCROLLWHEEL)
	{
		POINT pt ;
		pt.x = event.ptMouse.x;
		pt.y = event.ptMouse.y;
		pt.x -= m_rcPaint.left;
		pt.y -= m_rcPaint.top;
		ScreenToClient(m_pManager->GetPaintWindow(), &pt);

		int delta = GET_WHEEL_DELTA_WPARAM(event.wParam);

		unsigned int flags = 0;

		if (event.wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (event.wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		if (event.wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (event.wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (event.wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;

		//flags = wParam;

		handled = m_webView->mouseWheel(pt.x,pt.y, delta, flags);
	}else if (event.Type == UIEVENT_KEYDOWN)
	{
		unsigned int virtualKeyCode = event.wParam;
		unsigned int flags = 0;
		if (HIWORD(event.lParam) & KF_REPEAT)
			flags |= WKE_REPEAT;
		if (HIWORD(event.lParam) & KF_EXTENDED)
			flags |= WKE_EXTENDED;

		//flags = HIWORD(lParam);

		handled = m_webView->keyDown(virtualKeyCode, flags, false);
	}else if (event.Type == UIEVENT_KEYUP)
	{
		unsigned int virtualKeyCode = event.wParam;
		unsigned int flags = 0;
		if (HIWORD(event.lParam) & KF_REPEAT)
			flags |= WKE_REPEAT;
		if (HIWORD(event.lParam) & KF_EXTENDED)
			flags |= WKE_EXTENDED;

		//flags = HIWORD(lParam);

		handled = m_webView->keyUp(virtualKeyCode, flags, false);
	}else if (event.Type == UIEVENT_CHAR)
	{
		unsigned int charCode = event.chKey;
		unsigned int flags = 0;
		if (HIWORD(event.lParam) & KF_REPEAT)
			flags |= WKE_REPEAT;
		if (HIWORD(event.lParam) & KF_EXTENDED)
			flags |= WKE_EXTENDED;

		//flags = HIWORD(lParam);

		handled = m_webView->keyPress(charCode, flags, false);
	}else if (event.Type == UIEVENT_IME_STARTCOMPOSITION)
	{
		wkeRect caret = m_webView->getCaret();

		CANDIDATEFORM form;
		form.dwIndex = 0;
		form.dwStyle = CFS_EXCLUDE;
		form.ptCurrentPos.x = caret.x ;
		form.ptCurrentPos.y = caret.y + caret.h;
		form.rcArea.top = caret.y + m_rcPaint.top;
		form.rcArea.bottom = caret.y + caret.h +m_rcPaint.top;
		form.rcArea.left = caret.x +m_rcPaint.left;
		form.rcArea.right = caret.x + caret.w +m_rcPaint.left;

		HIMC hIMC = ImmGetContext(m_pManager->GetPaintWindow());
		ImmSetCandidateWindow(hIMC, &form);
		ImmReleaseContext(m_pManager->GetPaintWindow(), hIMC);
	}
	else if (event.Type ==UIEVENT_SETFOCUS)
	{
		m_webView->focus();
	}
	else if (event.Type == UIEVENT_KILLFOCUS)
	{
		m_webView->unfocus();
	}

	if (!handled)
	{
		//CControlUI::DoEvent(event);
	}
	
}


void CWkeWindowlessUI::PaintBkImage(HDC hDC)
{
	CControlUI::PaintStatusImage(hDC);
	CDuiRect rect(m_rcItem);


	m_webView->paint(hDC,rect.left,rect.top,rect.GetWidth(),rect.GetHeight(),0,0,true);

}

void CWkeWindowlessUI::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);
	CDuiRect rect(m_rcItem);
	if (m_webView)
	{
		m_webView->resize(rect.GetWidth(),rect.GetHeight());
		m_webView->tick();
	}
	
}
void CWkeWindowlessUI::InitBrowser(UINT nTimerID)
{
	m_webView = wkeCreateWebView();
	m_webView->setTransparent(false);


	// 			wkeClientHandler handler;
	// 			handler.onTitleChanged = NULL;
	// 			handler.onURLChanged = NULL;
	// 			m_webView->setClientHandler(&handler);


	m_webView->setBufHandler((_wkeBufHandler*)this);

	CDuiRect rect(m_rcItem);
	m_webView->resize(rect.GetWidth(),rect.GetHeight());


	//	::SetTimer(m_hWnd,nTimerID,50,NULL);

	m_pManager->SetTimer(this,nTimerID,50);

}
void CWkeWindowlessUI::onBufUpdated (const HDC hdc,int x, int y, int cx, int cy)
{
	this->Invalidate();
}


void CWkeWindowlessUI::LoadUrl(LPCTSTR szUrl)
{
	m_webView->loadURL(szUrl);
}


}