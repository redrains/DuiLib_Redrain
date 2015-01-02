//====================================================
//   CPopupWidget用来做仿酷狗程序的自动销毁的弹出控件的部分，
//   比如在原酷狗里面，鼠标移动到音量改变按钮时会弹出个小窗口来改变音量
//   在换肤窗口里改变播放列表透明度时也用到弹出的小窗口
//   大家可以根据需求来修改这里的参数和代码
//	 BY：Redrain   QQ：491646717  2014.8.7
//==================================================== 

#pragma once

const TCHAR kWidgetSlider[] =	 _T("Widget_slider");

enum WidgetType				 //选择小挂件的类型
{
	CHANGE_LIST_TRANS,		//改变主窗体的播放列表都名都
	CHANGE_MAINWND_TRANS,	//改变主窗体皮肤的透明度
	CHANGE_VOLUME,			//改变主窗体的音量，这个是给以后用的	
	UNKNOWN,
};


class CPopupWidget:  public CWindowWnd, public INotifyUI
{
public:
    explicit CPopupWidget();

protected:
    virtual ~CPopupWidget();  

public:
	LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	void OnFinalMessage(HWND /*hWnd*/);

	void InitWindow();
	void Init(LPCTSTR pszXMLPath, HWND hWndParent, CPaintManagerUI* pMainPaintManager, POINT ptPos, WidgetType type, CButtonUI * pBtnParent );
	void Notify(TNotifyUI& msg);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
   
protected:
	CDuiString		m_strXMLPath;
	CPaintManagerUI *m_MainWndpm;
	CPaintManagerUI m_pm;
	CButtonUI		*m_pBtnParent;
	CSliderUI		*m_pSlider;
	WidgetType		m_WidgetType;
};
