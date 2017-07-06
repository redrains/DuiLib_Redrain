#include "stdafx.h"
#include "FrameWnd.h"

jsValue JS_CALL js_msgBox(jsExecState es)
{
	const wchar_t* text = jsToStringW(es, jsArg(es, 0));
	const wchar_t* title = jsToStringW(es, jsArg(es, 1));

	HWND hwnd = CFrameWnd::MainWnd()->GetHWND();
	if (::IsWindow(hwnd) )
	{
		::PostMessage(hwnd, WM_USER_JS_NOTIFY, (WPARAM)text, (LPARAM)title);
	}
//	MessageBox(hwnd, text, title, 0);

	return jsUndefined();
}

void onURLChanged(const struct _wkeClientHandler* clientHandler, const wkeString URL)
{
//	MessageBox(hwnd, wkeToStringW(URL), _T(""), 0);
	HWND hwnd = CFrameWnd::MainWnd()->GetHWND();
	if (::IsWindow(hwnd) )
	{
		::PostMessage(hwnd, WM_USER_URL_CHANGE, (WPARAM)wkeToStringW(URL), (LPARAM)0);
	}
}

void onTitleChanged(const struct _wkeClientHandler* clientHandler, const wkeString title)
{
//	MessageBox(hwnd, wkeToStringW(title), _T(""), 0);
	HWND hwnd = CFrameWnd::MainWnd()->GetHWND();
	if (::IsWindow(hwnd) )
	{
		::PostMessage(hwnd, WM_USER_TITLE_CHANGE, (WPARAM)wkeToStringW(title), (LPARAM)0);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////
//////////////////

CFrameWnd::CFrameWnd( LPCTSTR pszXMLPath )
	:  m_strXMLPath(pszXMLPath),
	m_pWke(NULL),
	m_pURLEdit(NULL),
	m_pLblStatusBar(NULL)
{
}

CFrameWnd * CFrameWnd::MainWnd()
{
	static CFrameWnd Frame(_T("Main_dlg.xml"));
	return &Frame;
}

LPCTSTR CFrameWnd::GetWindowClassName() const
{
	return _T("MainWnd");
}


CDuiString CFrameWnd::GetSkinFile()
{
	return m_strXMLPath;
}

CControlUI* CFrameWnd::CreateControl( LPCTSTR pstrClassName )
{
	if (_tcsicmp(pstrClassName, _T("wkeWebkit")) == 0)
		return  new CWkeWebkitUI;

	return NULL;
}

void CFrameWnd::InitWindow()
{
	CenterWindow();	
	m_pURLEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("urlEdit")));
	m_pLblStatusBar = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("statusbar")));

	m_pWke = static_cast<CWkeWebkitUI*>(m_PaintManager.FindControl(_T("ie")));
	if (m_pWke)
	{
		jsBindFunction("msgBox", js_msgBox, 2);//这里绑定js函数，让js主动调用c++函数
		static wkeClientHandler hander;        //网页标题改变和URL改变的回调
			hander.onTitleChanged = onTitleChanged;
			hander.onURLChanged = onURLChanged;
		m_pWke->SetClientHandler(&hander);
		m_pWke->SetFile(_T("Html/index.html")/*msg.pSender->GetText().GetData()*/);
		//m_pWke->SetURL(L"http://mudcu.be/sketchpad/"); //一个漂亮的网站，大家可以自己试试

	}
}



void CFrameWnd::Notify( TNotifyUI& msg )
{
	if( msg.sType == _T("click") ) 
	{	
		if( msg.pSender->GetName() == _T("btnBack") ) 
		{
			m_pWke->GoBack();
		}
		if( msg.pSender->GetName() == _T("btnforward") ) 
		{
			m_pWke->GoForward();
		}
		if( msg.pSender->GetName() == _T("btnGo") ) 
		{
			if (m_pURLEdit)
				m_pWke->SetURL(m_pURLEdit->GetText().GetData());
		}
		if( msg.pSender->GetName() == _T("btnRefresh") ) 
		{
				wstring strRet = m_pWke->RunJS(m_pURLEdit->GetText().GetData());
				MessageBox(m_hWnd, strRet.c_str(), _T(""), 0);
		}

	}
	else if( msg.sType == _T("return") ) 
	{
		if( msg.pSender->GetName() == _T("urlEdit") ) 
		{
			if (m_pURLEdit)
				m_pWke->SetURL(m_pURLEdit->GetText().GetData());
		}
	}

	__super::Notify(msg);
}

LRESULT CFrameWnd::OnTitleChanged(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	m_Title = (LPCTSTR)wParam;
	TCHAR szText[MAX_PATH] = {0};
	_stprintf(szText, _T("网站标题：%s     网站网址:%s"),m_Title.c_str(),m_URL.c_str());
	m_pLblStatusBar->SetText(szText);
	return 0 ;
}

LRESULT CFrameWnd::OnURLChanged(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	m_URL = (LPCTSTR)wParam;
	TCHAR szText[MAX_PATH] = {0};
	_stprintf(szText, _T("网站标题：%s     网站网址:%s"),m_Title.c_str(),m_URL.c_str());
	m_pLblStatusBar->SetText(szText);
	return 0 ;
}

LRESULT CFrameWnd::OnJsNotify(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	MessageBox(m_hWnd, (LPCTSTR)wParam, (LPCTSTR)lParam, 0);
	return 0;
}

LRESULT CFrameWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (uMsg)
	{
		case WM_USER_TITLE_CHANGE:
		{
			bHandled = TRUE;
			return OnTitleChanged(*this, wParam, lParam);
		}
		case WM_USER_URL_CHANGE:
		{
			bHandled = TRUE;
			return OnURLChanged(*this, wParam, lParam);
		}
		
		case WM_USER_JS_NOTIFY:
		{
			bHandled = TRUE;
			return OnJsNotify(*this, wParam, lParam);
		}     
	}
	return 0;
}