#pragma once
#include "../wke/wke.h"
namespace UiLib {
const char TM_TICKER = 1;

class CWkeWebkitUI;
class CWkeWebkitWnd : public CWindowWnd ,protected wkeBufHandler 
{
	friend class CWkeWebkitUI;
public:
	CWkeWebkitWnd();
	~CWkeWebkitWnd();

	void Init(CWkeWebkitUI* pOwner);
	wkeWebView	GetWebView();
	LPCTSTR GetWindowClassName() const;
	void OnFinalMessage(HWND hWnd);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseEvent(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled); 
	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled); 
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyUp(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled);
	LRESULT OnChar(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled);
	LRESULT OnImeStartComposition(UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	virtual void onBufUpdated (const HDC hdc,int x, int y, int cx, int cy);

protected:
	CWkeWebkitUI* m_pOwner;
	wkeWebView	 m_pWebView;
	wstring		 m_strUrl;	
	bool		 m_bInit;

};
///////////////////////////////////////////  
//网页加载状态改变的回调  
class UILIB_API CWkeWebkitLoadCallback  
{  
public:  
    virtual void    OnLoadFailed()=0;  
    virtual void    OnLoadComplete()=0;  
    virtual void    OnDocumentReady()=0;  
};  

class UILIB_API CWkeWebkitUI :public CControlUI
{
	friend class CWkeWebkitWnd;
public:
	CWkeWebkitUI(void);
	~CWkeWebkitUI(void);

	static void WkeWebkit_Init();
	static void WkeWebkit_Shutdown();
	
	LPCTSTR	GetClass() const;
	LPVOID	GetInterface(LPCTSTR pstrName);

	void	SetVisible(bool bVisible);
	void	SetInternVisible(bool bVisible = true);
	void	DoInit();
	void	SetPos(RECT rc);
	void	DoEvent(TEventUI& event);	
	void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	wkeWebView  GetWebView(); 
	void	SetURL( wstring strValue);
	void	SetFile(wstring strValue);
	wstring RunJS(wstring strValue);
	void	SetClientHandler(const wkeClientHandler* handler);
	void    SetLoadCallback( CWkeWebkitLoadCallback* pCallback ) ;
	CWkeWebkitLoadCallback* GetLoadCallback()  ;

	void	GoBack();
	void	GoForward();

	
    void    StartCheckThread();  
    void    StopCheckThread(); 

protected:
	CWkeWebkitWnd *m_pWindow;
	CWkeWebkitLoadCallback*     m_pLoadCallback;
	HANDLE      m_hCheckThread;
};

}