#include "StdAfx.h"
#include <zmouse.h>

DECLARE_HANDLE(HZIP);	// An HZIP identifies a zip file that has been opened
typedef DWORD ZRESULT;
#define OpenZip OpenZipU
#define CloseZip(hz) CloseZipU(hz)
extern HZIP OpenZipU(void *z,unsigned int len,DWORD flags);
extern ZRESULT CloseZipU(HZIP hz);
#pragma init_seg(lib) // add by watertoeast, 2015-11-2

namespace UiLib {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//



	typedef struct tagFINDTABINFO
	{
		CControlUI* pFocus;
		CControlUI* pLast;
		bool bForward;
		bool bNextIsIt;
	} FINDTABINFO;

	typedef struct tagFINDSHORTCUT
	{
		TCHAR ch;
		bool bPickNext;
	} FINDSHORTCUT;

	typedef struct tagTIMERINFO
	{
		CControlUI* pSender;
		UINT nLocalID;
		HWND hWnd;
		UINT uWinTimer;
		bool bKilled;
	} TIMERINFO;

	/////////////////////////////////////////////////////////////////////////////////////
	CAnimationSpooler m_anim;
	HPEN m_hPens[UICOLOR__LAST] = { 0 };
	HFONT m_hFonts[UIFONT__LAST] = { 0 };
	HBRUSH m_hBrushes[UICOLOR__LAST] = { 0 };
	LOGFONT m_aLogFonts[UIFONT__LAST] = { 0 };
	COLORREF m_clrColors[UICOLOR__LAST][2] = { 0 };
	TEXTMETRIC m_aTextMetrics[UIFONT__LAST] = { 0 };
	HIMAGELIST m_himgIcons16 = NULL;
	HIMAGELIST m_himgIcons24 = NULL;
	HIMAGELIST m_himgIcons32 = NULL;
	HIMAGELIST m_himgIcons50 = NULL;
#define IDB_ICONS16 200
#define IDB_ICONS24 201
#define IDB_ICONS32 202
#define IDB_ICONS50 203


	HPEN m_hUpdateRectPen = NULL;
	HINSTANCE CPaintManagerUI::m_hInstance = NULL;
	HINSTANCE CPaintManagerUI::m_hResourceInstance = NULL;
	CDuiString CPaintManagerUI::m_pStrDefaultFontName;//added by cddjr at 05/18/2012
	CDuiString CPaintManagerUI::m_pStrResourcePath;
	CDuiString CPaintManagerUI::m_pStrResourceZip;
	bool CPaintManagerUI::m_bCachedResourceZip = false;
	HANDLE CPaintManagerUI::m_hResourceZip = NULL;
	short CPaintManagerUI::m_H = 180;
	short CPaintManagerUI::m_S = 100;
	short CPaintManagerUI::m_L = 100;
	CStdPtrArray CPaintManagerUI::m_aPreMessages;
	CStdPtrArray CPaintManagerUI::m_aPlugins;
	const UINT kCaretTimerID = 0xF1;

	CPaintManagerUI::CPaintManagerUI() :
	m_hWndPaint(NULL),
		m_hDcPaint(NULL),
		m_hDcOffscreen(NULL),
		m_hbmpOffscreen(NULL),
		m_pBmpOffscreenBits(NULL),
		m_bOffscreenPaint(true),
		m_bAlphaBackground(false),
		m_bIsRestore(false),
		m_hwndTooltip(NULL),
		m_uTimerID(0x1000),
		m_pRoot(NULL),
		m_pFocus(NULL),
		m_pEventHover(NULL),
		m_pEventClick(NULL),
		m_pEventKey(NULL),
		m_bFirstLayout(true),
		m_bFocusNeeded(false),
		m_bUpdateNeeded(false),
		m_bMouseTracking(false),
		m_bMouseCapture(false),
		m_bUsedVirtualWnd(false),
		m_nOpacity(255),
		m_pControlsStyle(NULL),
		m_pParentResourcePM(NULL),
		m_bCaretActive(false),
		m_bCaretShowing(false),
		m_currentCaretObject(NULL),
		m_bUseGdiplusText(false)
	{
		m_pControlsStyle				= &m_mControlsStyle;
		m_dwDefaultDisabledColor		= 0xFFA7A6AA;
		m_dwDefaultFontColor			= 0xFF000001;
		m_dwDefaultLinkFontColor		= 0xFF0000FF;
		m_dwDefaultLinkHoverFontColor	= 0xFFD3215F;
		m_dwDefaultSelectedBkColor		= 0xFFBAE4FF;
		m_sCurStylesName.Empty();

		LOGFONT lf = { 0 };
		::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		lf.lfCharSet = DEFAULT_CHARSET;
		if (CPaintManagerUI::m_pStrDefaultFontName.GetLength()>0)
		{
			_tcscpy_s(lf.lfFaceName, LF_FACESIZE, CPaintManagerUI::m_pStrDefaultFontName.GetData());
		}
		HFONT hDefaultFont = ::CreateFontIndirect(&lf);
		m_DefaultFontInfo.hFont = hDefaultFont;
		m_DefaultFontInfo.sFontName = lf.lfFaceName;
		m_DefaultFontInfo.iSize = -lf.lfHeight;
		m_DefaultFontInfo.bBold = (lf.lfWeight >= FW_BOLD);
		m_DefaultFontInfo.bUnderline = (lf.lfUnderline == TRUE);
		m_DefaultFontInfo.bItalic = (lf.lfItalic == TRUE);
		::ZeroMemory(&m_DefaultFontInfo.tm, sizeof(m_DefaultFontInfo.tm));

		if( m_hUpdateRectPen == NULL ) {
			m_hUpdateRectPen = ::CreatePen(PS_SOLID, 1, RGB(220, 0, 0));
			// Boot Windows Common Controls (for the ToolTip control)
			::InitCommonControls();
			::LoadLibrary(_T("msimg32.dll"));
		}

		m_pGdiplusStartupInput = new Gdiplus::GdiplusStartupInput;
		Gdiplus::GdiplusStartup( &m_gdiplusToken, m_pGdiplusStartupInput, NULL); // 加载GDI接口

		m_szMinWindow.cx = 0;
		m_szMinWindow.cy = 0;
		m_szMaxWindow.cx = 0;
		m_szMaxWindow.cy = 0;
		m_szInitWindowSize.cx = 0;
		m_szInitWindowSize.cy = 0;
		m_szRoundCorner.cx = m_szRoundCorner.cy = 0;
		::ZeroMemory(&m_rcSizeBox, sizeof(m_rcSizeBox));
		::ZeroMemory(&m_rcCaption, sizeof(m_rcCaption));
		::ZeroMemory(&m_rtCaret, sizeof(m_rtCaret));
		::ZeroMemory(&m_rcInvalidate, sizeof(m_rcInvalidate));
		m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
	}

	CPaintManagerUI::~CPaintManagerUI()
	{
		// Delete the control-tree structures
		for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
		for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) delete static_cast<TNotifyUI*>(m_aAsyncNotify[i]);
		m_mNameHash.Resize(0);
		delete m_pRoot;

		Gdiplus::GdiplusShutdown( m_gdiplusToken );	//  卸载GDI接口
		delete m_pGdiplusStartupInput;

		::DeleteObject(m_DefaultFontInfo.hFont);
		RemoveAllFonts();
		RemoveAllImages();
		RemoveAllDefaultAttributeList();
		RemoveAllEffectStyle();
		RemoveAllOptionGroups();
		RemoveAllTimers();
		RemoveAllStyles();

		// Reset other parts...
		if( m_hwndTooltip != NULL ) ::DestroyWindow(m_hwndTooltip);
		if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
		if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
		if( m_hDcPaint != NULL ) ::ReleaseDC(m_hWndPaint, m_hDcPaint);
		m_aPreMessages.Remove(m_aPreMessages.Find(this));
	}

	void CPaintManagerUI::Init(HWND hWnd)
	{
		ASSERT(::IsWindow(hWnd));
		// Remember the window context we came from
		m_hWndPaint = hWnd;
		m_hDcPaint = ::GetDC(hWnd);
		// We'll want to filter messages globally too
		m_aPreMessages.Add(this);
	}

	HINSTANCE CPaintManagerUI::GetInstance()
	{
		return m_hInstance;
	}

	CDuiString CPaintManagerUI::GetInstancePath()
	{
		if( m_hInstance == NULL ) return _T('\0');

		TCHAR tszModule[MAX_PATH + 1] = { 0 };
		::GetModuleFileName(m_hInstance, tszModule, MAX_PATH);
		CDuiString sInstancePath = tszModule;
		int pos = sInstancePath.ReverseFind(_T('\\'));
		if( pos >= 0 ) sInstancePath = sInstancePath.Left(pos + 1);
		return sInstancePath;
	}

	CDuiString CPaintManagerUI::GetCurrentPath()
	{
		TCHAR tszModule[MAX_PATH + 1] = { 0 };
		::GetCurrentDirectory(MAX_PATH, tszModule);
		return tszModule;
	}

	HINSTANCE CPaintManagerUI::GetResourceDll()
	{
		if( m_hResourceInstance == NULL ) return m_hInstance;
		return m_hResourceInstance;
	}

	const CDuiString& CPaintManagerUI::GetResourcePath()
	{
		return m_pStrResourcePath;
	}

	const CDuiString& CPaintManagerUI::GetResourceZip()
	{
		return m_pStrResourceZip;
	}

	bool CPaintManagerUI::IsCachedResourceZip()
	{
		return m_bCachedResourceZip;
	}

	HANDLE CPaintManagerUI::GetResourceZipHandle()
	{
		return m_hResourceZip;
	}

	void CPaintManagerUI::SetInstance(HINSTANCE hInst)
	{
		m_hInstance = hInst;
		CShadowUI::Initialize(hInst); 
	}

	void CPaintManagerUI::SetCurrentPath(LPCTSTR pStrPath)
	{
		::SetCurrentDirectory(pStrPath);
	}

	void CPaintManagerUI::SetResourceDll(HINSTANCE hInst)
	{
		m_hResourceInstance = hInst;
	}

	void CPaintManagerUI::SetResourcePath(LPCTSTR pStrPath)
	{
		m_pStrResourcePath = pStrPath;
		if( m_pStrResourcePath.IsEmpty() ) return;
		TCHAR cEnd = m_pStrResourcePath.GetAt(m_pStrResourcePath.GetLength() - 1);
		if( cEnd != _T('\\') && cEnd != _T('/') ) m_pStrResourcePath += _T('\\');
	}

	void CPaintManagerUI::SetResourceZip(LPVOID pVoid, unsigned int len)
	{
		if( m_pStrResourceZip == _T("membuffer") ) return;
		if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
			CloseZip((HZIP)m_hResourceZip);
			m_hResourceZip = NULL;
		}
		m_pStrResourceZip = _T("membuffer");
		m_bCachedResourceZip = true;
		if( m_bCachedResourceZip ) 
			m_hResourceZip = (HANDLE)OpenZip(pVoid, len, 3);
	}

	void CPaintManagerUI::SetResourceZip(LPCTSTR pStrPath, bool bCachedResourceZip)
	{
		if( m_pStrResourceZip == pStrPath && m_bCachedResourceZip == bCachedResourceZip ) return;
		if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
			CloseZip((HZIP)m_hResourceZip);
			m_hResourceZip = NULL;
		}
		m_pStrResourceZip = pStrPath;
		m_bCachedResourceZip = bCachedResourceZip;
		if( m_bCachedResourceZip ) {
			CDuiString sFile = CPaintManagerUI::GetResourcePath();
			sFile += CPaintManagerUI::GetResourceZip();
			m_hResourceZip = (HANDLE)OpenZip((void*)sFile.GetData(), 0, 2);
		}
	}

	void CPaintManagerUI::GetHSL(short* H, short* S, short* L)
	{
		*H = m_H;
		*S = m_S;
		*L = m_L;
	}

	void CPaintManagerUI::SetHSL(bool bUseHSL, short H, short S, short L)
	{
		if( H == m_H && S == m_S && L == m_L ) return;
		m_H = CLAMP(H, 0, 360);
		m_S = CLAMP(S, 0, 200);
		m_L = CLAMP(L, 0, 200);
		for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
			CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
			if( pManager != NULL && pManager->GetRoot() != NULL )
				pManager->GetRoot()->Invalidate();
		}
	}

	void CPaintManagerUI::ReloadSkin()
	{
		for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
			CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
			pManager->ReloadAllImages();
		}
	}

	bool CPaintManagerUI::LoadPlugin(LPCTSTR pstrModuleName)
	{
		ASSERT( !::IsBadStringPtr(pstrModuleName,-1) || pstrModuleName == NULL );
		if( pstrModuleName == NULL ) return false;
		HMODULE hModule = ::LoadLibrary(pstrModuleName);
		if( hModule != NULL ) {
			LPCREATECONTROL lpCreateControl = (LPCREATECONTROL)::GetProcAddress(hModule, "CreateControl");
			if( lpCreateControl != NULL ) {
				if( m_aPlugins.Find(lpCreateControl) >= 0 ) return true;
				m_aPlugins.Add(lpCreateControl);
				return true;
			}
		}
		return false;
	}

	CStdPtrArray* CPaintManagerUI::GetPlugins()
	{
		return &m_aPlugins;
	}

	HWND CPaintManagerUI::GetPaintWindow() const
	{
		return m_hWndPaint;
	}

	HWND CPaintManagerUI::GetTooltipWindow() const
	{
		return m_hwndTooltip;
	}

	HDC CPaintManagerUI::GetPaintDC() const
	{
		return m_hDcPaint;
	}

	POINT CPaintManagerUI::GetMousePos() const
	{
		return m_ptLastMousePos;
	}

	SIZE CPaintManagerUI::GetClientSize() const
	{
		RECT rcClient = { 0 };
		::GetClientRect(m_hWndPaint, &rcClient);
		return CSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
	}

	SIZE CPaintManagerUI::GetInitSize()
	{
		return m_szInitWindowSize;
	}

	void CPaintManagerUI::SetInitSize(int cx, int cy)
	{
		m_szInitWindowSize.cx = cx;
		m_szInitWindowSize.cy = cy;
		if( m_pRoot == NULL && m_hWndPaint != NULL ) {
			::SetWindowPos(m_hWndPaint, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		}
	}

	RECT& CPaintManagerUI::GetSizeBox()
	{
		return m_rcSizeBox;
	}

	void CPaintManagerUI::SetSizeBox(RECT& rcSizeBox)
	{
		m_rcSizeBox = rcSizeBox;
	}

	RECT& CPaintManagerUI::GetCaptionRect()
	{
		return m_rcCaption;
	}

	void CPaintManagerUI::SetCaptionRect(RECT& rcCaption)
	{
		m_rcCaption = rcCaption;
	}

	SIZE CPaintManagerUI::GetRoundCorner() const
	{
		return m_szRoundCorner;
	}

	void CPaintManagerUI::SetRoundCorner(int cx, int cy)
	{
		m_szRoundCorner.cx = cx;
		m_szRoundCorner.cy = cy;
	}

	SIZE CPaintManagerUI::GetMinInfo() const
	{
		return m_szMinWindow;
	}

	void CPaintManagerUI::SetMinInfo(int cx, int cy)
	{
		ASSERT(cx>=0 && cy>=0);
		m_szMinWindow.cx = cx;
		m_szMinWindow.cy = cy;
	}

	SIZE CPaintManagerUI::GetMaxInfo() const
	{
		return m_szMaxWindow;
	}

	void CPaintManagerUI::SetMaxInfo(int cx, int cy)
	{
		ASSERT(cx>=0 && cy>=0);
		m_szMaxWindow.cx = cx;
		m_szMaxWindow.cy = cy;
	}

	int CPaintManagerUI::GetTransparent() const
	{
		return m_nOpacity;
	}
	void CPaintManagerUI::SetTransparent(int nOpacity)
	{
		if (nOpacity<0)
			m_nOpacity = 0;
		else if (nOpacity>255)
			m_nOpacity = 255;
		else
			m_nOpacity = nOpacity;
		if( m_hWndPaint != NULL ) {
			typedef BOOL (__stdcall *PFUNCSETLAYEREDWINDOWATTR)(HWND, COLORREF, BYTE, DWORD);
			PFUNCSETLAYEREDWINDOWATTR fSetLayeredWindowAttributes;

			HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
			if (hUser32)
			{
				fSetLayeredWindowAttributes = 
					(PFUNCSETLAYEREDWINDOWATTR)::GetProcAddress(hUser32, "SetLayeredWindowAttributes");
				if( fSetLayeredWindowAttributes == NULL ) return;
			}

			DWORD dwStyle = ::GetWindowLong(m_hWndPaint, GWL_EXSTYLE);
			DWORD dwNewStyle = dwStyle;
			if( nOpacity >= 0 && nOpacity < 256 ) dwNewStyle |= WS_EX_LAYERED;
			else dwNewStyle &= ~WS_EX_LAYERED;
			if(dwStyle != dwNewStyle) ::SetWindowLong(m_hWndPaint, GWL_EXSTYLE, dwNewStyle);
			fSetLayeredWindowAttributes(m_hWndPaint, 0, nOpacity, LWA_ALPHA);
		}
	}

	void CPaintManagerUI::SetBackgroundTransparent(bool bTrans)
	{
		m_bAlphaBackground = bTrans;
	}

	bool CPaintManagerUI::IsBackgroundTransparent()
	{
		return m_bAlphaBackground;
	}

	bool CPaintManagerUI::ShowCaret(bool bShow)
	{
		if(m_bCaretShowing == bShow)
			return true;

		m_bCaretShowing = bShow;
		if(!bShow)
		{	
			::KillTimer(m_hWndPaint, kCaretTimerID);
			if(m_bCaretActive)
			{
				Invalidate(m_rtCaret);
			}
			m_bCaretActive = false;
		}
		else
		{
			::SetTimer(m_hWndPaint, kCaretTimerID, ::GetCaretBlinkTime(), NULL);
			if(!m_bCaretActive)
			{
				Invalidate(m_rtCaret);
				m_bCaretActive = true;
			}
		}

		return true;
	}

	bool CPaintManagerUI::SetCaretPos(CRichEditUI* obj, int x, int y)
	{
		if(!::SetCaretPos(x, y)) 
			return false;

		m_currentCaretObject = obj;
		RECT tempRt = m_rtCaret;
		int w = m_rtCaret.right - m_rtCaret.left;
		int h = m_rtCaret.bottom - m_rtCaret.top;
		m_rtCaret.left = x;
		m_rtCaret.top = y;
		m_rtCaret.right = x + w;
		m_rtCaret.bottom = y + h;
		Invalidate(tempRt);
		Invalidate(m_rtCaret);

		return true;
	}

	CRichEditUI* CPaintManagerUI::GetCurrentCaretObject()
	{
		return m_currentCaretObject;
	}

	bool CPaintManagerUI::CreateCaret(HBITMAP hBmp, int nWidth, int nHeight)
	{
		::CreateCaret(m_hWndPaint, hBmp, nWidth, nHeight);
		//TODO hBmp处理位图光标
		m_rtCaret.right = m_rtCaret.left + nWidth;
		m_rtCaret.bottom = m_rtCaret.top + nHeight;
		return true;
	}

	void CPaintManagerUI::DrawCaret(HDC hDC, const RECT& rcPaint)
	{
		if(m_currentCaretObject && (!m_currentCaretObject->IsFocused() || m_hWndPaint != ::GetFocus()))
		{
			::KillTimer(m_hWndPaint, kCaretTimerID);
			if(m_bCaretActive)
			{
				Invalidate(m_rtCaret);
			}
			m_bCaretActive = false;
			return;
		}

		if(m_bCaretActive && m_bCaretShowing && m_currentCaretObject)
		{
			RECT temp = {};
			if(::IntersectRect(&temp, &rcPaint, &m_rtCaret))
			{
				DWORD dwColor = m_currentCaretObject->GetTextColor();
				if(dwColor == 0)
					dwColor = m_dwDefaultFontColor;
				CRenderEngine::DrawColor(hDC, temp, dwColor);
			}
		}
	}

	CShadowUI* CPaintManagerUI::GetShadow()
	{
		return &m_shadow;
	}

	void CPaintManagerUI::SetUseGdiplusText(bool bUse)
	{
		m_bUseGdiplusText = bUse;
	}

	bool CPaintManagerUI::IsUseGdiplusText() const
	{
		return m_bUseGdiplusText;
	}

	bool CPaintManagerUI::PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& /*lRes*/)
	{
		for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) 
		{
			bool bHandled = false;
			LRESULT lResult = static_cast<IMessageFilterUI*>(m_aPreMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
			if( bHandled ) {
				return true;
			}
		}
		switch( uMsg ) {
		case WM_KEYDOWN:
			{
				// Tabbing between controls
				if( wParam == VK_TAB ) {
					if( m_pFocus && m_pFocus->IsVisible() && m_pFocus->IsEnabled() && _tcsstr(m_pFocus->GetClass(), _T("RichEditUI")) != NULL ) {
						if( static_cast<CRichEditUI*>(m_pFocus)->IsWantTab() ) return false;
					}
					SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
					return true;
				}
			}
			break;
		case WM_SYSCHAR:
			{
				// Handle ALT-shortcut key-combinations
				FINDSHORTCUT fs = { 0 };
				fs.ch = toupper((int)wParam);
				CControlUI* pControl = m_pRoot->FindControl(__FindControlFromShortcut, &fs, UIFIND_ENABLED | UIFIND_ME_FIRST | UIFIND_TOP_FIRST);
				if( pControl != NULL ) {
					pControl->SetFocus();
					pControl->Activate();
					return true;
				}
			}
			break;
		case WM_SYSKEYDOWN:
			{
				if( m_pFocus != NULL ) {
					TEventUI event = { 0 };
					event.Type = UIEVENT_SYSKEY;
					event.chKey = (TCHAR)wParam;
					event.ptMouse = m_ptLastMousePos;
					event.wKeyState = MapKeyState();
					event.dwTimestamp = ::GetTickCount();
					m_pFocus->Event(event);
				}
			}
			break;
		}
		return false;
	}

	bool CPaintManagerUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		//#ifdef _DEBUG
		//    switch( uMsg ) {
		//    case WM_NCPAINT:
		//    case WM_NCHITTEST:
		//    case WM_SETCURSOR:
		//       break;
		//    default:
		//       DUITRACE(_T("MSG: %-20s (%08ld)"), TRACEMSG(uMsg), ::GetTickCount());
		//    }
		//#endif
		// Not ready yet?
		if( m_hWndPaint == NULL ) return false;

		TNotifyUI* pMsg = NULL;
		while( pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify.GetAt(0)) ) {
			m_aAsyncNotify.Remove(0);
			if( pMsg->pSender != NULL ) {
				if( pMsg->pSender->OnNotify ){
					if(pMsg->pSender->OnNotify(pMsg)){
						delete pMsg;
						return true;
					}
				}
			}
			for( int j = 0; j < m_aNotifiers.GetSize(); j++ ) {
				static_cast<INotifyUI*>(m_aNotifiers[j])->Notify(*pMsg);
			}
			delete pMsg;
		}

		// Cycle through listeners
		for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) 
		{
			bool bHandled = false;
			LRESULT lResult = static_cast<IMessageFilterUI*>(m_aMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
			if( bHandled ) {
				lRes = lResult;
				return true;
			}
		}
		// Custom handling of events
		switch( uMsg ) {
		case WM_APP + 1:
			{
				for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) 
					delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
				m_aDelayedCleanup.Empty();
			}
			break;
		case WM_CLOSE:
			{
				// Make sure all matching "closing" events are sent
				TEventUI event = { 0 };
				event.ptMouse = m_ptLastMousePos;
				event.dwTimestamp = ::GetTickCount();
				if( m_pEventHover != NULL ) {
					event.Type = UIEVENT_MOUSELEAVE;
					event.pSender = m_pEventHover;
					m_pEventHover->Event(event);
				}
				if( m_pEventClick != NULL ) {
					event.Type = UIEVENT_BUTTONUP;
					event.pSender = m_pEventClick;
					m_pEventClick->Event(event);
				}

				SetFocus(NULL);

				// Hmmph, the usual Windows tricks to avoid
				// focus loss...
				HWND hwndParent = GetWindowOwner(m_hWndPaint);
				if( hwndParent != NULL ) ::SetFocus(hwndParent);
			}
			break;
		case WM_ERASEBKGND:
			{
				// We'll do the painting here...
				lRes = 1;
				return true;
			}
		case WM_EFFECTS:
			{
				//
				// Render screen
				//
				if( m_anim.IsAnimating() )
				{
					// 3D animation in progress
					//   3D动画  
					m_anim.Render();
					// Do a minimum paint loop  做一个最小的绘制循环
					// Keep the client area invalid so we generate lots of
					// WM_PAINT messages. Cross fingers that Windows doesn't
					// batch these somehow in the future.
					PAINTSTRUCT ps = { 0 };
					::BeginPaint(m_hWndPaint, &ps);
					::EndPaint(m_hWndPaint, &ps);
					::InvalidateRect(m_hWndPaint, NULL, FALSE);
				}
				else if( m_anim.IsJobScheduled() ) {
					// Animation system needs to be initialized
					//	动画系统需要初始化
					m_anim.Init(m_hWndPaint);
					// A 3D animation was scheduled; allow the render engine to
					// capture the window content and repaint some other time
					//翻译(by 金山词霸)一个3d动画被准备;允许渲染引擎捕获窗口内容，并且适时重画

					if( !m_anim.PrepareAnimation(m_hWndPaint)) m_anim.CancelJobs();
					::InvalidateRect(m_hWndPaint, NULL, TRUE);
				}
			}
			return true;
	   case WM_PAINT:
		   {
			   // Should we paint?
			   RECT rcPaint = {0};
			   if(!::GetUpdateRect(m_hWndPaint, &rcPaint, FALSE)) return true;
			   if(m_pRoot == NULL)
			   {
				   PAINTSTRUCT ps = {0};
				   ::BeginPaint(m_hWndPaint, &ps);
				   ::EndPaint(m_hWndPaint, &ps);
				   return true;
			   }

			   // Begin Windows paint
			   PAINTSTRUCT ps = {0};
			   ::BeginPaint(m_hWndPaint, &ps);


			   // Do we need to resize anything?
			   // This is the time where we layout the controls on the form.
			   // We delay this even from the WM_SIZE messages since resizing can be
			   // a very expensize operation.
			   if(m_bUpdateNeeded)
			   {
				   m_bUpdateNeeded = false;
				   RECT rcClient = {0};
				   ::GetClientRect(m_hWndPaint, &rcClient);
				   if(!::IsRectEmpty(&rcClient))
				   {
					   if(m_pRoot->IsUpdateNeeded())
					   {
						   if( !::IsIconic(m_hWndPaint))  //redrain修复bug
							   m_pRoot->SetPos(rcClient);
						   if(m_hDcOffscreen != NULL) ::DeleteDC(m_hDcOffscreen);
						   if(m_hbmpOffscreen != NULL) ::DeleteObject(m_hbmpOffscreen);
						   m_hDcOffscreen = NULL;
						   m_hbmpOffscreen = NULL;
						   m_pBmpOffscreenBits = NULL;
					   }
					   else
					   {
						   CControlUI* pControl = NULL;
						   while(pControl = m_pRoot->FindControl(__FindControlFromUpdate, NULL, UIFIND_VISIBLE | UIFIND_ME_FIRST))
						   {
							   pControl->SetPos(pControl->GetPos());
						   }
					   }
					   // We'll want to notify the window when it is first initialized
					   // with the correct layout. The window form would take the time
					   // to submit swipes/animations.
					   if(m_bFirstLayout)
					   {
						   m_bFirstLayout = false;
						   SendNotify(m_pRoot, _T("windowinit"), 0, 0, false);
					   }
				   }
			   }
			   // Set focus to first control?
			   if(m_bFocusNeeded)
			   {
				   SetNextTabControl();
			   }
			   if( m_anim.IsAnimating() || m_anim.IsJobScheduled())
				   ::PostMessage(m_hWndPaint,WM_EFFECTS,NULL,NULL);

			   // 是否开启了半透明窗体模式
			   if(m_bAlphaBackground)
			   {
				   // 设置层样式
				   DWORD dwExStyle = GetWindowLong(m_hWndPaint, GWL_EXSTYLE);
				   if((dwExStyle&WS_EX_LAYERED) != WS_EX_LAYERED)
					   SetWindowLong(m_hWndPaint, GWL_EXSTYLE, dwExStyle|WS_EX_LAYERED);

				   RECT rcClient = {0};
				   GetClientRect(m_hWndPaint, &rcClient);
				   // 如果窗体从最小化恢复，则刷新整个软件
				   if (!m_bIsRestore)
				   {
					   UnionRect(&rcPaint, &rcPaint, &m_rcInvalidate);
					   ::ZeroMemory(&m_rcInvalidate, sizeof(m_rcInvalidate));				   
				   }
				   else
				   {
					   rcPaint = rcClient;
					   m_bIsRestore = false;
				   }

				   int nClientWidth = rcClient.right - rcClient.left;
				   int nClientHeight = rcClient.bottom - rcClient.top;
				   if(m_bOffscreenPaint && m_hbmpOffscreen == NULL)
				   {
					   m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
					   BITMAPINFO bmi;
					   ::ZeroMemory(&bmi, sizeof(BITMAPINFO));
					   bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					   bmi.bmiHeader.biWidth = nClientWidth;
					   bmi.bmiHeader.biHeight = -nClientHeight;
					   bmi.bmiHeader.biPlanes = 1;
					   bmi.bmiHeader.biBitCount = 32;
					   bmi.bmiHeader.biCompression = BI_RGB;
					   bmi.bmiHeader.biSizeImage = nClientWidth * nClientHeight * 4;
					   bmi.bmiHeader.biClrUsed = 0;
					   m_hbmpOffscreen = ::CreateDIBSection(m_hDcPaint, &bmi, DIB_RGB_COLORS,
						   (void**)&m_pBmpOffscreenBits, NULL, 0);

					   ASSERT(m_hDcOffscreen);
					   ASSERT(m_hbmpOffscreen);
				   }

				   HBITMAP hOldBitmap = (HBITMAP)::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
				   int iSaveDC = ::SaveDC(m_hDcOffscreen);
				   CRenderEngine::ClearAlphaPixel(m_pBmpOffscreenBits, nClientWidth, &rcPaint);
				   m_pRoot->DoPaint(m_hDcOffscreen, rcPaint);
				   DrawCaret(m_hDcOffscreen, rcPaint);
				   for(int i = 0; i < m_aPostPaintControls.GetSize(); i++)
				   {
					   CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
					   pPostPaintControl->DoPostPaint(m_hDcOffscreen, ps.rcPaint);
				   }
				   CRenderEngine::RestoreAlphaColor(m_pBmpOffscreenBits, nClientWidth, &rcPaint);
				   ::RestoreDC(m_hDcOffscreen, iSaveDC);

				   // 贴到主窗体
				   RECT rcWnd = {0};
				   ::GetWindowRect(m_hWndPaint, &rcWnd);
				   POINT pt = {rcWnd.left, rcWnd.top};
				   SIZE szWindow = {rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top};
				   POINT ptSrc = {0, 0};
				   BLENDFUNCTION blendPixelFunction = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
				   ::UpdateLayeredWindow(m_hWndPaint, NULL, &pt, &szWindow, m_hDcOffscreen, 
					   &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);

				   ::SelectObject(m_hDcOffscreen, hOldBitmap);
			   }
			   else
			   {
				   if(m_bOffscreenPaint && m_hbmpOffscreen == NULL)
				   {
					   RECT rcClient = {0};
					   ::GetClientRect(m_hWndPaint, &rcClient);
					   m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
					   m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
					   ASSERT(m_hDcOffscreen);
					   ASSERT(m_hbmpOffscreen);
				   }

				   if(m_bOffscreenPaint)
				   {
					   HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
					   int iSaveDC = ::SaveDC(m_hDcOffscreen);
					   m_pRoot->DoPaint(m_hDcOffscreen, ps.rcPaint);
					   DrawCaret(m_hDcOffscreen, ps.rcPaint);
					   for(int i = 0; i < m_aPostPaintControls.GetSize(); i++)
					   {
						   CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
						   pPostPaintControl->DoPostPaint(m_hDcOffscreen, ps.rcPaint);
					   }
					   ::RestoreDC(m_hDcOffscreen, iSaveDC);
					   ::BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
						   ps.rcPaint.bottom - ps.rcPaint.top, m_hDcOffscreen, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
					   ::SelectObject(m_hDcOffscreen, hOldBitmap);

				   }
				   else
				   {
					   // A standard paint job
					   int iSaveDC = ::SaveDC(ps.hdc);
					   m_pRoot->DoPaint(ps.hdc, ps.rcPaint);
					   DrawCaret(ps.hdc, ps.rcPaint);
					   ::RestoreDC(ps.hdc, iSaveDC);
				   }

			   }

			   // All Done!
			   ::EndPaint(m_hWndPaint, &ps);

		   }
		   // If any of the painting requested a resize again, we'll need
		   // to invalidate the entire window once more.
		   if(m_bUpdateNeeded)
		   {
			   ::InvalidateRect(m_hWndPaint, NULL, FALSE);
		   }
		   return true;
		case WM_SYSCOMMAND:
			{
				if (SC_RESTORE == (wParam & 0xfff0))
				{
					m_bIsRestore = true;
				}
				return true;
			}
			break;
		case WM_PRINTCLIENT:
			{
				RECT rcClient;
				::GetClientRect(m_hWndPaint, &rcClient);
				HDC hDC = (HDC) wParam;
				int save = ::SaveDC(hDC);
				m_pRoot->DoPaint(hDC, rcClient);
				// Check for traversing children. The crux is that WM_PRINT will assume
				// that the DC is positioned at frame coordinates and will paint the child
				// control at the wrong position. We'll simulate the entire thing instead.
				if( (lParam & PRF_CHILDREN) != 0 ) {
					HWND hWndChild = ::GetWindow(m_hWndPaint, GW_CHILD);
					while( hWndChild != NULL ) {
						RECT rcPos = { 0 };
						::GetWindowRect(hWndChild, &rcPos);
						::MapWindowPoints(HWND_DESKTOP, m_hWndPaint, reinterpret_cast<LPPOINT>(&rcPos), 2);
						::SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, NULL);
						// NOTE: We use WM_PRINT here rather than the expected WM_PRINTCLIENT
						//       since the latter will not print the nonclient correctly for
						//       EDIT controls.
						::SendMessage(hWndChild, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
						hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
					}
				}
				::RestoreDC(hDC, save);
			}
			break;
		case WM_GETMINMAXINFO:
			{
				LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
				if( m_szMinWindow.cx > 0 ) lpMMI->ptMinTrackSize.x = m_szMinWindow.cx;
				if( m_szMinWindow.cy > 0 ) lpMMI->ptMinTrackSize.y = m_szMinWindow.cy;
				if( m_szMaxWindow.cx > 0 ) lpMMI->ptMaxTrackSize.x = m_szMaxWindow.cx;
				if( m_szMaxWindow.cy > 0 ) lpMMI->ptMaxTrackSize.y = m_szMaxWindow.cy;
			}
			break;
		case WM_SIZE:
			{
				if( m_pFocus != NULL ) {
					TEventUI event = { 0 };
					event.Type = UIEVENT_WINDOWSIZE;
					event.pSender = m_pFocus;
					event.dwTimestamp = ::GetTickCount();
					m_pFocus->Event(event);
				}
				//当对话框尺寸变化时 删除动画 job
				if( m_anim.IsAnimating() ) m_anim.CancelJobs();
				m_bUpdateNeeded= true;  //csk_no
				if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
			}
			return true;
		case WM_TIMER:
			{
				if(kCaretTimerID == LOWORD(wParam)){
					//DUI__Trace(_T("WM_TIMER:%d (%d,%d)"), m_bCaretActive, m_rtCaret.left, m_rtCaret.top);
					Invalidate(m_rtCaret);
					m_bCaretActive = !m_bCaretActive;
				}
				else{
					for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
						const TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
						if(pTimer->hWnd == m_hWndPaint && 
							pTimer->uWinTimer == LOWORD(wParam) && 
							pTimer->bKilled == false)
						{
							TEventUI event = { 0 };
							event.Type = UIEVENT_TIMER;
							event.pSender = pTimer->pSender;
							event.wParam = pTimer->nLocalID;
							event.dwTimestamp = ::GetTickCount();
							pTimer->pSender->Event(event);
							break;
						}
					}
				}

			}
			break;
		case WM_MOUSEHOVER:
			{
				m_bMouseTracking = false;
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				CControlUI* pHover = FindControl(pt);
				if( pHover == NULL ) break;
				// Generate mouse hover event
				if( m_pEventHover != NULL ) {
					TEventUI event = { 0 };
					event.ptMouse = pt;
					event.Type = UIEVENT_MOUSEHOVER;
					event.pSender = m_pEventHover;
					event.dwTimestamp = ::GetTickCount();
					m_pEventHover->Event(event);
				}
				// Create tooltip information
				CDuiString sToolTip = pHover->GetToolTip();
				if( sToolTip.IsEmpty() ) return true;
				::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
				m_ToolTip.cbSize = sizeof(TOOLINFO);
				m_ToolTip.uFlags = TTF_IDISHWND;
				m_ToolTip.hwnd = m_hWndPaint;
				m_ToolTip.uId = (UINT_PTR) m_hWndPaint;
				m_ToolTip.hinst = m_hInstance;
				m_ToolTip.lpszText = const_cast<LPTSTR>( (LPCTSTR) sToolTip );
				m_ToolTip.rect = pHover->GetPos();
				if( m_hwndTooltip == NULL ) {
					m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, m_hInstance, NULL);
					::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_ToolTip);
				}
				::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_ToolTip);
				::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_ToolTip);
			}
			return true;
		case WM_MOUSELEAVE:
			{
				if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
				if( m_bMouseTracking ) ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) -1);
				m_bMouseTracking = false;
			}
			break;
		case WM_MOUSEMOVE:
			{
				// Start tracking this entire window again...
				if( !m_bMouseTracking ) {
					TRACKMOUSEEVENT tme = { 0 };
					tme.cbSize = sizeof(TRACKMOUSEEVENT);
					tme.dwFlags = TME_HOVER | TME_LEAVE;
					tme.hwndTrack = m_hWndPaint;
					tme.dwHoverTime = m_hwndTooltip == NULL ? 400UL : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
					_TrackMouseEvent(&tme);
					m_bMouseTracking = true;
				}
				// Generate the appropriate mouse messages
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				CControlUI* pNewHover = FindControl(pt);
				if( pNewHover != NULL && pNewHover->GetManager() != this ) break;
				TEventUI event = { 0 };
				event.ptMouse = pt;
				event.dwTimestamp = ::GetTickCount();
				if( pNewHover != m_pEventHover && m_pEventHover != NULL ) {
					event.Type = UIEVENT_MOUSELEAVE;
					event.pSender = m_pEventHover;
					m_pEventHover->Event(event);
					m_pEventHover = NULL;
					if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
				}
				if( pNewHover != m_pEventHover && pNewHover != NULL ) {
					event.Type = UIEVENT_MOUSEENTER;
					event.pSender = pNewHover;
					pNewHover->Event(event);
					m_pEventHover = pNewHover;
				}
				if( m_pEventClick != NULL ) {
					event.Type = UIEVENT_MOUSEMOVE;
					event.pSender = m_pEventClick;
					m_pEventClick->Event(event);
				}
				else if( pNewHover != NULL ) {
					event.Type = UIEVENT_MOUSEMOVE;
					event.pSender = pNewHover;
					pNewHover->Event(event);
				}
			}
			break;
		case WM_LBUTTONDOWN:
			{
				// We alway set focus back to our app (this helps
				// when Win32 child windows are placed on the dialog
				// and we need to remove them on focus change).
				::SetFocus(m_hWndPaint);
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				CControlUI* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				m_pEventClick = pControl;
				pControl->SetFocus();
				SetCapture();
				TEventUI event = { 0 };
				event.Type = UIEVENT_BUTTONDOWN;
				event.pSender = pControl;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);

				// No need to burden user with 3D animations
				m_anim.CancelJobs();
			}
			break;
		case WM_LBUTTONDBLCLK:
			{
				::SetFocus(m_hWndPaint);
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				CControlUI* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				SetCapture();
				TEventUI event = { 0 };
				event.Type = UIEVENT_DBLCLICK;
				event.pSender = pControl;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);
				m_pEventClick = pControl;
			}
			break;
		case WM_LBUTTONUP:
			{
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				if( m_pEventClick == NULL ) break;
				ReleaseCapture();
				TEventUI event = { 0 };
				event.Type = UIEVENT_BUTTONUP;
				event.pSender = m_pEventClick;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();
				m_pEventClick->Event(event);
				m_pEventClick = NULL;
			}
			break;
		case WM_RBUTTONDOWN:
			{
				::SetFocus(m_hWndPaint);
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				CControlUI* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				pControl->SetFocus();
				SetCapture();
				TEventUI event = { 0 };
				event.Type = UIEVENT_RBUTTONDOWN;
				event.pSender = pControl;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);
				m_pEventClick = pControl;
			}
			break;
		case WM_CONTEXTMENU:
			{
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				::ScreenToClient(m_hWndPaint, &pt);
				m_ptLastMousePos = pt;
				if( m_pEventClick == NULL ) break;
				ReleaseCapture();
				TEventUI event = { 0 };
				event.Type = UIEVENT_CONTEXTMENU;
				event.pSender = m_pEventClick;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.lParam = (LPARAM)m_pEventClick;
				event.dwTimestamp = ::GetTickCount();
				m_pEventClick->Event(event);
				m_pEventClick = NULL;
			}
			break;
		case WM_RELOADSTYLE:
			{
				TEventUI event		= { 0 };
				event.Type			= UIEVENT_RELOADSTYLE;
				event.pSender		= m_pRoot;
				event.wParam		= wParam;
				event.lParam		= lParam;
				event.ptMouse.x		= 0;
				event.ptMouse.y		= 0;
				event.wKeyState		= 0;
				event.dwTimestamp	= ::GetTickCount();
				EventAllControl(event,m_pRoot);
			}
			break;
		case WM_MOUSEWHEEL:
			{
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				::ScreenToClient(m_hWndPaint, &pt);
				m_ptLastMousePos = pt;
				CControlUI* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				int zDelta = (int) (short) HIWORD(wParam);
				TEventUI event = { 0 };
				event.Type = UIEVENT_SCROLLWHEEL;
				event.pSender = pControl;
				event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
				event.lParam = lParam;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);

				// Let's make sure that the scroll item below the cursor is the same as before...
				::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
			}
			break;
		case WM_CHAR:
			{
				if( m_pFocus == NULL ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_CHAR;
				event.chKey = (TCHAR)wParam;
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->Event(event);
			}
			break;
		case WM_KEYDOWN:
			{
				if( m_pFocus == NULL ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_KEYDOWN;
				event.chKey = (TCHAR)wParam;
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->Event(event);
				m_pEventKey = m_pFocus;
			}
			break;
		case WM_KEYUP:
			{
				if( m_pEventKey == NULL ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_KEYUP;
				event.chKey = (TCHAR)wParam;
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				m_pEventKey->Event(event);
				m_pEventKey = NULL;
			}
			break;
		case WM_SETCURSOR:
			{
				if( LOWORD(lParam) != HTCLIENT ) break;
				if( m_bMouseCapture ) return true;

				POINT pt = { 0 };
				::GetCursorPos(&pt);
				::ScreenToClient(m_hWndPaint, &pt);
				CControlUI* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( (pControl->GetControlFlags() & UIFLAG_SETCURSOR) == 0 ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_SETCURSOR;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);
			}
			return true;
		case WM_NOTIFY:
			{
				LPNMHDR lpNMHDR = (LPNMHDR) lParam;
				if( lpNMHDR != NULL ) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
				return true;
			}
			break;
		case WM_COMMAND:
			{
				if( lParam == 0 ) break;
				HWND hWndChild = (HWND) lParam;
				lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
				return true;
			}
			break;
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORSTATIC:
			{
				// Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
				// Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
				if( lParam == 0 ) break;
				HWND hWndChild = (HWND) lParam;
				lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
				return true;
			}
			break;
		default:
			break;
		}

		pMsg = NULL;
		while( pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify.GetAt(0)) ) {
			m_aAsyncNotify.Remove(0);
			if( pMsg->pSender != NULL ) {
				if( pMsg->pSender->OnNotify ){
					if(pMsg->pSender->OnNotify(pMsg)){
						delete pMsg;
						return true;
					}
				}
			}
			for( int j = 0; j < m_aNotifiers.GetSize(); j++ ) {
				static_cast<INotifyUI*>(m_aNotifiers[j])->Notify(*pMsg);
			}
			delete pMsg;
		}

		return false;
	}

	void CPaintManagerUI::NeedUpdate()
	{
		m_bUpdateNeeded = true;
	}

	void CPaintManagerUI::Invalidate(RECT& rcItem)
	{
		::InvalidateRect(m_hWndPaint, &rcItem, FALSE);
		m_rcInvalidate = rcItem;
	}

	bool CPaintManagerUI::AttachDialog(CControlUI* pControl)
	{
		ASSERT(::IsWindow(m_hWndPaint));
		// Reset any previous attachment
		SetFocus(NULL);
		m_pEventKey = NULL;
		m_pEventHover = NULL;
		m_pEventClick = NULL;
		// Remove the existing control-tree. We might have gotten inside this function as
		// a result of an event fired or similar, so we cannot just delete the objects and
		// pull the internal memory of the calling code. We'll delay the cleanup.
		if( m_pRoot != NULL ) {
			m_aPostPaintControls.Empty();
			AddDelayedCleanup(m_pRoot);
		}
		// Set the dialog root element
		m_pRoot = pControl;
		// Go ahead...
		m_bUpdateNeeded = true;
		m_bFirstLayout = true;
		m_bFocusNeeded = true;

		m_shadow.Create(this);

		// Initiate all control
		return InitControls(pControl);
	}

	bool CPaintManagerUI::InitControls(CControlUI* pControl, CControlUI* pParent /*= NULL*/)
	{
		ASSERT(pControl);
		if( pControl == NULL ) return false;
		pControl->SetManager(this, pParent != NULL ? pParent : pControl->GetParent(), true);
		pControl->FindControl(__FindControlFromNameHash, this, UIFIND_ALL);
		return true;
	}

	void CPaintManagerUI::ReapObjects(CControlUI* pControl)
	{
		if( pControl == m_pEventKey ) m_pEventKey = NULL;
		if( pControl == m_pEventHover ) m_pEventHover = NULL;
		if( pControl == m_pEventClick ) m_pEventClick = NULL;
		if( pControl == m_pFocus ) m_pFocus = NULL;
		KillTimer(pControl);
		const CDuiString& sName = pControl->GetName();
		if( !sName.IsEmpty() ) {
			if( pControl == FindControl(sName) ) m_mNameHash.Remove(sName);
		}
		for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) {
			TNotifyUI* pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify[i]);
			if( pMsg->pSender == pControl ) pMsg->pSender = NULL;
		}    
	}

	bool CPaintManagerUI::AddOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl)
	{
		LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
		if( lp ) {
			CStdPtrArray* aOptionGroup = static_cast<CStdPtrArray*>(lp);
			for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
				if( static_cast<CControlUI*>(aOptionGroup->GetAt(i)) == pControl ) {
					return false;
				}
			}
			aOptionGroup->Add(pControl);
		}
		else {
			CStdPtrArray* aOptionGroup = new CStdPtrArray(6);
			aOptionGroup->Add(pControl);
			m_mOptionGroup.Insert(pStrGroupName, aOptionGroup);
		}
		return true;
	}

	CStdPtrArray* CPaintManagerUI::GetOptionGroup(LPCTSTR pStrGroupName)
	{
		LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
		if( lp ) return static_cast<CStdPtrArray*>(lp);
		return NULL;
	}

	void CPaintManagerUI::RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl)
	{
		LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
		if( lp ) {
			CStdPtrArray* aOptionGroup = static_cast<CStdPtrArray*>(lp);
			if( aOptionGroup == NULL ) return;
			for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
				if( static_cast<CControlUI*>(aOptionGroup->GetAt(i)) == pControl ) {
					aOptionGroup->Remove(i);
					break;
				}
			}
			if( aOptionGroup->IsEmpty() ) {
				delete aOptionGroup;
				m_mOptionGroup.Remove(pStrGroupName);
			}
		}
	}

	void CPaintManagerUI::RemoveAllOptionGroups()
	{
		CStdPtrArray* aOptionGroup;
		for( int i = 0; i< m_mOptionGroup.GetSize(); i++ ) {
			if(LPCTSTR key = m_mOptionGroup.GetAt(i)) {
				aOptionGroup = static_cast<CStdPtrArray*>(m_mOptionGroup.Find(key));
				delete aOptionGroup;
			}
		}
		m_mOptionGroup.RemoveAll();
	}

	bool CPaintManagerUI::AddEffectsStyle( LPCTSTR pStrStyleName,LPCTSTR pStrStyleValue )
	{
		CDuiString* pEffectsStyleValue = new CDuiString(pStrStyleValue);

		if(pEffectsStyleValue != NULL)
		{
			if( m_mEffectsStyle.Find(pStrStyleName) == NULL )
				m_mEffectsStyle.Set(pStrStyleName,pEffectsStyleValue);
			else
				delete pEffectsStyleValue;
			pEffectsStyleValue = NULL;
			return true;
		}
		return false;
	}

	UiLib::CDuiString CPaintManagerUI::GetEffectsStyle( LPCTSTR pStrStyleName )
	{
		CDuiString* pEffectsStyleValue = static_cast<CDuiString*>(m_mEffectsStyle.Find(pStrStyleName));
		if( !pEffectsStyleValue && m_pParentResourcePM ) return m_pParentResourcePM->GetEffectsStyle(pStrStyleName);

		if( pEffectsStyleValue ) return pEffectsStyleValue->GetData();
		else return CDuiString(_T(""));
	}

	const CStdStringPtrMap& CPaintManagerUI::GetEffectsStyles() const
	{
		return m_mEffectsStyle;
	}

	bool CPaintManagerUI::RemoveEffectStyle( LPCTSTR pStrStyleName )
	{
		CDuiString* pEffectsStyleValue = static_cast<CDuiString*>(m_mEffectsStyle.Find(pStrStyleName));
		if( !pEffectsStyleValue ) return false;

		delete pEffectsStyleValue;
		return m_mEffectsStyle.Remove(pStrStyleName);
	}

	void CPaintManagerUI::RemoveAllEffectStyle()
	{
		CDuiString* pEffectsStyleValue;
		for( int i = 0; i< m_mEffectsStyle.GetSize(); i++ ) {
			if(LPCTSTR key = m_mEffectsStyle.GetAt(i)) {
				pEffectsStyleValue = static_cast<CDuiString*>(m_mEffectsStyle.Find(key));
				if(pEffectsStyleValue)
					delete pEffectsStyleValue;
				pEffectsStyleValue = NULL;
			}
		}
		m_mEffectsStyle.RemoveAll();
	}

	bool CPaintManagerUI::AddControlStyle( LPCTSTR pStrStyleName,LPCTSTR pStrKey,LPCTSTR pStrVal,LPCTSTR pStylesName /*= NULL*/ )
	{
		CStdStringPtrMap* pCurPtrMap = m_pControlsStyle;
		if(pStylesName)
			pCurPtrMap = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pCurPtrMap){
			pCurPtrMap = new CStdStringPtrMap();
			m_mStyles.Set(pStylesName,(LPVOID)pCurPtrMap);
		}

		CStdStringPtrMap* pPtrMap = static_cast<CStdStringPtrMap*>(GetControlStyles(pStrStyleName,pStylesName));

		if(!pPtrMap){
			pPtrMap = new CStdStringPtrMap();
			pCurPtrMap->Set(pStrStyleName,(LPVOID)pPtrMap);
		}

		if(pPtrMap)
		{
			CDuiString* nVal = new CDuiString(pStrVal);

			if( pPtrMap->Find(pStrKey) == NULL )
				pPtrMap->Set(pStrKey,nVal);
			else
				delete nVal;
			nVal = NULL;
			return true;
		}
		return false;
	}

	bool CPaintManagerUI::AddControlStyle( LPCTSTR pStrStyleName,CStdStringPtrMap* _StyleMap,LPCTSTR pStylesName /*= NULL*/ )
	{
		CStdStringPtrMap* pCurPtrMap = m_pControlsStyle;
		if(pStylesName)
			pCurPtrMap = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pCurPtrMap){
			pCurPtrMap = new CStdStringPtrMap();
			m_mStyles.Set(pStylesName,(LPVOID)pCurPtrMap);
		}

		LPVOID pStyle = pCurPtrMap->Find(pStrStyleName);
		if(pStyle)
			return false;
		return NULL != pCurPtrMap->Set(pStrStyleName,(LPVOID)_StyleMap);
	}

	bool CPaintManagerUI::SetControlStyle( LPCTSTR pStrStyleName,LPCTSTR pStrKey,LPCTSTR pStrVal,LPCTSTR pStylesName /*= NULL*/ )
	{
		CStdStringPtrMap* pCurPtrMap = m_pControlsStyle;
		if(pStylesName)
			pCurPtrMap = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pCurPtrMap){
			pCurPtrMap = new CStdStringPtrMap();
			m_mStyles.Set(pStylesName,(LPVOID)pCurPtrMap);
		}

		CStdStringPtrMap* pPtrMap = static_cast<CStdStringPtrMap*>(GetControlStyles(pStrStyleName,pStylesName));

		if(!pPtrMap){
			pPtrMap = new CStdStringPtrMap();
			pCurPtrMap->Set(pStrStyleName,(LPVOID)pPtrMap);
		}

		if(pPtrMap)
		{
			CDuiString* nVal = new CDuiString(pStrVal);

			if( pPtrMap->Find(pStrKey))
				delete pPtrMap->Find(pStrKey);

			pPtrMap->Set(pStrKey,nVal);
			return true;
		}
		return false;
	}

	bool CPaintManagerUI::SetControlStyle( LPCTSTR pStrStyleName,CStdStringPtrMap* _StyleMap,LPCTSTR pStylesName /*= NULL*/ )
	{
		CStdStringPtrMap* pCurPtrMap = m_pControlsStyle;
		if(pStylesName)
			pCurPtrMap = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pCurPtrMap){
			pCurPtrMap = new CStdStringPtrMap();
			m_mStyles.Set(pStylesName,(LPVOID)pCurPtrMap);
		}

		LPVOID pStyle = pCurPtrMap->Find(pStrStyleName);
		if(pStyle)
			RemoveControlStyle(pStrStyleName,NULL,pStylesName);

		pCurPtrMap->Set(pStrStyleName,(LPVOID)_StyleMap);
		return true;
	}

	UiLib::CDuiString CPaintManagerUI::GetControlStyle( LPCTSTR pStrStyleName,LPCTSTR pStrKey,LPCTSTR pStylesName /*= NULL*/ )
	{
		if(pStrStyleName == NULL || pStrKey == NULL)
			return _T("");

		CStdStringPtrMap* pPtrMap = static_cast<CStdStringPtrMap*>(GetControlStyles(pStrStyleName,pStylesName));
		if(NULL == pPtrMap)
			return _T("");

		CDuiString* nStrVal = static_cast<CDuiString*>(pPtrMap->Find(pStrKey));
		return NULL == nStrVal?_T(""):nStrVal->GetData();
	}

	CStdStringPtrMap* CPaintManagerUI::GetControlsStyles( LPCTSTR pStylesName /*= NULL*/ ) const
	{
		CStdStringPtrMap* pCurPtrMap = m_pControlsStyle;
		if(pStylesName)
			pCurPtrMap = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pCurPtrMap)
			return NULL;

		return pCurPtrMap;
	}

	CStdStringPtrMap* CPaintManagerUI::GetControlStyles( LPCTSTR pStrStyleName,LPCTSTR pStylesName /*= NULL*/ ) const
	{
		CStdStringPtrMap* pCurPtrMap = m_pControlsStyle;
		if(pStylesName)
			pCurPtrMap = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pCurPtrMap)
			return NULL;

		if(pCurPtrMap->GetSize() == 0 || pStrStyleName == NULL)
			return NULL;

		CStdStringPtrMap* pControlStyle = static_cast<CStdStringPtrMap*>(pCurPtrMap->Find(pStrStyleName));
		return pControlStyle?pControlStyle:NULL;
	}

	bool CPaintManagerUI::RemoveControlStyle( LPCTSTR pStrStyleName,LPCTSTR pStrKey /*= NULL*/,LPCTSTR pStylesName /*= NULL*/ )
	{
		if(pStrStyleName == NULL)
			return false;

		CStdStringPtrMap* pCurPtrMap = m_pControlsStyle;
		if(pStylesName)
			pCurPtrMap = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pCurPtrMap)
			return false;

		CStdStringPtrMap* pPtrMap = GetControlStyles(pStrStyleName,pStylesName);

		if(pStrKey == NULL && pCurPtrMap->Remove(pStrStyleName)){
			delete pPtrMap;
			pPtrMap = NULL;
			return true;
		}

		if(pPtrMap){
			CDuiString* pVal = static_cast<CDuiString*>(pPtrMap->Find(pStrKey));
			if(pVal && pPtrMap->Remove(pStrKey)){
				delete pVal;
				pVal = NULL;
				return true;
			}
		}
		return false;
	}

	void CPaintManagerUI::RemoveAllControlStyle( LPCTSTR pStrStyleName /*= NULL*/,LPCTSTR pStylesName /*= NULL*/ )
	{
		CStdStringPtrMap* pCurPtrMap = m_pControlsStyle;
		if(pStylesName)
			pCurPtrMap = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pCurPtrMap)
			return;

		if(pStrStyleName){
			CStdStringPtrMap* pPtrMap = GetControlStyles(pStrStyleName,pStylesName);
			if(!pPtrMap)
				return;

			CDuiString* pVal = NULL;
			for( int i = 0; i< pPtrMap->GetSize(); i++ ) {
				if(LPCTSTR key = pPtrMap->GetAt(i)) {
					pVal = static_cast<CDuiString*>(pPtrMap->Find(key));
					if(pVal)
						delete pVal;
					pVal = NULL;
				}
			}
			pPtrMap->RemoveAll();
			delete pPtrMap;
			pPtrMap = NULL;
			pCurPtrMap->Remove(pStrStyleName);
			return;
		}

		for( int i = 0; i< pCurPtrMap->GetSize(); i++ ) {
			if(LPCTSTR key = pCurPtrMap->GetAt(i)) {
				RemoveAllControlStyle(key,pStylesName);
			}
		}
		pCurPtrMap->RemoveAll();
	}

	bool CPaintManagerUI::SetCurStyles( LPCTSTR pStylesName /*= NULL*/,bool _NowUpdate /*= true*/ )
	{
		if(!pStylesName){
			m_pControlsStyle	= &m_mControlsStyle;
			m_sCurStylesName.Empty();
			if(_NowUpdate)
				::PostMessage(m_hWndPaint,WM_RELOADSTYLE,NULL,NULL);
			return true;
		}

		if(pStylesName){
			CStdStringPtrMap* pStyleMap = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));
			if(pStyleMap){
				m_pControlsStyle	= pStyleMap;
				m_sCurStylesName	= pStylesName;
				if(_NowUpdate)
					::PostMessage(m_hWndPaint,WM_RELOADSTYLE,NULL,NULL);
				return true;
			}
		}

		return false;
	}

	bool CPaintManagerUI::SetCurStyles( int _iStyleIndex /*= 0*/,bool _NowUpdate /*= true*/ )
	{
		if(_iStyleIndex < 0)
			return false;

		return SetCurStyles(m_mStyles.GetAt(_iStyleIndex),_NowUpdate);
	}

	UINT CPaintManagerUI::GetStylesCount()
	{
		return (UINT)m_mStyles.GetSize();
	}

	UiLib::CDuiString CPaintManagerUI::GetCurStylesName()
	{
		return m_sCurStylesName.GetData();
	}

	bool CPaintManagerUI::RemoveStyles( LPCTSTR pStylesName )
	{
		if(m_sCurStylesName == pStylesName)
			return false;

		for(int nIndex = 0;nIndex < m_mStyles.GetSize();nIndex++)
		{
			CDuiString nKey = m_mStyles.GetAt(nIndex);
			if(nKey == pStylesName)
				continue;

			RemoveAllControlStyle(NULL,nKey.GetData());
			return m_mStyles.Remove(nKey);
		}
		return false;
	}

	void CPaintManagerUI::RemoveAllStyles()
	{
		SetCurStyles(NULL);
		for(int nIndex = 0;nIndex < m_mStyles.GetSize();nIndex++)
		{
			CDuiString nKey = m_mStyles.GetAt(nIndex);
			RemoveStyles(nKey.GetData());
		}
		m_mStyles.RemoveAll();
	}

	void CPaintManagerUI::MessageLoop()
	{
		MSG msg = { 0 };
		while( ::GetMessage(&msg, NULL, 0, 0) ) {
			if( !CPaintManagerUI::TranslateMessage(&msg) ) {
				::TranslateMessage(&msg);
				try{
					::DispatchMessage(&msg);
				} catch(...) {
					DUITRACE(_T("EXCEPTION: %s(%d)\n"), __FILET__, __LINE__);
#ifdef _DEBUG
					throw "CPaintManagerUI::MessageLoop";
#endif
				}
			}
		}
	}

	void CPaintManagerUI::Term()
	{
		if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
			CloseZip((HZIP)m_hResourceZip);
			m_hResourceZip = NULL;
		}
	}

	CControlUI* CPaintManagerUI::GetFocus() const
	{
		return m_pFocus;
	}

	void CPaintManagerUI::SetFocus(CControlUI* pControl)
	{
		// Paint manager window has focus?
		HWND hFocusWnd = ::GetFocus();
		if( hFocusWnd != m_hWndPaint && pControl != m_pFocus ) ::SetFocus(m_hWndPaint);
		// Already has focus?
		if( pControl == m_pFocus ) return;
		// Remove focus from old control
		if( m_pFocus != NULL ) 
		{
			TEventUI event = { 0 };
			event.Type = UIEVENT_KILLFOCUS;
			event.pSender = pControl;
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
			SendNotify(m_pFocus, DUI_MSGTYPE_KILLFOCUS);
			m_pFocus = NULL;
		}
		if( pControl == NULL ) return;
		// Set focus to new control
		if( pControl != NULL 
			&& pControl->GetManager() == this 
			&& pControl->IsVisible() 
			&& pControl->IsEnabled() ) 
		{
			m_pFocus = pControl;
			TEventUI event = { 0 };
			event.Type = UIEVENT_SETFOCUS;
			event.pSender = pControl;
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
			SendNotify(m_pFocus, DUI_MSGTYPE_SETFOCUS);
		}
	}

	void CPaintManagerUI::SetFocusNeeded(CControlUI* pControl)
	{
		::SetFocus(m_hWndPaint);
		if( pControl == NULL ) return;
		if( m_pFocus != NULL ) {
			TEventUI event = { 0 };
			event.Type = UIEVENT_KILLFOCUS;
			event.pSender = pControl;
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
			SendNotify(m_pFocus, DUI_MSGTYPE_KILLFOCUS);
			m_pFocus = NULL;
		}
		FINDTABINFO info = { 0 };
		info.pFocus = pControl;
		info.bForward = false;
		m_pFocus = m_pRoot->FindControl(__FindControlFromTab, &info, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
		m_bFocusNeeded = true;
		if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
	}

	bool CPaintManagerUI::SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse)
	{
		ASSERT(pControl!=NULL);
		ASSERT(uElapse>0);
		for( int i = 0; i< m_aTimers.GetSize(); i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
			if( pTimer->pSender == pControl
				&& pTimer->hWnd == m_hWndPaint
				&& pTimer->nLocalID == nTimerID ) {
					if( pTimer->bKilled == true ) {
						if( ::SetTimer(m_hWndPaint, pTimer->uWinTimer, uElapse, NULL) ) {
							pTimer->bKilled = false;
							return true;
						}
						return false;
					}
					return false;
			}
		}

		m_uTimerID = (++m_uTimerID) % 0xF0; //0xf1-0xfe特殊用途
		if( !::SetTimer(m_hWndPaint, m_uTimerID, uElapse, NULL) ) return FALSE;
		TIMERINFO* pTimer = new TIMERINFO;
		if( pTimer == NULL ) return FALSE;
		pTimer->hWnd = m_hWndPaint;
		pTimer->pSender = pControl;
		pTimer->nLocalID = nTimerID;
		pTimer->uWinTimer = m_uTimerID;
		pTimer->bKilled = false;
		return m_aTimers.Add(pTimer);
	}

	bool CPaintManagerUI::KillTimer(CControlUI* pControl, UINT nTimerID)
	{
		ASSERT(pControl!=NULL);
		for( int i = 0; i< m_aTimers.GetSize(); i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
			if( pTimer->pSender == pControl
				&& pTimer->hWnd == m_hWndPaint
				&& pTimer->nLocalID == nTimerID )
			{
				if( pTimer->bKilled == false ) {
					if( ::IsWindow(m_hWndPaint) ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
					pTimer->bKilled = true;
					return true;
				}
			}
		}
		return false;
	}

	void CPaintManagerUI::KillTimer(CControlUI* pControl)
	{
		ASSERT(pControl!=NULL);
		int count = m_aTimers.GetSize();
		for( int i = 0, j = 0; i < count; i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i - j]);
			if( pTimer->pSender == pControl && pTimer->hWnd == m_hWndPaint ) {
				if( pTimer->bKilled == false ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
				delete pTimer;
				m_aTimers.Remove(i - j);
				j++;
			}
		}
	}

	void CPaintManagerUI::RemoveAllTimers()
	{
		for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
			if( pTimer->hWnd == m_hWndPaint ) {
				if( pTimer->bKilled == false ) {
					if( ::IsWindow(m_hWndPaint) ) ::KillTimer(m_hWndPaint, pTimer->uWinTimer);
				}
				delete pTimer;
			}
		}

		m_aTimers.Empty();
	}

	void CPaintManagerUI::SetCapture()
	{
		::SetCapture(m_hWndPaint);
		m_bMouseCapture = true;
	}

	void CPaintManagerUI::ReleaseCapture()
	{
		::ReleaseCapture();
		m_bMouseCapture = false;
	}

	bool CPaintManagerUI::IsCaptured()
	{
		return m_bMouseCapture;
	}

	bool CPaintManagerUI::SetNextTabControl(bool bForward)
	{
		// If we're in the process of restructuring the layout we can delay the
		// focus calulation until the next repaint.
		if( m_bUpdateNeeded && bForward ) {
			m_bFocusNeeded = true;
			::InvalidateRect(m_hWndPaint, NULL, FALSE);
			return true;
		}
		// Find next/previous tabbable control
		FINDTABINFO info1 = { 0 };
		info1.pFocus = m_pFocus;
		info1.bForward = bForward;
		CControlUI* pControl = m_pRoot->FindControl(__FindControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
		if( pControl == NULL ) {  
			if( bForward ) {
				// Wrap around
				FINDTABINFO info2 = { 0 };
				info2.pFocus = bForward ? NULL : info1.pLast;
				info2.bForward = bForward;
				pControl = m_pRoot->FindControl(__FindControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
			}
			else {
				pControl = info1.pLast;
			}
		}
		if( pControl != NULL ) SetFocus(pControl);
		m_bFocusNeeded = false;
		return true;
	}

	void CPaintManagerUI::EventAllControl( TEventUI& event,CControlUI* pControl /*= NULL*/ )
	{
		IContainerUI* pContainer = static_cast<IContainerUI*>(pControl->GetInterface(_T("IContainer")));
		if(pContainer){
			int nCountNode = pContainer->GetCount();
			for(int nIndex = 0;nIndex < nCountNode;nIndex++)
			{
				CControlUI* nControl = pContainer->GetItemAt(nIndex);
				if(nControl)
					nControl->Event(event);

				EventAllControl(event,nControl);
			}
		}
	}

	bool CPaintManagerUI::AddNotifier(INotifyUI* pNotifier)
	{
		ASSERT(m_aNotifiers.Find(pNotifier)<0);
		return m_aNotifiers.Add(pNotifier);
	}

	bool CPaintManagerUI::RemoveNotifier(INotifyUI* pNotifier)
	{
		for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
			if( static_cast<INotifyUI*>(m_aNotifiers[i]) == pNotifier ) {
				return m_aNotifiers.Remove(i);
			}
		}
		return false;
	}

	bool CPaintManagerUI::AddPreMessageFilter(IMessageFilterUI* pFilter)
	{
		ASSERT(m_aPreMessageFilters.Find(pFilter)<0);
		return m_aPreMessageFilters.Add(pFilter);
	}

	bool CPaintManagerUI::RemovePreMessageFilter(IMessageFilterUI* pFilter)
	{
		for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) {
			if( static_cast<IMessageFilterUI*>(m_aPreMessageFilters[i]) == pFilter ) {
				return m_aPreMessageFilters.Remove(i);
			}
		}
		return false;
	}

	bool CPaintManagerUI::AddMessageFilter(IMessageFilterUI* pFilter)
	{
		ASSERT(m_aMessageFilters.Find(pFilter)<0);
		return m_aMessageFilters.Add(pFilter);
	}

	bool CPaintManagerUI::RemoveMessageFilter(IMessageFilterUI* pFilter)
	{
		for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) {
			if( static_cast<IMessageFilterUI*>(m_aMessageFilters[i]) == pFilter ) {
				return m_aMessageFilters.Remove(i);
			}
		}
		return false;
	}

	int CPaintManagerUI::GetPostPaintCount() const
	{
		return m_aPostPaintControls.GetSize();
	}

	bool CPaintManagerUI::AddPostPaint(CControlUI* pControl)
	{
		ASSERT(m_aPostPaintControls.Find(pControl) < 0);
		return m_aPostPaintControls.Add(pControl);
	}

	bool CPaintManagerUI::RemovePostPaint(CControlUI* pControl)
	{
		for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
			if( static_cast<CControlUI*>(m_aPostPaintControls[i]) == pControl ) {
				return m_aPostPaintControls.Remove(i);
			}
		}
		return false;
	}

	bool CPaintManagerUI::SetPostPaintIndex(CControlUI* pControl, int iIndex)
	{
		RemovePostPaint(pControl);
		return m_aPostPaintControls.InsertAt(iIndex, pControl);
	}

	void CPaintManagerUI::AddDelayedCleanup(CControlUI* pControl)
	{
		pControl->SetManager(this, NULL, false);
		m_aDelayedCleanup.Add(pControl);
		::PostMessage(m_hWndPaint, WM_APP + 1, 0, 0L);
	}

	void CPaintManagerUI::SendNotify(CControlUI* pControl, LPCTSTR pstrMessage, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, bool bAsync /*= false*/)
	{
		TNotifyUI Msg;
		Msg.pSender = pControl;
		Msg.sType = pstrMessage;
		Msg.wParam = wParam;
		Msg.lParam = lParam;
		SendNotify(Msg, bAsync);
	}

	void CPaintManagerUI::SendNotify(TNotifyUI& Msg, bool bAsync /*= false*/)
	{
		Msg.ptMouse = m_ptLastMousePos;
		Msg.dwTimestamp = ::GetTickCount();
		if( m_bUsedVirtualWnd )
		{
			Msg.sVirtualWnd = Msg.pSender->GetVirtualWnd();
		}

		if( !bAsync ) {
			// Send to all listeners
			if( Msg.pSender != NULL ) {
				if( Msg.pSender->OnNotify ){
					if(Msg.pSender->OnNotify(&Msg))
						return;
				}
				//if( Msg.pSender->OnNotify ) Msg.pSender->OnNotify(&Msg);
			}
			for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
				static_cast<INotifyUI*>(m_aNotifiers[i])->Notify(Msg);
			}
		}
		else {
			TNotifyUI *pMsg = new TNotifyUI;
			pMsg->pSender = Msg.pSender;
			pMsg->sType = Msg.sType;
			pMsg->wParam = Msg.wParam;
			pMsg->lParam = Msg.lParam;
			pMsg->ptMouse = Msg.ptMouse;
			pMsg->dwTimestamp = Msg.dwTimestamp;
			m_aAsyncNotify.Add(pMsg);
		}
	}

	bool CPaintManagerUI::UseParentResource(CPaintManagerUI* pm)
	{
		if( pm == NULL ) {
			m_pParentResourcePM = NULL;
			return true;
		}
		if( pm == this ) return false;

		CPaintManagerUI* pParentPM = pm->GetParentResource();
		while( pParentPM ) {
			if( pParentPM == this ) return false;
			pParentPM = pParentPM->GetParentResource();
		}
		m_pParentResourcePM = pm;
		return true;
	}

	CPaintManagerUI* CPaintManagerUI::GetParentResource() const
	{
		return m_pParentResourcePM;
	}

	DWORD CPaintManagerUI::GetDefaultDisabledColor() const
	{
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultDisabledColor();
		return m_dwDefaultDisabledColor;
	}

	void CPaintManagerUI::SetDefaultDisabledColor(DWORD dwColor)
	{
		m_dwDefaultDisabledColor = dwColor;
	}

	DWORD CPaintManagerUI::GetDefaultFontColor() const
	{
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultFontColor();
		return m_dwDefaultFontColor;
	}

	void CPaintManagerUI::SetDefaultFontColor(DWORD dwColor)
	{
		m_dwDefaultFontColor = dwColor;
	}

	DWORD CPaintManagerUI::GetDefaultLinkFontColor() const
	{
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultLinkFontColor();
		return m_dwDefaultLinkFontColor;
	}

	void CPaintManagerUI::SetDefaultLinkFontColor(DWORD dwColor)
	{
		m_dwDefaultLinkFontColor = dwColor;
	}

	DWORD CPaintManagerUI::GetDefaultLinkHoverFontColor() const
	{
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultLinkHoverFontColor();
		return m_dwDefaultLinkHoverFontColor;
	}

	void CPaintManagerUI::SetDefaultLinkHoverFontColor(DWORD dwColor)
	{
		m_dwDefaultLinkHoverFontColor = dwColor;
	}

	DWORD CPaintManagerUI::GetDefaultSelectedBkColor() const
	{
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultSelectedBkColor();
		return m_dwDefaultSelectedBkColor;
	}

	void CPaintManagerUI::SetDefaultSelectedBkColor(DWORD dwColor)
	{
		m_dwDefaultSelectedBkColor = dwColor;
	}

	TFontInfo* CPaintManagerUI::GetDefaultFontInfo()
	{
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultFontInfo();

		if( m_DefaultFontInfo.tm.tmHeight == 0 ) {
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, m_DefaultFontInfo.hFont);
			::GetTextMetrics(m_hDcPaint, &m_DefaultFontInfo.tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
		return &m_DefaultFontInfo;
	}

	void CPaintManagerUI::SetDefaultFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
	{
		LOGFONT lf = { 0 };
		::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		_tcscpy(lf.lfFaceName, pStrFontName);
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfHeight = -nSize;
		if( bBold ) lf.lfWeight += FW_BOLD;
		if( bUnderline ) lf.lfUnderline = TRUE;
		if( bItalic ) lf.lfItalic = TRUE;
		HFONT hFont = ::CreateFontIndirect(&lf);
		if( hFont == NULL ) return;

		::DeleteObject(m_DefaultFontInfo.hFont);
		m_DefaultFontInfo.hFont = hFont;
		m_DefaultFontInfo.sFontName = pStrFontName;
		m_DefaultFontInfo.iSize = nSize;
		m_DefaultFontInfo.bBold = bBold;
		m_DefaultFontInfo.bUnderline = bUnderline;
		m_DefaultFontInfo.bItalic = bItalic;
		::ZeroMemory(&m_DefaultFontInfo.tm, sizeof(m_DefaultFontInfo.tm));
		if( m_hDcPaint ) {
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
			::GetTextMetrics(m_hDcPaint, &m_DefaultFontInfo.tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
	}

	DWORD CPaintManagerUI::GetCustomFontCount() const
	{
		return m_aCustomFonts.GetSize();
	}

	HFONT CPaintManagerUI::AddFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
	{
		LOGFONT lf = { 0 };
		::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		_tcscpy(lf.lfFaceName, pStrFontName);
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfHeight = -nSize;
		lf.lfQuality = CLEARTYPE_QUALITY;
		if( bBold ) lf.lfWeight += FW_BOLD;
		if( bUnderline ) lf.lfUnderline = TRUE;
		if( bItalic ) lf.lfItalic = TRUE;
		HFONT hFont = ::CreateFontIndirect(&lf);
		if( hFont == NULL ) return NULL;

		TFontInfo* pFontInfo = new TFontInfo;
		if( !pFontInfo ) return false;
		::ZeroMemory(pFontInfo, sizeof(TFontInfo));
		pFontInfo->hFont = hFont;
		pFontInfo->sFontName = pStrFontName;
		pFontInfo->iSize = nSize;
		pFontInfo->bBold = bBold;
		pFontInfo->bUnderline = bUnderline;
		pFontInfo->bItalic = bItalic;
		if( m_hDcPaint ) {
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
			::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
		if( !m_aCustomFonts.Add(pFontInfo) ) {
			::DeleteObject(hFont);
			delete pFontInfo;
			return NULL;
		}

		return hFont;
	}

	HFONT CPaintManagerUI::AddFontAt(int index, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
	{
		LOGFONT lf = { 0 };
		::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		_tcscpy(lf.lfFaceName, pStrFontName);
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfHeight = -nSize;
		lf.lfQuality = CLEARTYPE_QUALITY;
		if( bBold ) lf.lfWeight += FW_BOLD;
		if( bUnderline ) lf.lfUnderline = TRUE;
		if( bItalic ) lf.lfItalic = TRUE;
		HFONT hFont = ::CreateFontIndirect(&lf);
		if( hFont == NULL ) return NULL;

		TFontInfo* pFontInfo = new TFontInfo;
		if( !pFontInfo ) return false;
		::ZeroMemory(pFontInfo, sizeof(TFontInfo));
		pFontInfo->hFont = hFont;
		pFontInfo->sFontName = pStrFontName;
		pFontInfo->iSize = nSize;
		pFontInfo->bBold = bBold;
		pFontInfo->bUnderline = bUnderline;
		pFontInfo->bItalic = bItalic;
		if( m_hDcPaint ) {
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
			::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
		if( !m_aCustomFonts.InsertAt(index, pFontInfo) ) {
			::DeleteObject(hFont);
			delete pFontInfo;
			return NULL;
		}

		return hFont;
	}

	HFONT CPaintManagerUI::GetFont(int index)
	{
		if( index < 0 || index >= m_aCustomFonts.GetSize() ) return GetDefaultFontInfo()->hFont;
		TFontInfo* pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[index]);
		return pFontInfo->hFont;
	}

	HFONT CPaintManagerUI::GetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
	{
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
				pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
				return pFontInfo->hFont;
		}
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetFont(pStrFontName, nSize, bBold, bUnderline, bItalic);
		return NULL;
	}

	bool CPaintManagerUI::FindFont(HFONT hFont)
	{
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( pFontInfo->hFont == hFont ) return true;
		}
		if( m_pParentResourcePM ) return m_pParentResourcePM->FindFont(hFont);
		return false;
	}

	bool CPaintManagerUI::FindFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
	{
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
				pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
				return true;
		}
		if( m_pParentResourcePM ) return m_pParentResourcePM->FindFont(pStrFontName, nSize, bBold, bUnderline, bItalic);
		return false;
	}

	int CPaintManagerUI::GetFontIndex(HFONT hFont)
	{
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( pFontInfo->hFont == hFont ) return it;
		}
		return -1;
	}

	int CPaintManagerUI::GetFontIndex(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
	{
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
				pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
				return it;
		}
		return -1;
	}

	bool CPaintManagerUI::RemoveFont(HFONT hFont)
	{
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( pFontInfo->hFont == hFont ) {
				::DeleteObject(pFontInfo->hFont);
				delete pFontInfo;
				return m_aCustomFonts.Remove(it);
			}
		}

		return false;
	}

	bool CPaintManagerUI::RemoveFontAt(int index)
	{
		if( index < 0 || index >= m_aCustomFonts.GetSize() ) return false;
		TFontInfo* pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[index]);
		::DeleteObject(pFontInfo->hFont);
		delete pFontInfo;
		return m_aCustomFonts.Remove(index);
	}

	void CPaintManagerUI::RemoveAllFonts()
	{
		TFontInfo* pFontInfo;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			::DeleteObject(pFontInfo->hFont);
			delete pFontInfo;
		}
		m_aCustomFonts.Empty();
	}

	TFontInfo* CPaintManagerUI::GetFontInfo(int index)
	{
		if( index < 0 || index >= m_aCustomFonts.GetSize() ) return GetDefaultFontInfo();
		TFontInfo* pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[index]);
		if( pFontInfo->tm.tmHeight == 0 ) {
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, pFontInfo->hFont);
			::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
		return pFontInfo;
	}

	TFontInfo* CPaintManagerUI::GetFontInfo(HFONT hFont)
	{
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( pFontInfo->hFont == hFont ) {
				if( pFontInfo->tm.tmHeight == 0 ) {
					HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, pFontInfo->hFont);
					::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
					::SelectObject(m_hDcPaint, hOldFont);
				}
				return pFontInfo;
			}
		}

		if( m_pParentResourcePM ) return m_pParentResourcePM->GetFontInfo(hFont);
		return GetDefaultFontInfo();
	}

	const TImageInfo* CPaintManagerUI::GetImage(LPCTSTR bitmap)
	{
		TImageInfo* data = static_cast<TImageInfo*>(m_mImageHash.Find(bitmap));
		if( !data && m_pParentResourcePM ) return m_pParentResourcePM->GetImage(bitmap);
		else return data;
	}

	const TImageInfo* CPaintManagerUI::GetImageEx(LPCTSTR bitmap, LPCTSTR type, DWORD mask)
	{
		TImageInfo* data = static_cast<TImageInfo*>(m_mImageHash.Find(bitmap));
		if( !data ) {
			if( AddImage(bitmap, type, mask) ) {
				data = static_cast<TImageInfo*>(m_mImageHash.Find(bitmap));
			}
		}

		return data;
	}

	const TImageInfo* CPaintManagerUI::AddImage(LPCTSTR bitmap, LPCTSTR type, DWORD mask)
	{
		TImageInfo* data = NULL;
		if( type != NULL ) {
			if( isdigit(*bitmap) ) {
				LPTSTR pstr = NULL;
				int iIndex = _tcstol(bitmap, &pstr, 10);
				data = CRenderEngine::LoadImage(iIndex, type, mask);
			}
		}
		else {
			data = CRenderEngine::LoadImage(bitmap, NULL, mask);
		}

		if( !data ) return NULL;
		if( type != NULL ) data->sResType = type;
		data->dwMask = mask;
		if( !m_mImageHash.Insert(bitmap, data) ) {
			::DeleteObject(data->hBitmap);
			delete data;
		}

		return data;
	}

	const TImageInfo* CPaintManagerUI::AddImage(LPCTSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha)
	{
		if( hBitmap == NULL || iWidth <= 0 || iHeight <= 0 ) return NULL;

		TImageInfo* data = new TImageInfo;
		data->hBitmap = hBitmap;
		data->nX = iWidth;
		data->nY = iHeight;
		data->alphaChannel = bAlpha;
		//data->sResType = _T("");
		data->dwMask = 0;
		if( !m_mImageHash.Insert(bitmap, data) ) {
			::DeleteObject(data->hBitmap);
			delete data;
		}

		return data;
	}

	bool CPaintManagerUI::RemoveImage(LPCTSTR bitmap)
	{
		const TImageInfo* data = GetImage(bitmap);
		if( !data ) return false;

		CRenderEngine::FreeImage(data) ;

		return m_mImageHash.Remove(bitmap);
	}

	void CPaintManagerUI::RemoveAllImages()
	{
		TImageInfo* data;
		for( int i = 0; i< m_mImageHash.GetSize(); i++ ) {
			if(LPCTSTR key = m_mImageHash.GetAt(i)) {
				data = static_cast<TImageInfo*>(m_mImageHash.Find(key, false));
				if (data) {
					CRenderEngine::FreeImage(data);
				}
			}
		}
		m_mImageHash.RemoveAll();
	}

	void CPaintManagerUI::ReloadAllImages()
	{
		bool bRedraw = false;
		TImageInfo* data;
		TImageInfo* pNewData;
		for( int i = 0; i< m_mImageHash.GetSize(); i++ ) {
			if(LPCTSTR bitmap = m_mImageHash.GetAt(i)) {
				data = static_cast<TImageInfo*>(m_mImageHash.Find(bitmap));
				if( data != NULL ) {
					if( !data->sResType.IsEmpty() ) {
						if( isdigit(*bitmap) ) {
							LPTSTR pstr = NULL;
							int iIndex = _tcstol(bitmap, &pstr, 10);
							pNewData = CRenderEngine::LoadImage(iIndex, data->sResType.GetData(), data->dwMask);
						}
					}
					else {
						pNewData = CRenderEngine::LoadImage(bitmap, NULL, data->dwMask);
					}
					if( pNewData == NULL ) continue;

					if( data->hBitmap != NULL ) ::DeleteObject(data->hBitmap);
					data->hBitmap = pNewData->hBitmap;
					data->nX = pNewData->nX;
					data->nY = pNewData->nY;
					data->alphaChannel = pNewData->alphaChannel;

					delete pNewData;
					bRedraw = true;
				}
			}
		}
		if( bRedraw && m_pRoot ) m_pRoot->Invalidate();
	}

	void CPaintManagerUI::AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList)
	{
		CDuiString* pDefaultAttr = new CDuiString(pStrControlAttrList);
		if (pDefaultAttr != NULL)
		{
			if (m_DefaultAttrHash.Find(pStrControlName) == NULL)
				m_DefaultAttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr);
			else
				delete pDefaultAttr;
		}
	}

	LPCTSTR CPaintManagerUI::GetDefaultAttributeList(LPCTSTR pStrControlName) const
	{
		CDuiString* pDefaultAttr = static_cast<CDuiString*>(m_DefaultAttrHash.Find(pStrControlName));
		if( !pDefaultAttr && m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultAttributeList(pStrControlName);

		if( pDefaultAttr ) return pDefaultAttr->GetData();
		else return NULL;
	}

	bool CPaintManagerUI::RemoveDefaultAttributeList(LPCTSTR pStrControlName)
	{
		CDuiString* pDefaultAttr = static_cast<CDuiString*>(m_DefaultAttrHash.Find(pStrControlName));
		if( !pDefaultAttr ) return false;

		delete pDefaultAttr;
		return m_DefaultAttrHash.Remove(pStrControlName);
	}

	const CStdStringPtrMap& CPaintManagerUI::GetDefaultAttribultes() const
	{
		return m_DefaultAttrHash;
	}

	void CPaintManagerUI::RemoveAllDefaultAttributeList()
	{
		CDuiString* pDefaultAttr;
		for( int i = 0; i< m_DefaultAttrHash.GetSize(); i++ ) {
			if(LPCTSTR key = m_DefaultAttrHash.GetAt(i)) {
				pDefaultAttr = static_cast<CDuiString*>(m_DefaultAttrHash.Find(key));
				delete pDefaultAttr;
			}
		}
		m_DefaultAttrHash.RemoveAll();
	}

	CControlUI* CPaintManagerUI::GetRoot() const
	{
		ASSERT(m_pRoot);
		return m_pRoot;
	}

	CControlUI* CPaintManagerUI::FindControl(POINT pt) const
	{
		ASSERT(m_pRoot);
		return m_pRoot->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
	}

	CControlUI* CPaintManagerUI::FindControl(LPCTSTR pstrName) const
	{
		ASSERT(m_pRoot);
		return static_cast<CControlUI*>(m_mNameHash.Find(pstrName));
	}

	CControlUI* CPaintManagerUI::FindSubControlByPoint(CControlUI* pParent, POINT pt) const
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		return pParent->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
	}

	CControlUI* CPaintManagerUI::FindSubControlByName(CControlUI* pParent, LPCTSTR pstrName) const
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		return pParent->FindControl(__FindControlFromName, (LPVOID)pstrName, UIFIND_ALL);
	}

	CControlUI* CPaintManagerUI::FindSubControlByClass(CControlUI* pParent, LPCTSTR pstrClass, int iIndex)
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		m_aFoundControls.Resize(iIndex + 1);
		return pParent->FindControl(__FindControlFromClass, (LPVOID)pstrClass, UIFIND_ALL);
	}

	CStdPtrArray* CPaintManagerUI::FindSubControlsByClass(CControlUI* pParent, LPCTSTR pstrClass)
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		m_aFoundControls.Empty();
		pParent->FindControl(__FindControlsFromClass, (LPVOID)pstrClass, UIFIND_ALL);
		return &m_aFoundControls;
	}

	CStdPtrArray* CPaintManagerUI::GetSubControlsByClass()
	{
		return &m_aFoundControls;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromNameHash(CControlUI* pThis, LPVOID pData)
	{
		CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(pData);
		const CDuiString& sName = pThis->GetName();
		if( sName.IsEmpty() ) return NULL;
		// Add this control to the hash list
		pManager->m_mNameHash.Set(sName, pThis);
		return NULL; // Attempt to add all controls
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromCount(CControlUI* /*pThis*/, LPVOID pData)
	{
		int* pnCount = static_cast<int*>(pData);
		(*pnCount)++;
		return NULL;  // Count all controls
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromPoint(CControlUI* pThis, LPVOID pData)
	{
		LPPOINT pPoint = static_cast<LPPOINT>(pData);
		return ::PtInRect(&pThis->GetPos(), *pPoint) ? pThis : NULL;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromTab(CControlUI* pThis, LPVOID pData)
	{
		FINDTABINFO* pInfo = static_cast<FINDTABINFO*>(pData);
		if( pInfo->pFocus == pThis ) {
			if( pInfo->bForward ) pInfo->bNextIsIt = true;
			return pInfo->bForward ? NULL : pInfo->pLast;
		}
		if( (pThis->GetControlFlags() & UIFLAG_TABSTOP) == 0 ) return NULL;
		pInfo->pLast = pThis;
		if( pInfo->bNextIsIt ) return pThis;
		if( pInfo->pFocus == NULL ) return pThis;
		return NULL;  // Examine all controls
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromShortcut(CControlUI* pThis, LPVOID pData)
	{
		if( !pThis->IsVisible() ) return NULL; 
		FINDSHORTCUT* pFS = static_cast<FINDSHORTCUT*>(pData);
		if( pFS->ch == toupper(pThis->GetShortcut()) ) pFS->bPickNext = true;
		if( _tcsstr(pThis->GetClass(), _T("LabelUI")) != NULL ) return NULL;   // Labels never get focus!
		return pFS->bPickNext ? pThis : NULL;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromUpdate(CControlUI* pThis, LPVOID pData)
	{
		return pThis->IsUpdateNeeded() ? pThis : NULL;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromName(CControlUI* pThis, LPVOID pData)
	{
		LPCTSTR pstrName = static_cast<LPCTSTR>(pData);
		const CDuiString& sName = pThis->GetName();
		if( sName.IsEmpty() ) return NULL;
		return (_tcsicmp(sName, pstrName) == 0) ? pThis : NULL;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromClass(CControlUI* pThis, LPVOID pData)
	{
		LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
		LPCTSTR pType = pThis->GetClass();
		CStdPtrArray* pFoundControls = pThis->GetManager()->GetSubControlsByClass();
		if( _tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0 ) {
			int iIndex = -1;
			while( pFoundControls->GetAt(++iIndex) != NULL ) ;
			if( iIndex < pFoundControls->GetSize() ) pFoundControls->SetAt(iIndex, pThis);
		}
		if( pFoundControls->GetAt(pFoundControls->GetSize() - 1) != NULL ) return pThis; 
		return NULL;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlsFromClass(CControlUI* pThis, LPVOID pData)
	{
		LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
		LPCTSTR pType = pThis->GetClass();
		if( _tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0 ) 
			pThis->GetManager()->GetSubControlsByClass()->Add((LPVOID)pThis);
		return NULL;
	}

	bool CPaintManagerUI::TranslateAccelerator(LPMSG pMsg)
	{
		for (int i = 0; i < m_aTranslateAccelerator.GetSize(); i++)
		{
			bool bHandled = false;
			LRESULT lResult = static_cast<ITranslateAccelerator *>(m_aTranslateAccelerator[i])->TranslateAccelerator(pMsg);
			return lResult == S_OK;
		}
		return false;
	}

	bool CPaintManagerUI::TranslateMessage(const LPMSG pMsg)
	{
		// Pretranslate Message takes care of system-wide messages, such as
		// tabbing and shortcut key-combos. We'll look for all messages for
		// each window and any child control attached.
		UINT uStyle = GetWindowStyle(pMsg->hwnd);
		UINT uChildRes = uStyle & WS_CHILD;	
		LRESULT lRes = 0;
		if (uChildRes != 0)
		{
			HWND hWndParent = ::GetParent(pMsg->hwnd);

			for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) 
			{
				CPaintManagerUI* pT = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);        
				HWND hTempParent = hWndParent;
				while(hTempParent)
				{
					if(pMsg->hwnd == pT->GetPaintWindow() || hTempParent == pT->GetPaintWindow())
					{
						if (pT->TranslateAccelerator(pMsg))
							return true;

						pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes);
						// 					if( pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) 
						// 						return true;
						// 
						// 					return false;  //code by redrain
					}
					hTempParent = GetParent(hTempParent);
				}
			}
		}
		else
		{
			for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) 
			{
				CPaintManagerUI* pT = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
				if(pMsg->hwnd == pT->GetPaintWindow())
				{
					if (pT->TranslateAccelerator(pMsg))
						return true;

					if( pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) 
						return true;

					return false;
				}
			}
		}
		return false;
	}


	bool CPaintManagerUI::AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
	{
		ASSERT(m_aTranslateAccelerator.Find(pTranslateAccelerator) < 0);
		return m_aTranslateAccelerator.Add(pTranslateAccelerator);
	}

	bool CPaintManagerUI::RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
	{
		for (int i = 0; i < m_aTranslateAccelerator.GetSize(); i++)
		{
			if (static_cast<ITranslateAccelerator *>(m_aTranslateAccelerator[i]) == pTranslateAccelerator)
			{
				return m_aTranslateAccelerator.Remove(i);
			}
		}
		return false;
	}

	bool CPaintManagerUI::AddAnimationJob( const CDxAnimationUI& _DxAnimationUI )
	{
		CDxAnimationUI* pDxAnimationJob = new CDxAnimationUI(_DxAnimationUI);
		if( pDxAnimationJob == NULL ) return false;
		::InvalidateRect(m_hWndPaint, NULL, FALSE);
		bool nRet = m_anim.AddJob(pDxAnimationJob);
		return nRet;
	}

	HPEN CPaintManagerUI::GetThemePen( UITYPE_COLOR Index ) const
	{
		if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return NULL;
		if( m_hPens[Index] == NULL ) m_hPens[Index] = ::CreatePen(PS_SOLID, 1, m_clrColors[Index][0]);
		return m_hPens[Index];
	}

	HFONT CPaintManagerUI::GetThemeFont( UITYPE_FONT Index ) const
	{
		if( Index <= UIFONT__FIRST || Index >= UIFONT__LAST ) return NULL;
		if( m_hFonts[Index] == NULL ) m_hFonts[Index] = ::CreateFontIndirect(&m_aLogFonts[Index]);
		return m_hFonts[Index];
	}

	HBRUSH CPaintManagerUI::GetThemeBrush( UITYPE_COLOR Index ) const
	{
		if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return NULL;
		if( m_hBrushes[Index] == NULL ) m_hBrushes[Index] = ::CreateSolidBrush(m_clrColors[Index][0]);
		return m_hBrushes[Index];
	}

	COLORREF CPaintManagerUI::GetThemeColor( UITYPE_COLOR Index ) const
	{
		if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return RGB(0,0,0);
		return m_clrColors[Index][0];
	}

	HICON CPaintManagerUI::GetThemeIcon( int Index, int cxySize ) const
	{
		if( m_himgIcons16 == NULL ) {
			m_himgIcons16 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS16), 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
			m_himgIcons24 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS16), 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
			m_himgIcons32 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS16), 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
			m_himgIcons50 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS50), 50, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
		}
		if( cxySize == 16 ) return ImageList_GetIcon(m_himgIcons16, Index, ILD_NORMAL);
		else if( cxySize == 24 ) return ImageList_GetIcon(m_himgIcons24, Index, ILD_NORMAL);
		else if( cxySize == 32 ) return ImageList_GetIcon(m_himgIcons32, Index, ILD_NORMAL);
		else if( cxySize == 50 ) return ImageList_GetIcon(m_himgIcons50, Index, ILD_NORMAL);
		return NULL;
	}

	const TEXTMETRIC& CPaintManagerUI::GetThemeFontInfo( UITYPE_FONT Index ) const
	{
		if( Index <= UIFONT__FIRST || Index >= UIFONT__LAST ) return m_aTextMetrics[0];
		if( m_aTextMetrics[Index].tmHeight == 0 ) {
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, GetThemeFont(Index));
			::GetTextMetrics(m_hDcPaint, &m_aTextMetrics[Index]);
			::SelectObject(m_hDcPaint, hOldFont);
		}
		return m_aTextMetrics[Index];
	}

	bool CPaintManagerUI::GetThemeColorPair( UITYPE_COLOR Index, COLORREF& clr1, COLORREF& clr2 ) const
	{
		if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return false;
		clr1 = m_clrColors[Index][0];
		clr2 = m_clrColors[Index][1];
		return true;
	}

	void CPaintManagerUI::UsedVirtualWnd(bool bUsed)
	{
		m_bUsedVirtualWnd = bUsed;
	}

	bool CPaintManagerUI::AddPropertyAction( LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop /*= false*/,bool bAutoStart /*= true*/ )
	{
		return SetPropertyAction(pAGroupName,pPropertyName,pType,pStartValue,pEndValue,iInterval,iTimer,iDelay,bRevers,bLoop,bAutoStart,true);
	}

	bool CPaintManagerUI::AddPropertyAction( TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop /*= false*/,bool bAutoStart /*= true*/ )
	{
		return SetPropertyAction(tAGroup,pPropertyName,pType,pStartValue,pEndValue,iInterval,iTimer,iDelay,bRevers,bLoop,bAutoStart,true);
	}

	bool CPaintManagerUI::SetPropertyAction( TAGroup& tAGroup,TProperty* tProperty,bool bAutoCreate /*= false*/ )
	{
		if(!tProperty)
			return false;

		int nIndex = -1;
		TProperty* pTProperty = GetPropertyAction(tAGroup,tProperty->sName.GetData(),tProperty->sType.GetData(),nIndex);
		if(!pTProperty && !bAutoCreate)
			return false;

		if(pTProperty){
			delete pTProperty;
			pTProperty = NULL;
		}

		tAGroup.mPropertys.Remove(nIndex);
		return tAGroup.mPropertys.Add(tProperty);
	}

	bool CPaintManagerUI::SetPropertyAction( LPCTSTR pAGroupName,TProperty* tProperty,bool bAutoCreate /*= false*/ )
	{
		if(!tProperty)
			return false;

		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return false;

		return SetPropertyAction(*pTAGroup,tProperty,bAutoCreate);
	}

	bool CPaintManagerUI::SetPropertyAction( LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop /*= false*/,bool bAutoStart /*= true*/,bool bAutoCreate /*= false*/ )
	{
		if(!pAGroupName || !pPropertyName || !pType || (!pStartValue && !pEndValue))
			return false;

		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return false;

		return SetPropertyAction(*pTAGroup,pPropertyName,pType,pStartValue,pEndValue,iInterval,iTimer,iDelay,bRevers,bLoop,bAutoStart,bAutoCreate);
	}

	bool CPaintManagerUI::SetPropertyAction( TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop /*= false*/,bool bAutoStart /*= true*/,bool bAutoCreate /*= false*/ )
	{
		if(!pPropertyName || !pType || (!pStartValue && !pEndValue))
			return false;

		TProperty* pTProperty = new TProperty();

		if(pTProperty && SetPropertyActionParse(*pTProperty,pPropertyName,pType,pStartValue,pEndValue,iInterval,iTimer,iDelay,bRevers,bLoop,bAutoStart)){
			if(SetPropertyAction(tAGroup.sName.GetData(),pTProperty,bAutoCreate))
				return true;
			delete pTProperty;
			pTProperty = NULL;
		}
		return false;
	}

	TProperty* CPaintManagerUI::GetPropertyAction( LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType ) const
	{
		int nIndex = -1;
		return GetPropertyAction(pAGroupName,pPropertyName,pType,nIndex);
	}

	TProperty* CPaintManagerUI::GetPropertyAction( TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType ) const
	{
		int nIndex = -1;
		return GetPropertyAction(tAGroup,pPropertyName,pType,nIndex);
	}

	TProperty* CPaintManagerUI::GetPropertyAction( LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType,int& iIndex ) const
	{
		if(!pAGroupName || !pPropertyName)
			return NULL;

		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return NULL;

		return GetPropertyAction(*pTAGroup,pPropertyName,pType,iIndex);
	}

	TProperty* CPaintManagerUI::GetPropertyAction( TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType,int& iIndex ) const
	{
		if(!pPropertyName || !pType)
			return NULL;

		iIndex = -1;

		for(int i = 0;i < tAGroup.mPropertys.GetSize();i++){
			TProperty* nTProperty = tAGroup.mPropertys.GetAt(i);
			if(nTProperty && _tcscmp(nTProperty->sName.GetData(),pPropertyName) == 0 && _tcscmp(nTProperty->sType.GetData(),pType) == 0){
				iIndex = i;
				return nTProperty;
			}
			break;
		}
		return NULL;
	}

	bool CPaintManagerUI::RemovePropertyAction( LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType )
	{
		if(!pAGroupName || !pPropertyName)
			return false;

		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return false;

		int nIndex = -1;
		TProperty* pTProperty = GetPropertyAction(*pTAGroup,pPropertyName,pType,nIndex);
		if(!pTProperty)
			return true;

		delete pTProperty;
		return pTAGroup->mPropertys.Remove(nIndex);
	}

	bool CPaintManagerUI::SetPropertyActionParse( TProperty& nTProperty,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop /*= false*/,bool bAutoStart /*= true*/ )
	{
		if(!pPropertyName || !pType || (!pStartValue && !pEndValue))
			return false;

		if( _tcscmp(pType, _T("int")) == 0 || _tcscmp(pType, _T("image.fade")) == 0 ) {
			nTProperty.nStartValue.iValue = _ttoi(pStartValue);
			nTProperty.nEndValue.iValue = _ttoi(pEndValue);
		}
		else if( _tcscmp(pType, _T("color")) == 0 || _tcscmp(pType, _T("image.mask")) == 0 ) {
			if( *pStartValue == _T('#')) pStartValue = ::CharNext(pStartValue);
			LPTSTR pStartStr = NULL;
			nTProperty.nStartValue.dwValue = _tcstoul(pStartValue, &pStartStr, 16);

			if( *pEndValue == _T('#')) pEndValue = ::CharNext(pEndValue);
			LPTSTR pEndStr = NULL;
			nTProperty.nEndValue.dwValue = _tcstoul(pEndValue, &pEndStr, 16);
		}
		else if( _tcscmp(pType, _T("rect")) == 0 || _tcscmp(pType, _T("image.source")) == 0 || _tcscmp(pType, _T("image.corner")) == 0 || _tcscmp(pType, _T("image.dest")) == 0 ) {
			LPTSTR pStartStr = NULL;
			nTProperty.nStartValue.rcValue.left = _tcstol(pStartValue, &pStartStr, 10);		ASSERT(pStartStr);
			nTProperty.nStartValue.rcValue.top = _tcstol(pStartStr + 1, &pStartStr, 10);	ASSERT(pStartStr);
			nTProperty.nStartValue.rcValue.right = _tcstol(pStartStr + 1, &pStartStr, 10);	ASSERT(pStartStr);
			nTProperty.nStartValue.rcValue.bottom = _tcstol(pStartStr + 1, &pStartStr, 10);	ASSERT(pStartStr); 

			LPTSTR pEndStr = NULL;
			nTProperty.nEndValue.rcValue.left = _tcstol(pEndValue, &pEndStr, 10);		ASSERT(pEndStr);
			nTProperty.nEndValue.rcValue.top = _tcstol(pEndStr + 1, &pEndStr, 10);    ASSERT(pEndStr);
			nTProperty.nEndValue.rcValue.right = _tcstol(pEndStr + 1, &pEndStr, 10);  ASSERT(pEndStr);
			nTProperty.nEndValue.rcValue.bottom = _tcstol(pEndStr + 1, &pEndStr, 10); ASSERT(pEndStr); 
		}
		else if( _tcscmp(pType, _T("point")) == 0 ) {
			LPTSTR pStartStr = NULL;
			nTProperty.nStartValue.siValue.cx = _tcstol(pStartValue, &pStartStr, 10);	ASSERT(pStartStr);
			nTProperty.nStartValue.siValue.cy  = _tcstol(pStartStr + 1, &pStartStr, 10); ASSERT(pStartStr);

			LPTSTR pEndStr = NULL;
			nTProperty.nStartValue.siValue.cx = _tcstol(pEndValue, &pEndStr, 10);	ASSERT(pEndStr);
			nTProperty.nStartValue.siValue.cy  = _tcstol(pEndStr + 1, &pEndStr, 10); ASSERT(pEndStr);
		}
		else false;

		if(_tcscmp(_T("none"),pStartValue) == 0){
			::ZeroMemory((void*)&nTProperty.nStartValue, sizeof(TProperty::unValue));
			nTProperty.sStartValue = _T("none");
		}
		if(_tcscmp(_T("none"),pEndValue) == 0){
			::ZeroMemory((void*)&nTProperty.nEndValue, sizeof(TProperty::unValue));
			nTProperty.sEndValue = _T("none");
		}

		nTProperty.sName		= pPropertyName;
		nTProperty.sType		= pType;
		nTProperty.bLoop		= bLoop;
		nTProperty.bReverse		= bRevers;
		nTProperty.bAutoStart	= bAutoStart;
		nTProperty.uInterval	= iInterval < 0?0:iInterval;
		nTProperty.uTimer		= iTimer < iInterval ?iInterval:iTimer;
		nTProperty.uDelay		= iDelay < 0?0:iDelay;

		return true;
	}

	int CPaintManagerUI::HasPropertyMsgType( TAGroup& tAGroup,LPCTSTR pType )
	{
		if(!pType)
			return -1;

		for(int i = 0;i < tAGroup.mPropertys.GetSize();i++){
			TProperty* pTProperty = tAGroup.mPropertys.GetAt(i);
			if(!pTProperty)
				continue;

			if(_tcscmp(pType,pTProperty->sType.GetData()) == 0)
				return 1;
		}
		return 0;
	}

	int CPaintManagerUI::HasPropertyMsgType( LPCTSTR pAGroupName,LPCTSTR pType )
	{
		if(!pAGroupName || !pType)
			return -1;

		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return -1;

		return HasPropertyMsgType(*pTAGroup,pType);
	}

	bool CPaintManagerUI::AddActionScriptGroup( LPCTSTR pAGroupName,LPCTSTR pNotifyName,int iDefaultInterval /*= 0*/,int iDefaultTimer /*= 500*/,bool bDefaultReverse /*= false*/,bool bDefaultLoop /*= false*/,bool bDefaultAutoStart /*= true*/ )
	{
		if(!pAGroupName || !pNotifyName || GetActionScriptGroup(pAGroupName))
			return false;

		TAGroup* pTAGroup			= new TAGroup();
		pTAGroup->sName				= pAGroupName;
		pTAGroup->sMsgType			= _T("notify");
		pTAGroup->sMsgValue			= pNotifyName;
		pTAGroup->iEventValue		= UIEVENT__ALL;
		pTAGroup->uDefaultInterval	= iDefaultInterval < 0?0:iDefaultInterval;
		pTAGroup->uDefaultTimer		= iDefaultTimer < (int)pTAGroup->uDefaultInterval?pTAGroup->uDefaultInterval:iDefaultTimer;
		pTAGroup->bDefaultLoop		= bDefaultLoop;
		pTAGroup->bDefaultAutoStart	= bDefaultAutoStart;
		pTAGroup->bDefaultReverse	= bDefaultReverse;

		m_mActionScript.Set(pAGroupName,pTAGroup);
		return true;
	}

	bool CPaintManagerUI::AddActionScriptGroup( LPCTSTR pAGroupName,EVENTTYPE_UI pEventType,int iDefaultInterval /*= 0*/,int iDefaultTimer /*= 500*/,bool bDefaultReverse /*= false*/,bool bDefaultLoop /*= false*/,bool bDefaultAutoStart /*= true*/ )
	{
		if(!pAGroupName || pEventType == UIEVENT__ALL || GetActionScriptGroup(pAGroupName))
			return false;

		TAGroup* pTAGroup			= new TAGroup();
		pTAGroup->sName				= pAGroupName;
		pTAGroup->sMsgType			= _T("event");
		pTAGroup->sMsgValue.Empty();
		pTAGroup->iEventValue		= pEventType;
		pTAGroup->uDefaultInterval	= iDefaultInterval < 0?0:iDefaultInterval;
		pTAGroup->uDefaultTimer		= iDefaultTimer < (int)pTAGroup->uDefaultInterval?pTAGroup->uDefaultInterval:iDefaultTimer;
		pTAGroup->bDefaultLoop		= bDefaultLoop;
		pTAGroup->bDefaultAutoStart	= bDefaultAutoStart;
		pTAGroup->bDefaultReverse	= bDefaultReverse;

		m_mActionScript.Set(pAGroupName,pTAGroup);
		return true;
	}

	bool CPaintManagerUI::SetActionScriptGroup( TAGroup** tAGroup,bool bMergerProperty /*= false*/ )
	{
		if(!*tAGroup)
			return false;

		TAGroup* pTAGroup = m_mActionScript.Find((*tAGroup)->sName.GetData());

		if(!pTAGroup)
			return false;

		if(bMergerProperty){
			for(int i = 0;i < (*tAGroup)->mPropertys.GetSize();i++){
				TProperty* pTProperty = (*tAGroup)->mPropertys.GetAt(i);
				if(!pTProperty)
					continue;

				SetPropertyAction(*pTAGroup,pTProperty,true);
			}
			for(int i = 0;i < pTAGroup->mPropertys.GetSize();i++){
				TProperty* pTProperty = pTAGroup->mPropertys.GetAt(i);
				if(!pTProperty)
					continue;

				SetPropertyAction(**tAGroup,pTProperty,true);
			}
		}

		pTAGroup = m_mActionScript.Set((*tAGroup)->sName.GetData(),(*tAGroup));
		if(!pTAGroup)
			return true;

		delete pTAGroup;
		pTAGroup = NULL;

		return true;
	}

	bool CPaintManagerUI::SetActionScriptGroup( LPCTSTR pAGroupName,LPCTSTR pNotifyName,int iDefaultInterval /*= 0*/,int iDefaultTimer /*= 500*/,bool bDefaultReverse /*= false*/,bool bDefaultLoop /*= false*/,bool bDefaultAutoStart /*= true*/,bool bMergerProperty /*= false*/ )
	{
		if(!pAGroupName || !pNotifyName)
			return false;

		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return false;

		pTAGroup->sMsgType = _T("notify");
		pTAGroup->sMsgValue= pNotifyName;
		pTAGroup->iEventValue		= UIEVENT__ALL;
		pTAGroup->uDefaultInterval	= iDefaultInterval < 0?0:iDefaultInterval;
		pTAGroup->uDefaultTimer		= iDefaultTimer < (int)pTAGroup->uDefaultInterval?pTAGroup->uDefaultInterval:iDefaultTimer;
		pTAGroup->bDefaultLoop		= bDefaultLoop;
		pTAGroup->bDefaultAutoStart	= bDefaultAutoStart;
		pTAGroup->bDefaultReverse	= bDefaultReverse;

		return true;
	}

	bool CPaintManagerUI::SetActionScriptGroup( LPCTSTR pAGroupName,EVENTTYPE_UI pEventType,int iDefaultInterval /*= 0*/,int iDefaultTimer /*= 500*/,bool bDefaultReverse /*= false*/,bool bDefaultLoop /*= false*/,bool bDefaultAutoStart /*= true*/,bool bMergerProperty /*= false*/ )
	{
		if(!pAGroupName || !pEventType)
			return false;

		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return false;

		pTAGroup->sMsgType = _T("notify");
		pTAGroup->sMsgValue.Empty();
		pTAGroup->iEventValue		= pEventType;
		pTAGroup->uDefaultInterval	= iDefaultInterval < 0?0:iDefaultInterval;
		pTAGroup->uDefaultTimer		= iDefaultTimer < (int)pTAGroup->uDefaultInterval?pTAGroup->uDefaultInterval:iDefaultTimer;
		pTAGroup->bDefaultLoop		= bDefaultLoop;
		pTAGroup->bDefaultAutoStart	= bDefaultAutoStart;
		pTAGroup->bDefaultReverse	= bDefaultReverse;

		return true;
	}

	TAGroup* CPaintManagerUI::GetActionScriptGroup( LPCTSTR pAGroupName ) const
	{
		if(!pAGroupName)
			return NULL;

		return m_mActionScript.Find(pAGroupName);
	}

	bool CPaintManagerUI::RemoveActionScriptGroup( LPCTSTR pAGroupName )
	{
		if(!pAGroupName)
			return false;

		TAGroup* pTAGroup = m_mActionScript.Find(pAGroupName);
		if(!pTAGroup)
			return false;

		if(!m_mActionScript.Remove(pAGroupName))
			return false;

		delete pTAGroup;
		pTAGroup = NULL;
		return true;
	}

	void CPaintManagerUI::RemoveActionScriptGroupAll()
	{
		for(int i = 0;i < m_mActionScript.GetSize();i++){
			LPCTSTR key = m_mActionScript.GetAt(i);
			TAGroup* pTAGroup = m_mActionScript.Find(key);
			if(!pTAGroup)
				continue;

			delete pTAGroup;
			pTAGroup = NULL;
			m_mActionScript.Remove(key);
		}
		m_mActionScript.RemoveAll();
	}

	int CPaintManagerUI::HasActionScriptGroup( TAGroup& tAGroup,LPCTSTR pMsgType )
	{
		if(!pMsgType)
			return -1;

		for(int i = 0;i < m_mActionScript.GetSize();i++){
			TAGroup* pTAGroup = m_mActionScript.GetAtObj(i);
			if(!pTAGroup)
				continue;

			if(_tcscmp(pMsgType,pTAGroup->sMsgType.GetData()) == 0 && _tcscmp(tAGroup.sName.GetData(),pTAGroup->sName.GetData()) != 0)
				return 1;
		}
		return 0;
	}

	int CPaintManagerUI::HasActionScriptGroup( TAGroup& tAGroup,int pEventValue /*= 0*/ )
	{
		if(!pEventValue)
			return -1;

		for(int i = 0;i < m_mActionScript.GetSize();i++){
			TAGroup* pTAGroup = m_mActionScript.GetAtObj(i);
			if(!pTAGroup)
				continue;

			if(pTAGroup->iEventValue == pEventValue && _tcscmp(tAGroup.sName.GetData(),pTAGroup->sName.GetData()) != 0)
				return 1;
		}
		return 0;
	}

	int CPaintManagerUI::HasActionScriptGroup( LPCTSTR pAGroupName,LPCTSTR pMsgType )
	{
		if(!pMsgType)
			return -1;
		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return -1;

		return HasActionScriptGroup(*pTAGroup,pMsgType);
	}

	int CPaintManagerUI::HasActionScriptGroup( LPCTSTR pAGroupName,int pEventValue /*= 0*/ )
	{
		if(!pEventValue)
			return -1;
		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return -1;

		return HasActionScriptGroup(*pTAGroup,pEventValue);
	}

	CEventSource& CPaintManagerUI::GetEventSource()
	{
		static CEventSource m_aCustomEvents;
		return m_aCustomEvents;
	}

	CDuiTrayIcon& CPaintManagerUI::GetTrayObject()
	{
		return mDuiTray;
	}

} // namespace UiLib
