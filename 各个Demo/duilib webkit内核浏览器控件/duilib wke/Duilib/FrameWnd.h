#pragma once
#include "WkeWebkit.h"

#define WM_USER_TITLE_CHANGE         WM_USER + 289     // 浏览器标题改变
#define WM_USER_URL_CHANGE		     WM_USER + 290     // 浏览器URL改变
#define WM_USER_JS_NOTIFY		     WM_USER + 291     // js脚本函数调用c++函数

class CFrameWnd: public WindowImplBase
{
public:
	static CFrameWnd* CFrameWnd::MainWnd();
	LPCTSTR GetWindowClassName() const override;
	CDuiString GetSkinFile() override;

     void InitWindow();
     void Notify(TNotifyUI& msg);
	 CControlUI* CreateControl(LPCTSTR pstrClassName) override;
	 LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

	 LRESULT OnTitleChanged(HWND hwnd, WPARAM wParam, LPARAM lParam);
	 LRESULT OnURLChanged(HWND hwnd, WPARAM wParam, LPARAM lParam);
	 LRESULT OnJsNotify(HWND hwnd, WPARAM wParam, LPARAM lParam);
private:

	explicit CFrameWnd(LPCTSTR pszXMLPath);
	~CFrameWnd() {};

private:
	CDuiString		m_strXMLPath;

	CWkeWebkitUI	*m_pWke;
	CEditUI			*m_pURLEdit;
	CLabelUI		*m_pLblStatusBar;

	wstring			m_URL;
	wstring			m_Title;
};