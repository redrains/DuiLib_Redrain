#ifndef SETTING_DIALOG_H
#define SETTING_DIALOG_H


const TCHAR kSetting[] = _T("SkinPictureItem");


	

class CSettingDialog : public CWindowWnd, public INotifyUI
{
public:
	CSettingDialog(LPCTSTR pszXMLPath, CFrameWnd *pMainWnd);
	~CSettingDialog();

	LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	void OnFinalMessage(HWND /*hWnd*/);

	void Init();
	void Notify(TNotifyUI& msg);

	void SetBkColor(DWORD dwBackColor);
	DWORD GetBkColor() const;
	void SetBkImage(CDuiString strBkImage);
	LPCTSTR GetBkImage();

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

protected:
	CFrameWnd		*m_pMainWnd;
	CPaintManagerUI	 m_pm;
	CDuiString		 m_strXMLPath;
	CWndShadow		 m_WndShadow;

};

#endif SETTING_DIALOG_H
