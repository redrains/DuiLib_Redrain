#include "duilib.h"

CDropTargetEx::CDropTargetEx(CFrameWnd *pMainWnd):
	m_pMainWnd(pMainWnd),
	tb_RefCount(0),
	m_hTargetWnd(0),
	m_AcceptKeyState(0),
	m_piDropHelper(NULL),
	m_bUseDnDHelper(false),
	m_pDropCallBack(NULL)
{
	// Create an instance of the shell DnD helper object.
	if ( SUCCEEDED( CoCreateInstance ( CLSID_DragDropHelper, NULL, 
		CLSCTX_INPROC_SERVER,
		IID_IDropTargetHelper, 
		(void**) &m_piDropHelper ) ))
	{
		m_bUseDnDHelper = true;
	}
}

CDropTargetEx::~CDropTargetEx()
{
	if ( NULL != m_piDropHelper )
		m_piDropHelper->Release();
}

BOOL CDropTargetEx::DragDropRegister(HWND hWnd,	DWORD AcceptKeyState)
{
	if(!IsWindow(hWnd))return false;
	HRESULT s = ::RegisterDragDrop (hWnd,this);
	if(SUCCEEDED(s))
	{ 
		m_hTargetWnd = hWnd;
		m_AcceptKeyState = AcceptKeyState; 
		if (m_pMainWnd->GetLeftListPos(m_rcList))
			return true;
		return false;
	}
	else 
	{ 
		return false; 
	}

}

HRESULT STDMETHODCALLTYPE CDropTargetEx::QueryInterface(REFIID iid, void ** ppvObject)
{
	*ppvObject = NULL;

	if (iid == IID_IDropTarget)
		*ppvObject = static_cast<IDropTarget*>(this);

	if( *ppvObject != NULL )
		AddRef();
	return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}


ULONG STDMETHODCALLTYPE CDropTargetEx::AddRef(void)
{
	InterlockedIncrement(&tb_RefCount); 
	return tb_RefCount;
}

ULONG STDMETHODCALLTYPE CDropTargetEx::Release(void)
{
	ULONG ulRefCount = InterlockedDecrement(&tb_RefCount);
	return ulRefCount; 
}

HRESULT STDMETHODCALLTYPE CDropTargetEx::DragOver(DWORD grfKeyState,POINTL pt,	DWORD *pdwEffect)
{
	ScreenToClient(m_hTargetWnd, (LPPOINT)&pt);
	if( grfKeyState != m_AcceptKeyState || pt.x < m_rcList.left || pt.x > m_rcList.right || pt.y < m_rcList.top || pt.y > m_rcList.bottom)
	{
		*pdwEffect = DROPEFFECT_NONE;	
	}
	else
	{
		*pdwEffect = DROPEFFECT_COPY ;
	}
	if ( m_bUseDnDHelper )
	{
		m_piDropHelper->DragOver((LPPOINT)&pt, *pdwEffect);
	}
	
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDropTargetEx::DragEnter(IDataObject * pDataObject,DWORD grfKeyState, POINTL pt,	DWORD * pdwEffect)
{
	if( grfKeyState != m_AcceptKeyState )
	{
		*pdwEffect = DROPEFFECT_NONE;
		return S_OK;
	}
	//我这里只关心CE_HDROP类型，如果需要，可以调用EnumDragData函数来枚举所有类型
	FORMATETC cFmt = {(CLIPFORMAT) CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
	GetDragData(pDataObject, cFmt);

	*pdwEffect = DROPEFFECT_COPY;

	if ( m_bUseDnDHelper )
	{
		m_piDropHelper->DragEnter ( m_hTargetWnd, pDataObject, (LPPOINT)&pt, *pdwEffect );
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDropTargetEx::DragLeave(void)
{
	int temp = m_Array.size();
	for(UINT i = 0;i < m_Array.size(); i++)
	{
		LPDRAGDATA pData = m_Array[i];
		::ReleaseStgMedium(&pData->stgMedium);
		delete pData;
		m_Array.clear();
	}

	if ( m_bUseDnDHelper )
	{
		m_piDropHelper->DragLeave();
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDropTargetEx::Drop(IDataObject *pDataObj,DWORD grfKeyState,	POINTL pt,	DWORD __RPC_FAR *pdwEffect)
{

	int temp = m_Array.size();
	for(UINT i = 0; i < m_Array.size(); i++)
	{
		LPDRAGDATA pData = m_Array[i];

		//我这里只获取了HDROP类型的数据，所以直接开始处理
		ProcessDrop(pData);
		::ReleaseStgMedium(&pData->stgMedium);
		delete pData;
		m_Array.clear();
	}
	
	if ( m_bUseDnDHelper )
	{
		m_piDropHelper->Drop ( pDataObj,  (LPPOINT)&pt, *pdwEffect );
	}

	return S_OK;
}

BOOL CDropTargetEx::EnumDragData(IDataObject *pDataObject)
{	
	IEnumFORMATETC *pEnumFmt = NULL;

	//如果获取成功，则可以通过IEnumFORMATETC接口的Next方法，来枚举所有的数据格式：
	HRESULT ret = pDataObject->EnumFormatEtc (DATADIR_GET,&pEnumFmt);
		pEnumFmt->Reset ();

	HRESULT Ret = S_OK;
	FORMATETC cFmt = {0};
	ULONG Fetched = 0;

	while(Ret != S_OK)
	{
		Ret = pEnumFmt->Next(1,&cFmt,&Fetched);

		if(SUCCEEDED(ret))
		{
			if(  cFmt.cfFormat == CF_HDROP)
			{
				if(GetDragData(pDataObject,cFmt))
					return TRUE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CDropTargetEx::GetDragData(IDataObject *pDataObject,FORMATETC cFmt)
{
	HRESULT ret=S_OK;
	STGMEDIUM stgMedium;

	ret = pDataObject->GetData(&cFmt, &stgMedium);

	if (FAILED(ret))
		return FALSE;

	if (stgMedium.pUnkForRelease != NULL)
		return FALSE;


	switch (stgMedium.tymed)
	{
	//可以扩充这块代码，配合EnumDragData函数来保存更多类型的数据
	case TYMED_HGLOBAL:
		{

			LPDRAGDATA pData = new DRAGDATA;

			pData->cfFormat = cFmt.cfFormat ;
			memcpy(&pData->stgMedium,&stgMedium,sizeof(STGMEDIUM));

			m_Array.push_back(pData);

			return true;
			break;

		}
	default:
		// type not supported, so return error
		{
			::ReleaseStgMedium(&stgMedium);
		}
		break;
	}

	return false;

}

void CDropTargetEx::SetHDropCallBack(DROPCALLBACK pFun)
{
	if (pFun != NULL)
	{
		m_pDropCallBack = pFun;
	}
}

void CDropTargetEx::ProcessDrop(LPDRAGDATA pDropData/*HDROP hDrop*/)
{
	switch(pDropData->cfFormat)
	{
	case CF_TEXT:
		{
			//m_pTextCallBack((HDROP)pDropData->stgMedium.hGlobal);
			break;
		}
	case CF_HDROP:
		{
			m_pDropCallBack(m_pMainWnd, (HDROP)pDropData->stgMedium.hGlobal);
			break;
		}
	default:
		break;
	}

}