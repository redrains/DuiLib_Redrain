#ifndef DROP_TARGET_EX_H
#define DROP_TARGET_EX_H

#include "OleIdl.h"
#include "ShObjIdl.h"

typedef struct _DRAGDATA
{
	int cfFormat;
	STGMEDIUM stgMedium;

}DRAGDATA,*LPDRAGDATA;

typedef void (*DROPCALLBACK)(CFrameWnd*, HDROP);

class CDropTargetEx : public IDropTarget
{
public:
	CDropTargetEx(CFrameWnd *pMainWnd);
	virtual ~CDropTargetEx();

	BOOL DragDropRegister(HWND hWnd,DWORD AcceptKeyState = MK_LBUTTON);

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);

	ULONG STDMETHODCALLTYPE AddRef(void);

	ULONG STDMETHODCALLTYPE Release(void);

	HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState,POINTL pt,	DWORD *pdwEffect);

	HRESULT STDMETHODCALLTYPE DragEnter(IDataObject * pDataObject,DWORD grfKeyState, POINTL pt,	DWORD * pdwEffect);

	HRESULT STDMETHODCALLTYPE DragLeave(void);

	HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj,DWORD grfKeyState,	POINTL pt,	DWORD __RPC_FAR *pdwEffect);

	BOOL GetDragData(IDataObject *pDataObject,FORMATETC cFmt);

	void SetHDropCallBack(DROPCALLBACK pFun);

	void ProcessDrop(LPDRAGDATA pDropData/*HDROP hDrop*/);

	//枚举数据格式的函数，我这里并没有用到，但是将来可能会用,函数目前只枚举了HDROP类型
	BOOL EnumDragData(IDataObject *pDataObject);

private:
	CFrameWnd *m_pMainWnd;
	CDuiRect	m_rcList;

	ULONG	tb_RefCount;
	HWND	m_hTargetWnd;
	DWORD	m_AcceptKeyState;

	bool	m_bUseDnDHelper;
	IDropTargetHelper* m_piDropHelper;

	DROPCALLBACK	m_pDropCallBack;
	vector<LPDRAGDATA> m_Array;
};
#endif	//DROP_TARGET_EX_H