#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H
class CMessageDialog : public CWindowWnd, public INotifyUI
{
public:
	CMessageDialog();
	~CMessageDialog();

	LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	void OnFinalMessage(HWND /*hWnd*/);

	void SetBkColor(DWORD dwBackColor);
	DWORD GetBkColor() const;
	void SetBkImage(CDuiString strBkImage);
	LPCTSTR GetBkImage();

	void Init();
	void Notify(TNotifyUI& msg);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	UINT DuiMessageBox(CFrameWnd *pMainWnd, LPCTSTR lpText, LPCTSTR lpCaption = _T("ÌבÊ¾"),UINT uType = MB_OK, UINT uIcon = MB_ICONWARNING);
	void DuiAboutBox(CFrameWnd *m_pMainWnd, LPCTSTR lpXMLPath = _T("xmls\\about.xml"));

protected:
	CPaintManagerUI	 m_pm;
	CDuiString		 m_strXMLPath;
	CWndShadow		 m_WndShadow;
	CButtonUI		*m_pButtonOK;
	CButtonUI		*m_pButtonCancel;
	CFrameWnd		*m_pMainWnd;

};

#endif MESSAGEBOX_H