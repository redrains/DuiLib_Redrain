#pragma once
#include "Controls/AnimLayout.h"

class CFrameWnd: public WindowImplBase
{
public:
	explicit CFrameWnd(LPCTSTR pszXMLPath);
	~CFrameWnd();

	LPCTSTR GetWindowClassName() const override;
	CDuiString GetSkinFile() override;

	void InitWindow();
	void OnPrepare(TNotifyUI& msg);
	void Notify(TNotifyUI& msg);
	CControlUI* CreateControl(LPCTSTR pstrClassName) override;

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;


private:
	CDuiString		m_strXMLPath;
	AnimLayout*		m_pAnimLayout;
};