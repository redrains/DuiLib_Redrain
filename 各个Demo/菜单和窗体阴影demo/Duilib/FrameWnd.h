#pragma once

class CFrameWnd: public WindowImplBase
{
public:
	explicit CFrameWnd(LPCTSTR pszXMLPath);

	LPCTSTR GetWindowClassName() const override;
	CDuiString GetSkinFile() override;

     void InitWindow();
     void Notify(TNotifyUI& msg);
	 LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

private:
	CDuiString		m_strXMLPath;

	map<CDuiString,bool> m_MenuCheckInfo; //保存菜单的单选复选信息
};