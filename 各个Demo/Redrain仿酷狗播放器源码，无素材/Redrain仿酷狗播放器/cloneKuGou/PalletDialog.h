#ifndef PALLET_DIALOG_H
#define PALLET_DIALOG_H
#include "SkinPickerDialog.h"

const TCHAR kPalletPallet[] = _T("Pallet");
const TCHAR kPalletOk[] =		_T("Pallet_BTN_OK");
const TCHAR kPalletCancel[] =	_T("Pallet_BTN_CANCEL");
	

class CPalletDialog : public CWindowWnd, public INotifyUI, public IDialogBuilderCallback
{
public:
	CPalletDialog(LPCTSTR pszXMLPath, CSkinPickerDialog *pSkinPickerWnd);
	~CPalletDialog();

	LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	void OnFinalMessage(HWND /*hWnd*/);
	DWORD GetSelectColor() ;

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
	CSkinPickerDialog*m_pSkinPickerWnd;
	CPaintManagerUI	 m_pm;
	CDuiString		 m_strXMLPath;
	CWndShadow		 m_WndShadow;
	CColorPaletteUI	*m_pPallet;
	CButtonUI		*m_pBtnOk;
	CButtonUI		*m_pBtnCancel;
	DWORD			m_SelectedColor;
};

#endif PALLET_DIALOG_H
