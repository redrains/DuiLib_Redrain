#ifndef DuiAutoComplete_h__
#define DuiAutoComplete_h__

#pragma once
#include <initguid.h>
#include <shldisp.h>
#include <shlguid.h>

namespace UiLib
{
	class UILIB_API CDuiAutoComplete : public IEnumString
	{
	private:
		TStdPtrArray<CDuiString*> m_sStringArrayMap;
		CComPtr<IAutoComplete>& GetIAutoComplete();

		ULONG m_nCurrentElement;
		ULONG m_nRefCount;
		BOOL m_fBound;
		int m_iMaxItemCount;

	public:
		CDuiAutoComplete();
		~CDuiAutoComplete();

	public:
		BOOL Bind(HWND p_hWndEdit, DWORD p_dwOptions = 0, LPCTSTR p_lpszFormatString = NULL);
		VOID Unbind();
		BOOL IsBound() const { return m_fBound; }

		const TStdPtrArray<CDuiString*>& GetStringArray() const;
		int GetItemCount();

		BOOL AddItem(const CDuiString& p_sItem, int iPos = -1);
		BOOL RemoveItem(const CDuiString& p_sItem);
		CDuiString GetItem(int pos);

		BOOL Clear();
		BOOL Disable();
		BOOL Enable(VOID);

		BOOL LoadList(LPCTSTR pszFileName);
		BOOL SaveList(LPCTSTR pszFileName);

	public:
		STDMETHOD_(ULONG,AddRef)();
		STDMETHOD_(ULONG,Release)();
		STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject);

	public:
		STDMETHOD(Next)(ULONG celt, LPOLESTR* rgelt, ULONG* pceltFetched);
		STDMETHOD(Skip)(ULONG celt);
		STDMETHOD(Reset)(void);
		STDMETHOD(Clone)(IEnumString** ppenum);

	private:
		void InternalInit();
		HRESULT EnDisable(BOOL p_fEnable);
		int FindItem(const CDuiString& rstr);
	};
}
#endif // DuiAutoComplete_h__
