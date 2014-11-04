#pragma once

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_ud.lib")
#   else
#       pragma comment(lib, "DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_u.lib")
#   else
#       pragma comment(lib, "DuiLib.lib")
#   endif
#endif

#define WM_USER_TITLE_CHANGE         WM_USER + 289     // 浏览器标题改变
#define WM_USER_URL_CHANGE		     WM_USER + 290     // 浏览器URL改变
#define WM_USER_JS_NOTIFY		     WM_USER + 291     // js脚本函数调用c++函数

class CFrameWnd: public WindowImplBase
{
public:
	explicit CFrameWnd(LPCTSTR pszXMLPath);

	static CFrameWnd * CFrameWnd::MainWnd();
	LPCTSTR GetWindowClassName() const;
	CDuiString GetSkinFile();
	CDuiString GetSkinFolder();
	//UILIB_RESOURCETYPE GetResourceType() const;
	//LPCTSTR GetResourceID() const;

     void InitWindow();
     void Notify(TNotifyUI& msg);
	 CControlUI* CreateControl(LPCTSTR pstrClassName);
	 LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	 LRESULT OnTitleChanged(HWND hwnd, WPARAM wParam, LPARAM lParam);
	 LRESULT OnURLChanged(HWND hwnd, WPARAM wParam, LPARAM lParam);
	 LRESULT OnJsNotify(HWND hwnd, WPARAM wParam, LPARAM lParam);
private:
	CDuiString		m_strXMLPath;
	CWndShadow		m_WndShadow;

	CWkeWebkitUI	*m_pWke;
	CEditUI			*m_pURLEdit;
	CLabelUI		*m_pLblStatusBar;

	wstring			m_URL;
	wstring			m_Title;
};