#include "StdAfx.h"
#include "MainWnd.h"

#define IDM_SHOW_LOG		WM_USER+2200
#define IDM_EXIT_APP		WM_USER+2300




CMainWnd::CMainWnd(void)
{
	mChartDataPos = 0;
} 


CMainWnd::~CMainWnd(void)
{
}

//************************************
// Method:    GetSkinFile
// FullName:  CMainWnd::GetSkinFile
// Access:    public 
// Returns:   UiLib::CDuiString
// Qualifier:
// Note:	  
//************************************
UiLib::CDuiString CMainWnd::GetSkinFile()
{
	try
	{
		return CDuiString(_T("MainSkin.xml"));
	}
	catch (...)
	{
		throw "CMainWnd::GetSkinFile";
	}
}

UiLib::CDuiString CMainWnd::GetSkinFolder()
{
	return _T("");
}
//************************************
// Method:    GetWindowClassName
// FullName:  CMainWnd::GetWindowClassName
// Access:    public 
// Returns:   LPCTSTR
// Qualifier: const
// Note:	  
//************************************
LPCTSTR CMainWnd::GetWindowClassName() const
{
	try
	{
		return _T("UiLib_Demos");
	}
	catch (...)
	{
		throw "CMainWnd::GetWindowClassName";
	}
}

//************************************
// Method:    HandleMessage
// FullName:  CMainWnd::HandleMessage
// Access:    public 
// Returns:   LRESULT
// Qualifier:
// Parameter: UINT uMsg
// Parameter: WPARAM wParam
// Parameter: LPARAM lParam
// Note:	  
//************************************
LRESULT CMainWnd::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	try
	{
		if(lParam == WM_RBUTTONUP)
		{
			::SetForegroundWindow(this->m_hWnd);

			hMenu = CreatePopupMenu();
			AppendMenu(hMenu,MF_STRING,IDM_SHOW_LOG,_T("显示日志"));
			AppendMenu(hMenu,MF_STRING,IDM_EXIT_APP,_T("退出程序"));

			POINT point;
			point.x=LOWORD(lParam);
			point.y=HIWORD(lParam);
			GetCursorPos(&point);
			TrackPopupMenu(hMenu,TPM_RIGHTALIGN,point.x,point.y,0,m_hWnd,NULL);

			::PostMessage(m_hWnd,WM_NULL,0,0);

			return TRUE;
		}
		else if(wParam == IDM_SHOW_LOG || lParam == WM_LBUTTONDBLCLK)
		{
			this->ShowWindow();
			return TRUE;
		}
		else if(wParam == IDM_EXIT_APP)
		{
			Close(IDOK);
			return TRUE;
		}
		return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
	}
	catch (...)
	{
		throw "CMainWnd::HandleMessage";
	}
}

void CMainWnd::InitWindow()
{
	try
	{
		WindowImplBase::InitWindow();
		
		pAnimWnd		= static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("AnimWnd")));
		pTestLabel		= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("TestLabel")));
		pChartView		= static_cast<CChartViewUI*>(m_PaintManager.FindControl(_T("ChartView")));
		pEffectsDemo	= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("EffectsDemo")));
		
		CButtonUI*	pStartTrayAminCtrl	= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("StartTrayAminCtrl")));

		CButtonUI*	pTextMsgBtn			= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("TextMsg")));
		CButtonUI*	pAddNodeBtn			= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("AddNode")));
		CButtonUI*	pAddAtNodeBtn		= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("AddAtNode")));
		CButtonUI*	pCreateChartViewBtn	= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("CreateChartView")));
		CButtonUI*	pAddChartDataABtn	= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("AddChartDataA")));
		CButtonUI*	pAddChartDataBBtn	= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("AddChartDataB")));
		CButtonUI*	pAddChartDataCBtn	= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("AddChartDataC")));
		CButtonUI*	pLegendTopCBtn		= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("LegendTop")));
		CButtonUI*	pLegendRightBtn		= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("LegendRight")));
		CButtonUI*	pLegendBottomBtn	= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("LegendBottom")));
		CButtonUI*	pDelChartDataBtn	= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("DelChartData")));

		pStartTrayAminCtrl->OnNotify	+= MakeDelegate(this,&CMainWnd::OnStartTrayAminCtrlClick,_T("click"));
		pEffectsDemo->OnNotify			+= MakeDelegate(this,&CMainWnd::OnEffectsBtnClick,_T("click"));
		pTextMsgBtn->OnNotify			+= MakeDelegate(this,&CMainWnd::OnMsgBtnClick,_T("click"));
		pTextMsgBtn->OnEvent			+= MakeDelegate(this,&CMainWnd::OnMsgBtnMouseEnter,UIEVENT_MOUSEENTER);
		pTextMsgBtn->OnEvent			+= MakeDelegate(this,&CMainWnd::OnMsgBtnMouseLeave,UIEVENT_MOUSELEAVE);
		pAddNodeBtn->OnNotify			+= MakeDelegate(this,&CMainWnd::OnAddNodeBtnClick,_T("click"));
		pAddAtNodeBtn->OnNotify			+= MakeDelegate(this,&CMainWnd::OnAddNodeBtnClick,_T("click"));
		pCreateChartViewBtn->OnNotify	+= MakeDelegate(this,&CMainWnd::OnCreateChartViewBtn,_T("click"));
		pAddChartDataABtn->OnNotify		+= MakeDelegate(this,&CMainWnd::OnAddChartDataABtn,_T("click"));
		pAddChartDataBBtn->OnNotify		+= MakeDelegate(this,&CMainWnd::OnAddChartDataBBtn,_T("click"));
		pAddChartDataCBtn->OnNotify		+= MakeDelegate(this,&CMainWnd::OnAddChartDataCBtn,_T("click"));
		pLegendTopCBtn->OnNotify		+= MakeDelegate(this,&CMainWnd::OnLegendTopBtn,_T("click"));
		pLegendRightBtn->OnNotify		+= MakeDelegate(this,&CMainWnd::OnLegendRightBtn,_T("click"));
		pLegendBottomBtn->OnNotify		+= MakeDelegate(this,&CMainWnd::OnLegendBottomBtn,_T("click"));
		pDelChartDataBtn->OnNotify		+= MakeDelegate(this,&CMainWnd::OnDelChartDataBtn,_T("click"));

		//pEffectsDemo 为Duilib控件指针

		//IDuiTimer* pControlTimer = NULL;
		//定义OnTimer的绑定函数为：void OnDuiTimerA(IDuiTimer* pTimer);
		//无参数调用定时器,并将OnTimer函数绑定为CMainWnd::OnDuiTimerA函数，触发间隔为1000毫秒，100000毫秒后自动停止
		//pEffectsDemo->OnTimers			+=	pControlTimer = MakeDuiTimer(this,&CMainWnd::OnDuiTimerA,1000,100000);
		//pControlTimer->KillDuiTimer();

		//定义OnTimer的绑定函数为：void OnDuiTimerB(IDuiTimer* pTimer,CButtonUI* pControl);
		//有参数调用定时器,并将OnTimer函数绑定为CMainWnd::OnDuiTimerA函数，且传入一个Button类型的指针，间隔为1000毫秒，100000毫秒后自动停止
		//pEffectsDemo->OnTimers			+=	pControlTimer = MakeDuiTimer(this,&CMainWnd::OnDuiTimerB,pEffectsDemo,1000,100000);
		//pControlTimer->KillDuiTimer();

		//定义OnTimer的绑定函数为：void OnDuiTimerC(IDuiTimer* pTimer,HWND hWnd,CMainWnd* lParam,WPARAM wParam);
		//有句柄调用定时器，并将OnTimer函数绑定为CMainWnd::OnDuiTimerC函数，lParam 值为this，wParam值为1234，触发间隔为1000毫秒,并一直持续
		//并可对LPARAM WPARAM 的赋值，若第2个参数为空时，会触发WM_TIMER消息，否则调用第二个参数传入需要绑定的OnTimer方法
		//pEffectsDemo->OnTimers			+=	pControlTimer = MakeDuiTimer(this,&CMainWnd::OnDuiTimerC,m_hWnd,this,1234,1000,NULL);
		//pControlTimer->KillDuiTimer();

		//停止pEffectsDemo控件的所有定时器
		//pEffectsDemo->OnTimers.KillTimers();
		//开启pEffectsDemo控件的所有定时器
		//pEffectsDemo->OnTimers.SetTimers();
		//停止并删除pEffectsDemo控件的所有定时器
		//pEffectsDemo->OnTimers.RemoveTimerAll();

		//无控件下使用定时器
		//IDuiTimer* pTimer = MakeDuiTimer(this,&CMainWnd::OnDuiTimerC,m_hWnd,this,1234,1000,NULL);
		//pTimer->KillDuiTimer();
		//delete pTimer;

		//构造函数方式调用 
 		//CDuiTimer* mTimer = new CDuiTimer();
 		//mTimer->SetDuiTimer(m_hWnd,(LPARAM)this,1234,1000,10000);

		//通过MakeDelegate方式绑定WM_TIMER消息到指定方法
 		//pEffectsDemo->OnNotify			+= MakeDelegate(this,&CMainWnd::OnDuiTimerD,(EVENTTYPE_UI)WM_TIMER);

		//CControlUI* pControl = CreateDuiInstance<CControlUI*>("CControlUI");
		//pControl->SetText(_T("tset"));
		//CDuiString texx = pControl->GetText();

	}
	catch (...)
	{
		throw "CMainWnd::Init";
	}
}

//************************************
// 函数名称: Notify
// 返回类型: void
// 参数信息: TNotifyUI & msg
// 函数说明: 
//************************************
void CMainWnd::Notify( TNotifyUI& msg )
{
	WindowImplBase::Notify(msg);
}

//************************************
// Method:    OnFinalMessage
// FullName:  CMainWnd::OnFinalMessage
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: HWND hWnd
// Note:	  
//************************************
void CMainWnd::OnFinalMessage( HWND hWnd )
{
	try
	{
		WindowImplBase::OnFinalMessage(hWnd);
		PostQuitMessage(0);
		delete this;
	}
	catch (...)
	{
		throw "CMainWnd::OnFinalMessage";
	}
}
//************************************
// 函数名称: OnMsgBtnClick
// 返回类型: bool
// 参数信息: TNotifyUI * pTNotifyUI
// 函数说明: 
//************************************
bool CMainWnd::OnMsgBtnClick( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	MessageBox(m_hWnd,_T("我是绑定的按钮点击消息 OK!"),_T("消息路由"),MB_OK);
	return true;
}

//************************************
// 函数名称: OnStartTrayAminCtrlClick
// 返回类型: bool
// 参数信息: TNotifyUI * pTNotifyUI
// 参数信息: LPARAM lParam
// 参数信息: WPARAM wParam
// 函数说明: 
//************************************
bool CMainWnd::OnStartTrayAminCtrlClick( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	if(m_PaintManager.GetTrayObject().IsTwinkling())
		m_PaintManager.GetTrayObject().StopTwinkling();
	else
		m_PaintManager.GetTrayObject().StartTwinkling();

	return true;
}

//************************************
// 函数名称: OnMsgBtnMouseEnter
// 返回类型: bool
// 参数信息: TEventUI * pTEventUI
// 函数说明: 
//************************************
bool CMainWnd::OnMsgBtnMouseEnter( TEventUI* pTEventUI,LPARAM lParam,WPARAM wParam )
{
	DUI__Trace(_T("我是鼠标进入按钮时的消息 OK!"));
	pTEventUI->pSender->SetText(_T("鼠标进来了"));
	pTEventUI->pSender->SetUserData(_T("鼠标进来了"));

	//pEffectsDemo->OnTimers			+= MakeDuiTimer(this,&CMainWnd::OnDuiTimerB,(CButtonUI*)pTEventUI->pSender,1000,NULL,true,true);
	return true;
}

//************************************
// 函数名称: OnMsgBtnMouseLeave
// 返回类型: bool
// 参数信息: TEventUI * pTEventUI
// 函数说明: 
//************************************
bool CMainWnd::OnMsgBtnMouseLeave( TEventUI* pTEventUI,LPARAM lParam,WPARAM wParam )
{
	DUI__Trace(_T("我是鼠标离开按钮时的消息 OK!"));
	pTEventUI->pSender->SetText(_T("鼠标跑了"));
	pTEventUI->pSender->SetUserData(_T("鼠标跑了"));

	//pEffectsDemo->OnTimers			+= MakeDuiTimer(this,&CMainWnd::OnDuiTimerB,(CButtonUI*)pTEventUI->pSender,1000,NULL,true,true);
	return true;
}

//************************************
// 函数名称: OnEffectsBtnClick
// 返回类型: bool
// 参数信息: TNotifyUI * pTNotifyUI
// 函数说明: 
//************************************
bool CMainWnd::OnEffectsBtnClick( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	pAnimWnd->SetAnimEffects(true);
	pEffectsDemo->SetTag(pEffectsDemo->GetTag()+1);

	m_PaintManager.SetCurStyles(pEffectsDemo->GetTag()%2?_T("LangChinese"):_T("LangEnglish"));

	if(pEffectsDemo->GetTag()%2)
		m_PaintManager.SetCurStyles(_T("LangChinese"));
	else 
		m_PaintManager.SetCurStyles(_T("LangEnglish"));
		

	if(pEffectsDemo->GetTag() == 1)
		pAnimWnd->SetAttribute(_T("adveffects"),_T("anim='left2right' fillingbk='#FFFF0000' offset='180'"));
	else if(pTNotifyUI->pSender->GetTag() == 2)
		pAnimWnd->SetAttribute(_T("adveffects"),_T("anim='right2left' fillingbk='#FFFF0000' offset='180'"));
	else if(pEffectsDemo->GetTag() == 3)
		pAnimWnd->SetAttribute(_T("adveffects"),_T("anim='top2bottom' fillingbk='#FFFF0000' offset='180'"));
	else if(pEffectsDemo->GetTag() == 4)
		pAnimWnd->SetAttribute(_T("adveffects"),_T("anim='bottom2top' fillingbk='#FFFF0000' offset='180'"));
	else if(pEffectsDemo->GetTag() == 5)
		pAnimWnd->SetAttribute(_T("adveffects"),_T("anim='zoom+' offset='180'"));
	else if(pEffectsDemo->GetTag() == 6)
		pAnimWnd->SetAttribute(_T("adveffects"),_T("anim='zoom-' offset='180'"));
	else if(pEffectsDemo->GetTag() == 7)
		pAnimWnd->SetAttribute(_T("adveffects"),_T("offsetx='180' rotation='0.3'"));
	else if(pEffectsDemo->GetTag() == 8)
		pAnimWnd->SetAttribute(_T("adveffects"),_T("offsetx='180' rotation='-0.3'"));
	else if(pEffectsDemo->GetTag() == 9)
		pAnimWnd->SetAttribute(_T("adveffects"),_T("offsety='180' rotation='0.3'"));
	else if(pEffectsDemo->GetTag() == 10)
		pAnimWnd->SetAttribute(_T("adveffects"),_T("offsety='180' rotation='-0.3'"));
	else
	{
		pEffectsDemo->SetTag(1);
		pAnimWnd->SetAttribute(_T("adveffects"),_T("anim='left2right' offset='80'"));
	}
	pAnimWnd->TriggerEffects();

	return true;
}

//************************************
// 函数名称: OnAddNodeBtnClick
// 返回类型: bool
// 参数信息: TNotifyUI * pTNotifyUI
// 函数说明: 
//************************************
bool CMainWnd::OnAddNodeBtnClick( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	CEditUI* pEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("AddNodeText")));
	if(!pEdit && pEdit->GetText().GetLength() > 0)
		return true;

	CTreeNodeUI* pParentNode = static_cast<CTreeNodeUI*>(pTNotifyUI->pSender->GetParent()->GetParent());
	if(!pParentNode || !pParentNode->GetInterface(_T("TreeNode")))
		return true;

	CFadeButtonUI* pRemoveBtn = new CFadeButtonUI();
	pRemoveBtn->SetText(_T("删除节点"));
	pRemoveBtn->SetName(_T("RemoveNodeBtn"));

	CTreeNodeUI* pNewNode = new CTreeNodeUI();
	pNewNode->SetItemText(pEdit->GetText().GetData());
	pParentNode->Add(pNewNode);
	pNewNode->GetTreeNodeHoriznotal()->Add(pRemoveBtn);

	pNewNode->SetStyleName(_T("treenode"));
	pRemoveBtn->SetStyleName(_T("FadeButtonDemo"),&m_PaintManager);
	pRemoveBtn->SetFixedWidth(60);

	pRemoveBtn->OnNotify		+= MakeDelegate(this,&CMainWnd::OnRemoveNodeBtnClick,_T("click"));

	return true;
}

//************************************
// 函数名称: OnAddAtNodeBtnClick
// 返回类型: bool
// 参数信息: TNotifyUI * pTNotifyUI
// 函数说明: 
//************************************
bool CMainWnd::OnAddAtNodeBtnClick( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	CTreeNodeUI* pParentNode = static_cast<CTreeNodeUI*>(pTNotifyUI->pSender->GetParent()->GetParent());
	if(!pParentNode || !pParentNode->GetInterface(_T("TreeNode")))
		return true;

	CEditUI* pEdit = static_cast<CEditUI*>(pParentNode->GetTreeNodeHoriznotal()->FindSubControl(_T("AddAtNodeText")));
	if(!pEdit && pEdit->GetText().GetLength() > 0)
		return true;

	CFadeButtonUI* pRemoveBtn = new CFadeButtonUI();
	pRemoveBtn->SetText(_T("删"));
	pRemoveBtn->SetName(_T("RemoveNodeBtn"));

	CFadeButtonUI* pAddAtBtn = new CFadeButtonUI();
	pAddAtBtn->SetText(_T("插入节点"));
	pAddAtBtn->SetName(_T("AddAtNode"));

	CEditUI* pAddEdit = new CEditUI();
	pAddEdit->SetName(_T("AddAtNodeText"));
	pAddEdit->SetText(_T("节点名称"));
	pAddEdit->SetTipValue(_T("节点名称"));

	CTreeNodeUI* pNewNode = new CTreeNodeUI();
	pNewNode->SetItemText(pEdit->GetText().GetData());
	pParentNode->AddAt(pNewNode,0);
	pNewNode->GetTreeNodeHoriznotal()->Add(pAddEdit);
	pNewNode->GetTreeNodeHoriznotal()->Add(pAddAtBtn);
	pNewNode->GetTreeNodeHoriznotal()->Add(pRemoveBtn);

	pNewNode->SetStyleName(_T("treenode"));
	pAddAtBtn->SetStyleName(_T("FadeButtonDemo"),&m_PaintManager);
	pRemoveBtn->SetStyleName(_T("FadeButtonDemo"),&m_PaintManager);
	pAddEdit->SetFixedWidth(50);
	pAddAtBtn->SetFixedWidth(60);
	pRemoveBtn->SetFixedWidth(20);


	pRemoveBtn->OnNotify		+= MakeDelegate(this,&CMainWnd::OnRemoveNodeBtnClick,_T("click"));

	return true;
}

//************************************
// 函数名称: OnRemoveNodeBtnClick
// 返回类型: bool
// 参数信息: TNotifyUI * pTNotifyUI
// 函数说明: 
//************************************
bool CMainWnd::OnRemoveNodeBtnClick( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	CTreeNodeUI* pParentNode = static_cast<CTreeNodeUI*>(pTNotifyUI->pSender->GetParent()->GetParent());
	if(!pParentNode || !pParentNode->GetInterface(_T("TreeNode")))
		return true;

	if(pParentNode->GetParentNode())
		pParentNode->GetParentNode()->Remove(pParentNode);

	return true;
}

//************************************
// 函数名称: OnCreateChartViewBtn
// 返回类型: bool
// 参数信息: TNotifyUI * pTNotifyUI
// 函数说明: 
//************************************
bool CMainWnd::OnCreateChartViewBtn( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	//pChartView->GetXYAxis().SetTickLimis(-50,100,20);
	pChartView->AddLabel(_T("一月"));
	pChartView->AddLabel(_T("二月"));
	pChartView->AddLabel(_T("三月"));
	pChartView->AddLabel(_T("四月"));

	return true;
}

//************************************
// 函数名称: OnAddChartDataABtn
// 返回类型: bool
// 参数信息: TNotifyUI * pTNotifyUI
// 函数说明: 
//************************************
bool CMainWnd::OnAddChartDataABtn( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	using namespace Gdiplus;
	if(!pChartView->GetGroupCount())
		return true;

	CDuiString mLegend;
	mLegend.Format(_T("测试图例%d"),mChartDataPos++);
	CChartSeries* pSeriesA = new CChartSeries(mLegend,Color(rand()%256,rand()%256,rand()%256).GetValue(),Color(rand()%256,rand()%256,rand()%256).GetValue());

	for(int nIndex = 0;(UINT)nIndex < pChartView->GetGroupCount();nIndex++){
		pSeriesA->AddSeriesData(rand()%100-50);
	}
	pChartView->AddSeries(pSeriesA);

	return true;
}

//************************************
// 函数名称: OnAddChartDataBBtn
// 返回类型: bool
// 参数信息: TNotifyUI * pTNotifyUI
// 函数说明: 
//************************************
bool CMainWnd::OnAddChartDataBBtn( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	using namespace Gdiplus;
	if(!pChartView->GetGroupCount())
		return true;

	CDuiString mLegend;
	mLegend.Format(_T("测试图例%d"),mChartDataPos++);
	CChartSeries* pSeriesA = new CChartSeries(mLegend,Color(rand()%256,rand()%256,rand()%256).GetValue(),Color(rand()%256,rand()%256,rand()%256).GetValue());

	for(int nIndex = 0;(UINT)nIndex < pChartView->GetGroupCount();nIndex++){
		pSeriesA->AddSeriesData(rand()%151);
	}
	pChartView->AddSeries(pSeriesA);

	return true;
}

//************************************
// 函数名称: OnAddChartDataCBtn
// 返回类型: bool
// 参数信息: TNotifyUI * pTNotifyUI
// 函数说明: 
//************************************
bool CMainWnd::OnAddChartDataCBtn( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	using namespace Gdiplus;
	if(!pChartView->GetGroupCount())
		return true;

	CDuiString mLegend;
	mLegend.Format(_T("测试图例%d"),mChartDataPos++);
	CChartSeries* pSeriesA = new CChartSeries(mLegend,Color(rand()%256,rand()%256,rand()%256).GetValue(),Color(rand()%256,rand()%256,rand()%256).GetValue());

	for(int nIndex = 0;(UINT)nIndex < pChartView->GetGroupCount();nIndex++){
		pSeriesA->AddSeriesData(rand()%151-151);
	}
	pChartView->AddSeries(pSeriesA);

	return true;
}

//************************************
// 函数名称: OnLegendTopBtn
// 返回类型: bool
// 参数信息: TNotifyUI * pTNotifyUI
// 函数说明: 
//************************************
bool CMainWnd::OnLegendTopBtn( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	pChartView->GetXYAxis().SetLegendLocation(LOCATION_TOP);
	return true;
}

//************************************
// 函数名称: OnLegendRightBtn
// 返回类型: bool
// 参数信息: TNotifyUI * pTNotifyUI
// 函数说明: 
//************************************
bool CMainWnd::OnLegendRightBtn( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	pChartView->GetXYAxis().SetLegendLocation(LOCATION_RIGHT);
	return true;
}

//************************************
// 函数名称: OnLegendBottomBtn
// 返回类型: bool
// 参数信息: TNotifyUI * pTNotifyUI
// 函数说明: 
//************************************
bool CMainWnd::OnLegendBottomBtn( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	pChartView->GetXYAxis().SetLegendLocation(LOCATION_BOTTOM);
	return true;
}

//************************************
// 函数名称: OnDelChartDataBtn
// 返回类型: bool
// 参数信息: TNotifyUI * pTNotifyUI
// 函数说明: 
//************************************
bool CMainWnd::OnDelChartDataBtn( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	pChartView->RemoveSeries(0);
	return true;
}

//************************************
// 函数名称: OnDuiTimerA
// 返回类型: void
// 参数信息: IDuiTimer * pTimer
// 函数说明: 
//************************************
void CMainWnd::OnDuiTimerA( IDuiTimer* pTimer )
{
	DUITRACE(_T("无参数定时器来了:Interval=%d CurTimer=%d TotalTimer=%d"),pTimer->GetInterval(),pTimer->GetCurTimer(),pTimer->GetTotalTimer());
}

//************************************
// 函数名称: OnDuiTimerB
// 返回类型: void
// 参数信息: IDuiTimer * pTimer
// 参数信息: CButtonUI * pControl
// 函数说明: 
//************************************
void CMainWnd::OnDuiTimerB( IDuiTimer* pTimer,CButtonUI* pControl )
{
	CDuiString mText;
	mText.SmallFormat(_T("%s %d 秒"),pControl->GetUserData().GetData(),(int)(pTimer->GetCurTimer()/1000)+1);
	DUITRACE(_T(""));
	pControl->SetText(mText);
	DUITRACE(_T("有参数定时器来了:Interval=%d CurTimer=%d TotalTimer=%d"),pTimer->GetInterval(),pTimer->GetCurTimer(),pTimer->GetTotalTimer());
}

//************************************
// 函数名称: OnDuiTimerC
// 返回类型: void
// 参数信息: IDuiTimer * pTimer
// 参数信息: HWND hWnd
// 参数信息: LPARAM lParam
// 参数信息: WPARAM wParam
// 函数说明: 
//************************************
void CMainWnd::OnDuiTimerC( IDuiTimer* pTimer,HWND hWnd,CMainWnd* lParam,WPARAM wParam )
{
	DUITRACE(_T("有句柄定时器来了:hwnd=%d lparam=%d wparam=%d Interval=%d CurTimer=%d TotalTimer=%d"),pTimer->GetHwnd(),pTimer->GetLParam(),pTimer->GetWParam(),pTimer->GetInterval(),pTimer->GetCurTimer(),pTimer->GetTotalTimer());
}

//************************************
// 函数名称: OnDuiTimerD
// 返回类型: bool
// 参数信息: TEventUI * pTEventUI
// 函数说明：#end$
//************************************
bool CMainWnd::OnDuiTimerD( TEventUI* pTEventUI )
{
	DUITRACE(_T("定时器事件方式来了:lparam=%d wparam=%d"),pTEventUI->lParam,pTEventUI->wParam);
	return true;
}
