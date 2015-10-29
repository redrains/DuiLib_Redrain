#include "StdAfx.h"
#include "WkeWebkit.h"
#pragma comment(lib,"../wke/wke.lib")
namespace UiLib {

CWkeWebkitWnd::CWkeWebkitWnd() : m_pOwner(NULL), m_pWebView(NULL), m_bInit(false)
{
}

CWkeWebkitWnd::~CWkeWebkitWnd()
{
	if(m_pWebView) wkeDestroyWebView(m_pWebView);
}

wkeWebView	CWkeWebkitWnd::GetWebView()
{
	return m_pWebView;
}


void CWkeWebkitWnd::onBufUpdated( const HDC hdc,int x, int y, int cx, int cy )
{
	RECT rcClient;
	GetClientRect(m_hWnd,&rcClient);
	RECT rcInvalid = {rcClient.left + x, rcClient.top + y, rcClient.left + x + cx, rcClient.top + y + cy};
	InvalidateRect(m_hWnd, &rcInvalid ,TRUE);
}

void CWkeWebkitWnd::Init(CWkeWebkitUI* pOwner)
{
	m_pOwner = pOwner;
	m_bInit = true;
	if (m_hWnd == NULL)
	{
		RECT rcPos = m_pOwner->GetPos();
		UINT uStyle = UI_WNDSTYLE_CHILD;
//	HWND   hWnd = CreateWindow(_T("#32770"), _T("WndMediaDisplay"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, m_PaintManager.GetPaintWindow(), (HMENU)0, NULL, NULL);
		Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
	}
}


LPCTSTR CWkeWebkitWnd::GetWindowClassName() const
{
	return _T("WkeWebkit");
}

void CWkeWebkitWnd::OnFinalMessage(HWND /*hWnd*/)
{
	m_pOwner->Invalidate();
	m_pOwner->m_pWindow = NULL;
	delete this;
}

LRESULT CWkeWebkitWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_pWebView) wkeDestroyWebView(m_pWebView);
	KillTimer(m_hWnd, TM_TICKER);
	bHandled = TRUE;
	return 0;
}

LRESULT CWkeWebkitWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps = { 0 };
	HDC hDcPaint = ::BeginPaint(m_hWnd, &ps);

	RECT rcClip;	
	::GetClipBox(hDcPaint,&rcClip);	

	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	
	RECT rcInvalid;
	::IntersectRect(&rcInvalid, &rcClip,&rcClient);

	BitBlt(hDcPaint,rcInvalid.left,rcInvalid.top,
		rcInvalid.right - rcInvalid.left, rcInvalid.bottom - rcInvalid.top,
		m_pWebView->getViewDC(),rcInvalid.left-rcClient.left,rcInvalid.top-rcClient.top,SRCCOPY);            


	::EndPaint(m_hWnd, &ps);
	return 0;
}




LRESULT CWkeWebkitWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{


	m_pWebView = wkeCreateWebView();
	if(!m_pWebView) return -1;

	m_pWebView->setBufHandler(this);

// 	hander.onTitleChanged = onTitleChanged;   //Ä¬ÈÏ¹Ø±ÕÁËÍ¨Öª±êÌâ¸Ä±äµÄ»Øµ÷º¯Êý
// 	hander.onURLChanged = onURLChanged;
// 	m_pWebView->setClientHandler(&hander);

	SetTimer(m_hWnd, TM_TICKER,50 ,NULL);

	return 0;
}

LRESULT CWkeWebkitWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CDuiRect rcWnd;
	::GetWindowRect(*this, &rcWnd);

	m_pWebView->resize(rcWnd.GetWidth(), rcWnd.GetHeight());
	m_pWebView->tick();
	
	::InvalidateRect(m_hWnd, NULL, FALSE);
	return 0;
}

LRESULT CWkeWebkitWnd::OnMouseEvent(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_LBUTTONDOWN || uMsg == WM_MBUTTONDOWN || uMsg == WM_RBUTTONDOWN)
	{
		SetFocus(m_hWnd);
		SetCapture(m_hWnd);
	}
	else if (uMsg == WM_LBUTTONUP || uMsg == WM_MBUTTONUP || uMsg == WM_RBUTTONUP)
	{
		ReleaseCapture();
	}

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	int x = GET_X_LPARAM(lParam)-rcClient.left;
	int y = GET_Y_LPARAM(lParam)-rcClient.top;

	unsigned int flags = 0;

	if (wParam & MK_CONTROL)
		flags |= WKE_CONTROL;
	if (wParam & MK_SHIFT)
		flags |= WKE_SHIFT;

	if (wParam & MK_LBUTTON)
		flags |= WKE_LBUTTON;
	if (wParam & MK_MBUTTON)
		flags |= WKE_MBUTTON;
	if (wParam & MK_RBUTTON)
		flags |= WKE_RBUTTON;

	bHandled = m_pWebView->mouseEvent(uMsg, x, y, flags);
	return 0;
}

LRESULT CWkeWebkitWnd::OnMouseWheel(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	ScreenToClient(m_hWnd, &pt);

	int delta = GET_WHEEL_DELTA_WPARAM(wParam);

	unsigned int flags = 0;

	if (wParam & MK_CONTROL)
		flags |= WKE_CONTROL;
	if (wParam & MK_SHIFT)
		flags |= WKE_SHIFT;

	if (wParam & MK_LBUTTON)
		flags |= WKE_LBUTTON;
	if (wParam & MK_MBUTTON)
		flags |= WKE_MBUTTON;
	if (wParam & MK_RBUTTON)
		flags |= WKE_RBUTTON;

	bHandled = m_pWebView->mouseWheel(pt.x, pt.y, delta, flags);
	return 0;
}

LRESULT CWkeWebkitWnd::OnKeyDown(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled)
{
	unsigned int flags = 0;
	if (HIWORD(lParam) & KF_REPEAT)
		flags |= WKE_REPEAT;
	if (HIWORD(lParam) & KF_EXTENDED)
		flags |= WKE_EXTENDED;

	bHandled = m_pWebView->keyDown(wParam, flags, false);
	return 0;
}
LRESULT CWkeWebkitWnd::OnKeyUp(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled)
{
	unsigned int flags = 0;
	if (HIWORD(lParam) & KF_REPEAT)
		flags |= WKE_REPEAT;
	if (HIWORD(lParam) & KF_EXTENDED)
		flags |= WKE_EXTENDED;

	bHandled = m_pWebView->keyUp(wParam, flags, false);
	return 0;
}

LRESULT CWkeWebkitWnd::OnChar(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled)
{
	unsigned int charCode = wParam;
	unsigned int flags = 0;
	if (HIWORD(lParam) & KF_REPEAT)
		flags |= WKE_REPEAT;
	if (HIWORD(lParam) & KF_EXTENDED)
		flags |= WKE_EXTENDED;

	//flags = HIWORD(lParam);

	bHandled = m_pWebView->keyPress(charCode, flags, false);
	return 0;
}

LRESULT CWkeWebkitWnd::OnImeStartComposition(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled)
{
	wkeRect caret = m_pWebView->getCaret();

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	CANDIDATEFORM form;
	form.dwIndex = 0;
	form.dwStyle = CFS_EXCLUDE;
	form.ptCurrentPos.x = caret.x + rcClient.left;
	form.ptCurrentPos.y = caret.y + caret.h + rcClient.top;
	form.rcArea.top = caret.y + rcClient.top;
	form.rcArea.bottom = caret.y + caret.h + rcClient.top;
	form.rcArea.left = caret.x + rcClient.left;
	form.rcArea.right = caret.x + caret.w + rcClient.left;
	COMPOSITIONFORM compForm;
	compForm.ptCurrentPos=form.ptCurrentPos;
	compForm.rcArea=form.rcArea;
	compForm.dwStyle=CFS_POINT;

	HWND hWnd=m_pOwner->GetManager()->GetPaintWindow();
	HIMC hIMC = ImmGetContext(hWnd);
	ImmSetCandidateWindow(hIMC, &form);
	ImmSetCompositionWindow(hIMC,&compForm);
	ImmReleaseContext(hWnd, hIMC);

	//bHandled = TRUE;
	return 0;
}

LRESULT CWkeWebkitWnd::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_pWebView->focus();
//	bHandled = TRUE;
	return 0;
}

LRESULT CWkeWebkitWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_pWebView->unfocus();
	//bHandled = TRUE;
	return 0;
}

LRESULT CWkeWebkitWnd::OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);

	if (pt.x != -1 && pt.y != -1)
		ScreenToClient(m_hWnd, &pt);

	unsigned int flags = 0;

	if (wParam & MK_CONTROL)
		flags |= WKE_CONTROL;
	if (wParam & MK_SHIFT)
		flags |= WKE_SHIFT;

	if (wParam & MK_LBUTTON)
		flags |= WKE_LBUTTON;
	if (wParam & MK_MBUTTON)
		flags |= WKE_MBUTTON;
	if (wParam & MK_RBUTTON)
		flags |= WKE_RBUTTON;

	bHandled = m_pWebView->contextMenuEvent(pt.x, pt.y, flags);
	return 0;
}

LRESULT CWkeWebkitWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam == TM_TICKER)
	{
		m_pWebView->tick();
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CWkeWebkitWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch (uMsg)
	{
	case WM_PAINT:			lRes = OnPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEWHEEL:		lRes = OnMouseWheel(uMsg, wParam, lParam, bHandled); break;
	case WM_SIZE:			lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
	case WM_CHAR:			lRes = OnChar(uMsg, wParam, lParam, bHandled); break;
	case WM_KEYDOWN:		lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
	case WM_KEYUP:			lRes = OnKeyUp(uMsg, wParam, lParam, bHandled); break;
	case WM_KILLFOCUS:		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_SETFOCUS:		lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_TIMER:			lRes = OnTimer(uMsg, wParam, lParam, bHandled); break;
	case WM_IME_STARTCOMPOSITION: lRes = OnImeStartComposition(uMsg, wParam, lParam, bHandled); break;	
	case WM_CONTEXTMENU:	lRes= OnContextMenu(uMsg, wParam, lParam, bHandled); break;	
	case WM_LBUTTONDOWN:	
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEMOVE:
	case WM_MOUSELEAVE:
		lRes = OnMouseEvent(uMsg, wParam, lParam, bHandled); break;
	default:				bHandled = FALSE; break;
	}
	if (bHandled) return lRes;

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}
///////////////////////////////////////////////////  
//网页加载状态监测线程  
DWORD WINAPI CheckThreadFun(LPVOID lpParam)  
{  
    CWkeWebkitUI* pWebkitUI=(CWkeWebkitUI*)lpParam;  
    wkeWebView  pWebView=pWebkitUI->GetWebView();  
    if ( NULL == pWebView )  
        return 1;  
    CWkeWebkitLoadCallback* pCallback=pWebkitUI->GetLoadCallback();  
    if ( NULL == pCallback )  
        return 1;  
    bool bOver=false;  
    while( !pWebView->isLoaded() )  
    {  
        if ( !bOver && pWebView->isDocumentReady() )  
        {  
            pCallback->OnDocumentReady();  
            bOver=true;  
        }  
        if ( pWebView->isLoadFailed() )  
        {  
            pCallback->OnLoadFailed();  
            return 1;  
        }  
        ::Sleep(30);  
    }  
    if ( pWebView->isLoadComplete() )  
        pCallback->OnLoadComplete();  
    return 0;  
}  
///////////////////////////////////////////////////////////////////////////////
//////////////
//////////////

CWkeWebkitUI::CWkeWebkitUI(void):
m_pWindow(NULL),
m_pLoadCallback(NULL),
m_hCheckThread(0),
m_startUrl(_T("www.baidu.com"))
{

}


CWkeWebkitUI::~CWkeWebkitUI(void)
{	
	StopCheckThread(); 
	if (m_pWindow != NULL)
	{
		m_pWindow->Close();
	}
}

void CWkeWebkitUI::SetLoadCallback( CWkeWebkitLoadCallback* pCallback )  
{  
    m_pLoadCallback=pCallback;  
}  

CWkeWebkitLoadCallback* CWkeWebkitUI::GetLoadCallback()  
{  
    return m_pLoadCallback;  
}  

void CWkeWebkitUI::StartCheckThread()  
{  
    StopCheckThread();  
    m_hCheckThread=::CreateThread(NULL, 0, CheckThreadFun, this, 0, NULL);  
}  

void CWkeWebkitUI::StopCheckThread()  
{  
    if ( m_hCheckThread )  
    {  
        if ( ::WaitForSingleObject(m_hCheckThread, 10) != WAIT_OBJECT_0 )  
            ::TerminateThread(m_hCheckThread, 0);  
        ::CloseHandle(m_hCheckThread);  
        m_hCheckThread = NULL;  
    }  
}  

void CWkeWebkitUI::WkeWebkit_Init()
{
	wkeInit();
}

void CWkeWebkitUI::WkeWebkit_Shutdown()
{
	wkeShutdown();
}
wkeWebView CWkeWebkitUI::GetWebView()  
{  
	return m_pWindow->m_pWebView;  
}  
void CWkeWebkitUI::SetVisible(bool bVisible)
{
	CControlUI::SetVisible(bVisible);
	SetInternVisible(bVisible);
}

void CWkeWebkitUI::SetInternVisible(bool bVisible)
{
	CControlUI::SetInternVisible(bVisible);
	if (m_pWindow != NULL)
		::ShowWindow(m_pWindow->GetHWND(), bVisible?SW_SHOW:SW_HIDE); 
}

void CWkeWebkitUI::DoInit()
{
	m_pWindow = new CWkeWebkitWnd();
	if(m_pWindow)
	{
		m_pWindow->Init(this);
		m_pWindow->ShowWindow();
		SetURL(m_startUrl);
	}	
}

LPCTSTR CWkeWebkitUI::GetClass() const
{
	return _T("WkeWebkitUI");
}

LPVOID CWkeWebkitUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, DUI_CTR_WKEWEBKIT) == 0 ) return static_cast<CWkeWebkitUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

void CWkeWebkitUI::DoEvent(TEventUI& event)
{

	if( event.Type == UIEVENT_SETCURSOR )
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		return;
	}
		CControlUI::DoEvent(event);
}

void CWkeWebkitUI::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);	
	::SetWindowPos(m_pWindow->GetHWND(), NULL, rc.left, rc.top, rc.right - rc.left, 
		rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);   
}

void CWkeWebkitUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcscmp(pstrName, _T("starturl")) == 0)
	{
		m_startUrl = pstrValue;
		return;
	}
	CControlUI::SetAttribute(pstrName,pstrValue);
}

void CWkeWebkitUI::SetURL( wstring strValue)
{
	if (m_pWindow && m_pWindow->m_pWebView)
	{
		m_pWindow->m_strUrl = strValue;
		m_pWindow->m_pWebView->loadURL(strValue.c_str());
		StartCheckThread();
	}
}

void CWkeWebkitUI::SetFile( wstring strValue)
{
	if (m_pWindow && m_pWindow->m_pWebView)
	{
		m_pWindow->m_strUrl = strValue;
		m_pWindow->m_pWebView->loadFile(strValue.c_str());
		StartCheckThread();
	}
}

wstring CWkeWebkitUI::RunJS(wstring strValue)
{
	jsValue jsRet = m_pWindow->m_pWebView->runJS(strValue.c_str());
	return jsToStringW(m_pWindow->m_pWebView->globalExec(), jsRet);
}

void CWkeWebkitUI::SetClientHandler(const wkeClientHandler* handler)
{
	m_pWindow->m_pWebView->setClientHandler(handler);
}

void CWkeWebkitUI::GoBack()
{
	if (m_pWindow->m_pWebView->canGoBack())
		m_pWindow->m_pWebView->goBack();
}

void CWkeWebkitUI::GoForward()
{
	if (m_pWindow->m_pWebView->canGoForward())
		m_pWindow->m_pWebView->goForward();
}

}