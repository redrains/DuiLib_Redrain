#ifndef _CCUSTOM_WEBBROWSER_EVENT_HANDLER_H_
#define _CCUSTOM_WEBBROWSER_EVENT_HANDLER_H_

#pragma once
class CCustomWebEventHandler:public CWebBrowserEventHandler
{
public:
	CCustomWebEventHandler() {}
	~CCustomWebEventHandler() {}

	virtual void BeforeNavigate2( IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel ) {}
	virtual void NavigateError(IDispatch *pDisp,VARIANT * &url,VARIANT *&TargetFrameName,VARIANT *&StatusCode,VARIANT_BOOL *&Cancel) {}
	virtual void NavigateComplete2(IDispatch *pDisp,VARIANT *&url)
	{
#pragma region MyRegion

		HRESULT hr;
		// 获得Html文档指针
		IHTMLDocument2 *pDocument = NULL;
		hr = pDisp->QueryInterface( IID_IHTMLDocument2, (void**)&pDocument );

		if (pDocument)
		{
			IHTMLElement *pBody = NULL;
			hr = pDocument->get_body( &pBody );
			if (pBody)
			{
				// 从body获得IHTMLElement2接口指针，用以访问滚动条
				IHTMLElement2 *pElement = NULL;
				hr = pBody->QueryInterface(IID_IHTMLElement2,(void**)&pElement);
				if (pElement)
				{
					// 向下滚动100个像素
					pElement->put_scrollTop( 100 ); 

					// 获得文档真正的高度，不是可见区域的高度
					long scroll_height; 
					pElement->get_scrollHeight( &scroll_height );
					// 获得文档真正的宽度，不是可见区域的宽度
					long scroll_width; 
					pElement->get_scrollWidth( &scroll_width );
					// 获得滚动条位置，从顶端开始
					long scroll_top;
					pElement->get_scrollTop( &scroll_top );
				}
				
			}
			
		}
#pragma endregion
	}

	virtual void ProgressChange(LONG nProgress, LONG nProgressMax){}
	virtual void NewWindow3(IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl){}
	virtual void CommandStateChange(long Command,VARIANT_BOOL Enable){};

	// interface IDocHostUIHandler
	virtual HRESULT STDMETHODCALLTYPE ShowContextMenu(
		/* [in] */ DWORD dwID,
		/* [in] */ POINT __RPC_FAR *ppt,
		/* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
		/* [in] */ IDispatch __RPC_FAR *pdispReserved)
	{
		return S_OK;
		//return S_FALSE
	}

	virtual HRESULT STDMETHODCALLTYPE GetHostInfo(
		/* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo)
	{
		if (pInfo != NULL)
		{		
			pInfo->cbSize = sizeof(DOCHOSTUIINFO);  
			pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;  
			pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_THEME | 
				DOCHOSTUIFLAG_NO3DOUTERBORDER | DOCHOSTUIFLAG_DIALOG |
				DOCHOSTUIFLAG_DISABLE_HELP_MENU;//| DOCHOSTUIFLAG_SCROLL_NO;;
			LPWSTR m_pZoom = L"BODY{Zoom:100%;}";
			pInfo->pchHostCss = (LPWSTR)::CoTaskMemAlloc((lstrlenW(m_pZoom)+1)*2);
			lstrcpyW(pInfo->pchHostCss, m_pZoom);
		}

		return S_OK;
		
	}

	virtual HRESULT STDMETHODCALLTYPE ShowUI(
		/* [in] */ DWORD dwID,
		/* [in] */ IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
		/* [in] */ IOleCommandTarget __RPC_FAR *pCommandTarget,
		/* [in] */ IOleInPlaceFrame __RPC_FAR *pFrame,
		/* [in] */ IOleInPlaceUIWindow __RPC_FAR *pDoc)
	{
		return S_FALSE;
	}

	virtual HRESULT STDMETHODCALLTYPE HideUI( void)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE UpdateUI( void)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE EnableModeless(
		/* [in] */ BOOL fEnable)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnDocWindowActivate(
		/* [in] */ BOOL fActivate)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(
		/* [in] */ BOOL fActivate)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE ResizeBorder(
		/* [in] */ LPCRECT prcBorder,
		/* [in] */ IOleInPlaceUIWindow __RPC_FAR *pUIWindow,
		/* [in] */ BOOL fRameWindow)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(
		/* [in] */ LPMSG lpMsg,
		/* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
		/* [in] */ DWORD nCmdID)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetOptionKeyPath(
		/* [out] */ LPOLESTR __RPC_FAR *pchKey,
		/* [in] */ DWORD dw)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetDropTarget(
		/* [in] */ IDropTarget __RPC_FAR *pDropTarget,
		/* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetExternal(
		/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE TranslateUrl(
		/* [in] */ DWORD dwTranslate,
		/* [in] */ OLECHAR __RPC_FAR *pchURLIn,
		/* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE FilterDataObject(
		/* [in] */ IDataObject __RPC_FAR *pDO,
		/* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet)
	{
		return S_OK;
	}

	// 	virtual HRESULT STDMETHODCALLTYPE GetOverrideKeyPath( 
	// 		/* [annotation][out] */ 
	// 		__deref_out  LPOLESTR *pchKey,
	// 		/* [in] */ DWORD dw)
	// 	{
	// 		return E_NOTIMPL;
	// 	}

	// IDownloadManager
	virtual HRESULT STDMETHODCALLTYPE Download( 
		/* [in] */ IMoniker *pmk,
		/* [in] */ IBindCtx *pbc,
		/* [in] */ DWORD dwBindVerb,
		/* [in] */ LONG grfBINDF,
		/* [in] */ BINDINFO *pBindInfo,
		/* [in] */ LPCOLESTR pszHeaders,
		/* [in] */ LPCOLESTR pszRedir,
		/* [in] */ UINT uiCP)
	{
		return S_OK;
	}
};

#endif //_CCUSTOM_WEBBROWSER_EVENT_HANDLER_H_