#ifndef IWindowBase_h__
#define IWindowBase_h__

#pragma once
#include "WinImplBase.h"

using namespace UiLib;

class UILIB_API IWindowBase 
	: public CWindowWnd
	, public CNotifyPump
	, public INotifyUI
	, public IMessageFilterUI
	, public IDialogBuilderCallback
{
public:
	IWindowBase(LPCTSTR _ZipSkin = NULL);
	virtual ~IWindowBase();

	virtual void OnFinalMessage(HWND hWnd);

	virtual void Init();

	virtual LPCTSTR GetWindowClassName() const;

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass, CPaintManagerUI* pManager);

	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);   
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	virtual void Notify(TNotifyUI& msg);

public:
	CPaintManagerUI* GetPaintManager();
#pragma region 窗口关闭按钮
	CButtonUI*		mpCloseBtn;
	CButtonUI*		mpMaxBtn;
	CButtonUI*		mpRestoreBtn;
	CButtonUI*		mpMinBtn;
#pragma endregion 窗口关闭按钮

protected:
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile() = 0;
	virtual UINT GetClassStyle() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
protected:
	CDuiString		ZipSkinPath;
	CPaintManagerUI pm;
	LPBYTE m_lpResourceZIPBuffer;
};
#endif // IWindowBase_h__
