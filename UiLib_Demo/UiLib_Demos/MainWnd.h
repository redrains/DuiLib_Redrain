#ifndef MainWnd_h__
#define MainWnd_h__

#include <map>
#include <ShellAPI.h>
#include   <time.h>
#include "Utils\WinImplBase.h"


#pragma once
class CMainWnd : public WindowImplBase
{
public:
	CMainWnd(void);
	~CMainWnd(void);

public:
	CDuiString GetSkinFolder();
	CDuiString GetSkinFile();
	LPCTSTR GetWindowClassName() const ;
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void InitWindow();
	void Notify(TNotifyUI& msg);
	void OnFinalMessage(HWND hWnd);

	bool OnStartTrayAminCtrlClick(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnMsgBtnMouseEnter(TEventUI* pTEventUI,LPARAM lParam,WPARAM wParam);
	bool OnMsgBtnMouseLeave(TEventUI* pTEventUI,LPARAM lParam,WPARAM wParam);
	bool OnMsgBtnClick(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnEffectsBtnClick(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnAddNodeBtnClick(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnAddAtNodeBtnClick(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnRemoveNodeBtnClick(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnCreateChartViewBtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnAddChartDataABtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnAddChartDataBBtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnAddChartDataCBtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnLegendTopBtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnLegendRightBtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnLegendBottomBtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnDelChartDataBtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);

	void OnDuiTimerA(IDuiTimer* pTimer);
	void OnDuiTimerB(IDuiTimer* pTimer,CButtonUI* pControl);
	void OnDuiTimerC(IDuiTimer* pTimer,HWND hWnd,CMainWnd* lParam,WPARAM wParam);
	bool OnDuiTimerD(TEventUI* pTEventUI);
private:
	int				mChartDataPos;
	CLabelUI*		pTestLabel;
	CEditUI*		pTestEdit;
	CButtonUI*		pEffectsDemo;
	CChartViewUI*	pChartView;
	CHorizontalLayoutUI* pAnimWnd;
	HMENU			hMenu;

};

#endif // MainWnd_h__
