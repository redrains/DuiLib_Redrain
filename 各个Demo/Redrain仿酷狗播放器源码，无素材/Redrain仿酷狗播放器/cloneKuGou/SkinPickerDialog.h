#ifndef SKIN_PICKER_H
#define SKIN_PICKER_H


const TCHAR kSkinPickerPicutreItem[] = _T("SkinPictureItem");
const TCHAR kSkinTab[] =		_T("Skin_Tab");
const TCHAR kOfficialSkin[] =	_T("Skin_OptOfficial");
const TCHAR kMySkin[] =			_T("Skin_OptMy");
const TCHAR kBtnRestore[] =		_T("Skin_BtnRestore");
const TCHAR kBtnListTrans[] =	_T("Skin_BtnListTrans");
const TCHAR kOptSkinTrans[] =	_T("Skin_OptSkinTrans");
const TCHAR kBtnSkinTrans[] =	_T("Skin_BtnSkinTrans");
const TCHAR kBtnPattet[] =		_T("Skin_BtnPattet");
const TCHAR kBtnCustom[] =		_T("Skin_BtnCustom");

	

class CSkinPickerDialog : public CWindowWnd, public INotifyUI, public IDialogBuilderCallback
{
public:
	CSkinPickerDialog(LPCTSTR pszXMLPath, CFrameWnd *pMainWnd);
	~CSkinPickerDialog();

	LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	void OnFinalMessage(HWND /*hWnd*/);

	void Init();
	void Notify(TNotifyUI& msg);
	CControlUI* CreateControl(LPCTSTR pstrClass) ;

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
	CAnimationTabLayoutUI *m_pSkinTab;
	CButtonUI		*m_pBtnRestore;
	CButtonUI		*m_pBtnListTrans;
	COptionUI		*m_pOptSkinTrans;
	CButtonUI		*m_pBtnSkinTrans;
	CButtonUI		*m_pBtnPattet;
	CButtonUI		*m_pBtnCustom;
};

#endif SKIN_PICKER_H
