/************************************************************************/
/* 
    Webkit浏览器控件
	不依赖于窗口

	作者：相建(QQ:848861075 快樂每一天)

*/
/************************************************************************/


#ifndef WKE_WINDOWLESS_BROWSER_H
#define WKE_WINDOWLESS_BROWSER_H
#include "wke\wke.h"
namespace UiLib
{


#pragma once
#define DUI_CTR_WKEWINDOWLESSBROWSER "WkeWindowless"
	class UILIB_API CWkeWindowlessUI : public CControlUI ,public _wkeBufHandler 
	{
	public:
		CWkeWindowlessUI(void);
		~CWkeWindowlessUI(void);

		LPCTSTR GetClass() const;

		LPVOID GetInterface(LPCTSTR pstrName);

		void DoEvent(TEventUI& event);

		void onBufUpdated (const HDC hdc,int x, int y, int cx, int cy);


		void SetPos(RECT rc);

		void PaintBkImage(HDC hDC);

		void InitBrowser(UINT nTimerID);

		void LoadUrl(LPCTSTR szUrl); 


	protected:
		wkeWebView m_webView;

	};

}

#endif

