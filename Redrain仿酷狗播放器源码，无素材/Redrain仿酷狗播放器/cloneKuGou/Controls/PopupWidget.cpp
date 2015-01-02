#include "..\duilib.h"

CPopupWidget::CPopupWidget() :m_MainWndpm(NULL),m_WidgetType(UNKNOWN),m_pBtnParent(NULL)
{
}

CPopupWidget::~CPopupWidget()
{
}


LPCTSTR CPopupWidget::GetWindowClassName() const 
{ 
	return _T("PopupWdiget");
}

UINT CPopupWidget::GetClassStyle() const 
{ 
	return CS_DBLCLKS;
}

void CPopupWidget::OnFinalMessage(HWND /*hWnd*/) 
{ 
	 delete this;
}

//===================================================================
//   函数名:Init()
//   参数：pszXMLPath：xml文件的路径
//		  hWndParent:指定父窗体参数用来创建模式窗体
//		  pMainPaintManager:主窗体的m_PaintManager指针用来发通知到主窗体
//		  pBtnParent:触发创建本窗体的按钮，用来改变按钮显示的值
//		  ptPos:弹出的位置的左下角的坐标
//		  type：弹出的窗口的类型，这个类型会附带在传送消息到主窗体时，好让主窗体判断是什么类型的小挂件传来的消息
//		  pBtnParent：在改变CSliderUI的同时要改变换肤窗口的CComboUI的显示值，pBtnParent就是这个控件的指针
//===================================================================
void CPopupWidget::Init( LPCTSTR pszXMLPath, HWND hWndParent, CPaintManagerUI* pMainPaintManager, POINT ptPos, WidgetType type, CButtonUI * pBtnParent)
{  
	m_strXMLPath = pszXMLPath;
	m_MainWndpm = pMainPaintManager;
	m_WidgetType = type;
	m_pBtnParent = pBtnParent;
	Create(hWndParent, _T("PopupWdiget"), UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW);
	::ClientToScreen(hWndParent, &ptPos);
	//这里会计算应该显示的位置，好让自己出现在组合框上方
	::SetWindowPos(*this, NULL, ptPos.x, ptPos.y - m_pm.GetClientSize().cy , 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void CPopupWidget::InitWindow()
{
	m_pSlider = static_cast<CSliderUI*>(m_pm.FindControl(kWidgetSlider));

	if (m_pSlider)
	{
		if ( m_WidgetType == CHANGE_LIST_TRANS )
		{
			m_pSlider->SetMinValue(0);
			m_pSlider->SetMaxValue(255);
		//	m_pSlider->SetValue(m_pBtnParent->GetTag()* 100 / 255);
		}
		else if ( m_WidgetType == CHANGE_MAINWND_TRANS )
		{
			m_pSlider->SetMinValue(0);
			m_pSlider->SetMaxValue(150);
		//	m_pSlider->SetValue(m_pBtnParent->GetTag()* 100 / 150);
		}
		else if (m_WidgetType == CHANGE_VOLUME)
		{
			m_pSlider->SetMinValue(0);
			m_pSlider->SetMaxValue(100);
		//	m_pSlider->SetValue(m_pBtnParent->GetTag());
		}
		m_pSlider->SetValue(m_pBtnParent->GetTag());
	}
}

void CPopupWidget::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click") ) 
	{
		if( msg.pSender->GetName() == _T("closebtn")) {
			Close(IDOK);
			return; 
		}
	}
	else if ( msg.sType == _T("valuechanged") ||  msg.sType == _T("movevaluechanged"))
	{
		if ( msg.pSender == m_pSlider)
		{
			//修改皮肤设置窗体的组合框的文本显示值，把实际的值通过SetTag函数来保存
			CDuiString szValue;
			if ( m_WidgetType == CHANGE_LIST_TRANS )
			{
				szValue.SmallFormat(_T("%d%%"),m_pSlider->GetValue()* 100 / 255 );
				m_pBtnParent->SetTag(m_pSlider->GetValue());
				m_pBtnParent->SetText(szValue);
			}
			else if ( m_WidgetType == CHANGE_MAINWND_TRANS )
			{
				szValue.SmallFormat(_T("%d%%"),m_pSlider->GetValue() * 100 / 150);
				m_pBtnParent->SetTag(m_pSlider->GetValue());
				m_pBtnParent->SetText(szValue);
			}
			else if ( m_WidgetType == CHANGE_VOLUME )
			{
				//m_pBtnParent->SetTag(m_pSlider->GetValue());
			}
			//设置SetTag函数好让主窗体知道是什么类型的小挂件窗体
			msg.pSender->SetTag((UINT_PTR)m_WidgetType);
			//通过主窗体的CPaintManager指针来发送消息到主窗体
			m_MainWndpm->SendNotify(msg, false);
		}
	}

}

LRESULT CPopupWidget::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	CDialogBuilder builder;

	CControlUI *pRoot = builder.Create(m_strXMLPath.GetData(), (UINT)0,  0, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	InitWindow();
	return 0;
}

LRESULT CPopupWidget::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CPopupWidget::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CPopupWidget::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CPopupWidget::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CPopupWidget::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CPopupWidget::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return HTCLIENT;
}


LRESULT CPopupWidget::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	Close();
	return 0 ;
}

LRESULT CPopupWidget::OnMouseLeave( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	Close();
	return 0 ;
}


LRESULT CPopupWidget::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
	case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
	case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
	case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break; 
	case WM_MOUSELEAVE:	   lRes = OnMouseLeave(uMsg, wParam, lParam, bHandled); break; 
	default:
		bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

