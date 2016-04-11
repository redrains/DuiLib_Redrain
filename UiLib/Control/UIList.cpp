#include "StdAfx.h"

namespace UiLib {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CListUI::CListUI() : m_pCallback(NULL), m_bScrollSelect(false)/*, m_iCurSel(-1)*/, m_iExpandedItem(-1),m_bSingleSel(false)
	{
		m_pList = new CListBodyUI(this);
		m_pHeader = new CListHeaderUI;

		Add(m_pHeader);
		CVerticalLayoutUI::Add(m_pList);

		m_ListInfo.nColumns = 0;
		m_ListInfo.nFont = -1;
		m_ListInfo.uTextStyle = DT_VCENTER; // m_uTextStyle(DT_VCENTER | DT_END_ELLIPSIS)
		m_ListInfo.dwTextColor = 0xFF000000;
		m_ListInfo.dwBkColor = 0;
		m_ListInfo.bAlternateBk = false;
		m_ListInfo.dwSelectedTextColor = 0xFF000000;
		m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
		m_ListInfo.dwHotTextColor = 0xFF000000;
		m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
		m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
		m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
		m_ListInfo.dwLineColor = 0;
		m_ListInfo.bShowHtml = false;
		m_ListInfo.bMultiExpandable = false;
		m_ListInfo.bShowHLine = true;
		m_ListInfo.bShowVLine = true;
		::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
		::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
		::ZeroMemory(&m_ListInfo.szCheckImg, sizeof(m_ListInfo.szCheckImg));
		::ZeroMemory(&m_ListInfo.szIconImg, sizeof(m_ListInfo.szIconImg));
	}

	LPCTSTR CListUI::GetClass() const
	{
		return _T("ListUI");
	}

	UINT CListUI::GetControlFlags() const
	{
		return UIFLAG_TABSTOP;
	}

	LPVOID CListUI::GetInterface(LPCTSTR pstrName)
	{
		if(m_pHeader)
			m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);

		if( _tcscmp(pstrName, DUI_CTR_LIST) == 0 ) return static_cast<CListUI*>(this);
		if( _tcscmp(pstrName, _T("IList")) == 0 ) return static_cast<IListUI*>(this);
		if( _tcscmp(pstrName, _T("IListOwner")) == 0 ) return static_cast<IListOwnerUI*>(this);
		return CVerticalLayoutUI::GetInterface(pstrName);
	}

	CControlUI* CListUI::GetItemAt(int iIndex) const
	{
		return m_pList->GetItemAt(iIndex);
	}

	int CListUI::GetItemIndex(CControlUI* pControl) const
	{
		if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) return CVerticalLayoutUI::GetItemIndex(pControl);
		// We also need to recognize header sub-items
		if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return m_pHeader->GetItemIndex(pControl);

		return m_pList->GetItemIndex(pControl);
	}

	bool CListUI::SetItemIndex(CControlUI* pControl, int iIndex)
	{
		if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) return CVerticalLayoutUI::SetItemIndex(pControl, iIndex);
		// We also need to recognize header sub-items
		if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return m_pHeader->SetItemIndex(pControl, iIndex);

		int iOrginIndex = m_pList->GetItemIndex(pControl);
		if( iOrginIndex == -1 ) return false;
		if( iOrginIndex == iIndex ) return true;

		IListItemUI* pSelectedListItem = NULL;
		if( !m_pList->SetItemIndex(pControl, iIndex) ) return false;
		int iMinIndex = min(iOrginIndex, iIndex);
		int iMaxIndex = max(iOrginIndex, iIndex);
		for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
			CControlUI* p = m_pList->GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(DUI_CTR_LISTITEM));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}
		UnSelectAllItems();
		return true;
	}

	int CListUI::GetCount() const
	{
		return m_pList->GetCount();
	}

	bool CListUI::Add(CControlUI* pControl)
	{
		// Override the Add() method so we can add items specifically to
		// the intended widgets. Headers are assumed to be
		// answer the correct interface so we can add multiple list headers.
		if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) {
			if( m_pHeader != pControl && m_pHeader->GetCount() == 0 ) {
				CVerticalLayoutUI::Remove(m_pHeader);
				m_pHeader = static_cast<CListHeaderUI*>(pControl);
			}
			m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
			return CVerticalLayoutUI::AddAt(pControl, 0);
		}
		// We also need to recognize header sub-items
		if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) {
			bool ret = m_pHeader->Add(pControl);
			m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
			return ret;
		}
		// The list items should know about us
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_LISTITEM));
		if( pListItem != NULL ) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(GetCount());
		}
		return m_pList->Add(pControl);
	}

	bool CListUI::AddAt(CControlUI* pControl, int iIndex)
	{
		// Override the AddAt() method so we can add items specifically to
		// the intended widgets. Headers and are assumed to be
		// answer the correct interface so we can add multiple list headers.
		if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) {
			if( m_pHeader != pControl && m_pHeader->GetCount() == 0 ) {
				CVerticalLayoutUI::Remove(m_pHeader);
				m_pHeader = static_cast<CListHeaderUI*>(pControl);
			}
			m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
			return CVerticalLayoutUI::AddAt(pControl, 0);
		}
		// We also need to recognize header sub-items
		if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) {
			bool ret = m_pHeader->AddAt(pControl, iIndex);
			m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
			return ret;
		}
		if (!m_pList->AddAt(pControl, iIndex)) return false;

		// The list items should know about us
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_LISTITEM));
		if( pListItem != NULL ) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(iIndex);
		}

		for(int i = iIndex + 1; i < m_pList->GetCount(); ++i) {
			CControlUI* p = m_pList->GetItemAt(i);
			pListItem = static_cast<IListItemUI*>(p->GetInterface(DUI_CTR_LISTITEM));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}
		UnSelectAllItems();
		return true;
	}

	bool CListUI::Remove(CControlUI* pControl)
	{
		if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) return CVerticalLayoutUI::Remove(pControl);
		// We also need to recognize header sub-items
		if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return m_pHeader->Remove(pControl);

		int iIndex = m_pList->GetItemIndex(pControl);
		if (iIndex == -1) return false;

		if (!m_pList->RemoveAt(iIndex)) return false;

		for(int i = iIndex; i < m_pList->GetCount(); ++i) {
			CControlUI* p = m_pList->GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(DUI_CTR_LISTITEM));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}

		m_aSelItems.Remove(m_aSelItems.Find((LPVOID)iIndex));
		return true;
	}

	bool CListUI::RemoveAt(int iIndex)
	{
		if (!m_pList->RemoveAt(iIndex)) return false;

		for(int i = iIndex; i < m_pList->GetCount(); ++i) {
			CControlUI* p = m_pList->GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(DUI_CTR_LISTITEM));
			if( pListItem != NULL ) pListItem->SetIndex(i);
		}

		m_aSelItems.Remove(m_aSelItems.Find((LPVOID)iIndex));
		return true;
	}

	void CListUI::RemoveAll()
	{
		//m_iCurSel = -1;
		m_iExpandedItem = -1;
		m_pList->RemoveAll();
	}

	void CListUI::SetPos(RECT rc)
	{
		CVerticalLayoutUI::SetPos(rc);
		if( m_pHeader == NULL ) return;
		// Determine general list information and the size of header columns
		m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
		// The header/columns may or may not be visible at runtime. In either case
		// we should determine the correct dimensions...

		if( !m_pHeader->IsVisible() ) {
			for( int it = 0; it < m_pHeader->GetCount(); it++ ) {
				static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
			}
			m_pHeader->SetPos(CDuiRect(rc.left, 0, rc.right, 0));
		}
		int iOffset = m_pList->GetScrollPos().cx;
		for( int i = 0; i < m_ListInfo.nColumns; i++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pHeader->GetItemAt(i));
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;

			RECT rcPos = pControl->GetPos();
			if( iOffset > 0 ) {
				rcPos.left -= iOffset;
				rcPos.right -= iOffset;
				pControl->SetPos(rcPos);
			}
			m_ListInfo.rcColumn[i] = pControl->GetPos();
		}
		if( !m_pHeader->IsVisible() ) {
			for( int it = 0; it < m_pHeader->GetCount(); it++ ) {
				static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
			}
		}

		m_pList->SetPos(m_pList->GetPos());
	}

	int CListUI::GetMinSelItemIndex()
	{
		if (m_aSelItems.GetSize() <= 0)
			return -1;
		int min = (int)m_aSelItems.GetAt(0);
		int index;
		for (int i = 0; i < m_aSelItems.GetSize(); ++i)
		{
			index = (int)m_aSelItems.GetAt(i);
			if (min > index)
				min = index;
		}
		return min;
	}

	int CListUI::GetMaxSelItemIndex()
	{
		if (m_aSelItems.GetSize() <= 0)
			return -1;
		int max = (int)m_aSelItems.GetAt(0);
		int index;
		for (int i = 0; i < m_aSelItems.GetSize(); ++i)
		{
			index = (int)m_aSelItems.GetAt(i);
			if (max < index)
				max = index;
		}
		return max;
	}

	void CListUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CVerticalLayoutUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			m_bFocused = true;
			return;
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			m_bFocused = false;
			return;
		}

		switch( event.Type ) {
		case UIEVENT_KEYDOWN:
			switch( event.chKey ) {
			case VK_UP:
				{
					if (m_aSelItems.GetSize() > 0)
					{					
						int index = GetMinSelItemIndex() - 1;
						UnSelectAllItems();
						index > 0 ? SelectItem(index, true) : SelectItem(0, true);					
					}
				}			
				return;
			case VK_DOWN:
				{
					if (m_aSelItems.GetSize() > 0)
					{					
						int index = GetMaxSelItemIndex() + 1;
						UnSelectAllItems();
						index + 1 > m_pList->GetCount() ? SelectItem(GetCount() - 1, true) : SelectItem(index, true);					
					}
				}
				return;
			case VK_PRIOR:
				PageUp();
				return;
			case VK_NEXT:
				PageDown();
				return;
			case VK_HOME:
				{
					if (m_pList->GetCount() > 0)
						SelectItem(0, true);
				}
				return;
			case VK_END:
				{
					if (m_pList->GetCount() > 0)
						SelectItem(m_pList->GetCount() - 1, true);
				}
				return;        
			case 0x41:
				{
					if (!m_bSingleSel && (GetKeyState(VK_CONTROL) & 0x8000))
						SelectAllItems();
				}
				return;
			}
				break;
			case UIEVENT_SCROLLWHEEL:
				{
					switch( LOWORD(event.wParam) )
					{
					case SB_LINEUP:
						if( m_bScrollSelect && m_bSingleSel)
							SelectItem(FindSelectable((int)m_aSelItems.GetAt(0) - 1, false), true);
						else LineUp();
						return;
					case SB_LINEDOWN:
						if( m_bScrollSelect && m_bSingleSel) 
							SelectItem(FindSelectable((int)m_aSelItems.GetAt(0) + 1, true), true);
						else LineDown();
						return;
					}
				}
				break;

		}
		CVerticalLayoutUI::DoEvent(event);
	}

	CListHeaderUI* CListUI::GetHeader() const
	{
		return m_pHeader;
	}

	CContainerUI* CListUI::GetList() const
	{
		return m_pList;
	}

	bool CListUI::GetScrollSelect()
	{
		return m_bScrollSelect;
	}

	void CListUI::SetScrollSelect(bool bScrollSelect)
	{
		m_bScrollSelect = bScrollSelect;
	}

	int CListUI::GetCurSel() const
	{
		if (m_aSelItems.GetSize() <= 0)
		{
			return -1;
		}
		else
		{
			return (int)m_aSelItems.GetAt(0);
		}
	}

	bool CListUI::SelectItem(int iIndex, bool bTakeFocus)
	{
		if( iIndex < 0 ) return false;
		CControlUI* pControl = GetItemAt(iIndex);
		if( pControl == NULL ) return false;
		if( !pControl->IsVisible() ) return false;
		if( !pControl->IsEnabled() ) return false;
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem == NULL ) return false;

		UnSelectAllItems();

		if(m_bSingleSel && m_aSelItems.GetSize() > 0) {
			CControlUI* pControl = GetItemAt((int)m_aSelItems.GetAt(0));
			if( pControl != NULL) {
				IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
				if( pListItem != NULL ) pListItem->Select(false);
			}		
		}	

		if( !pListItem->Select(true) ) {		
			return false;
		}

		m_aSelItems.Add((LPVOID)iIndex);

		EnsureVisible(iIndex);
		if( bTakeFocus ) pControl->SetFocus();
		if( m_pManager != NULL ) {
			m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, iIndex);
		}

		return true;
	}

	TListInfoUI* CListUI::GetListInfo()
	{
		return &m_ListInfo;
	}

	bool CListUI::IsDelayedDestroy() const
	{
		return m_pList->IsDelayedDestroy();
	}

	void CListUI::SetDelayedDestroy(bool bDelayed)
	{
		m_pList->SetDelayedDestroy(bDelayed);
	}

	int CListUI::GetChildPadding() const
	{
		return m_pList->GetChildPadding();
	}

	void CListUI::SetChildPadding(int iPadding)
	{
		m_pList->SetChildPadding(iPadding);
	}

	void CListUI::SetItemFont(int index)
	{
		m_ListInfo.nFont = index;
		NeedUpdate();
	}

	void CListUI::SetItemTextStyle(UINT uStyle)
	{
		m_ListInfo.uTextStyle = uStyle;
		NeedUpdate();
	}

	void CListUI::SetItemTextPadding(RECT rc)
	{
		m_ListInfo.rcTextPadding = rc;
		NeedUpdate();
	}

	RECT CListUI::GetItemTextPadding() const
	{
		return m_ListInfo.rcTextPadding;
	}

	void CListUI::SetItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwTextColor = dwTextColor;
		Invalidate();
	}

	void CListUI::SetItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwBkColor = dwBkColor;
		Invalidate();
	}

	void CListUI::SetItemBkImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sBkImage = pStrImage;
		Invalidate();
	}

	void CListUI::SetAlternateBk(bool bAlternateBk)
	{
		m_ListInfo.bAlternateBk = bAlternateBk;
		Invalidate();
	}

	DWORD CListUI::GetItemTextColor() const
	{
		return m_ListInfo.dwTextColor;
	}

	DWORD CListUI::GetItemBkColor() const
	{
		return m_ListInfo.dwBkColor;
	}

	LPCTSTR CListUI::GetItemBkImage() const
	{
		return m_ListInfo.sBkImage;
	}

	bool CListUI::IsAlternateBk() const
	{
		return m_ListInfo.bAlternateBk;
	}

	void CListUI::SetSelectedItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwSelectedTextColor = dwTextColor;
		Invalidate();
	}

	void CListUI::SetSelectedItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwSelectedBkColor = dwBkColor;
		Invalidate();
	}

	void CListUI::SetSelectedItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sSelectedImage = pStrImage;
		Invalidate();
	}

	DWORD CListUI::GetSelectedItemTextColor() const
	{
		return m_ListInfo.dwSelectedTextColor;
	}

	DWORD CListUI::GetSelectedItemBkColor() const
	{
		return m_ListInfo.dwSelectedBkColor;
	}

	LPCTSTR CListUI::GetSelectedItemImage() const
	{
		return m_ListInfo.sSelectedImage;
	}

	void CListUI::SetHotItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwHotTextColor = dwTextColor;
		Invalidate();
	}

	void CListUI::SetHotItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwHotBkColor = dwBkColor;
		Invalidate();
	}

	void CListUI::SetHotItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sHotImage = pStrImage;
		Invalidate();
	}

	DWORD CListUI::GetHotItemTextColor() const
	{
		return m_ListInfo.dwHotTextColor;
	}
	DWORD CListUI::GetHotItemBkColor() const
	{
		return m_ListInfo.dwHotBkColor;
	}

	LPCTSTR CListUI::GetHotItemImage() const
	{
		return m_ListInfo.sHotImage;
	}

	void CListUI::SetDisabledItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwDisabledTextColor = dwTextColor;
		Invalidate();
	}

	void CListUI::SetDisabledItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwDisabledBkColor = dwBkColor;
		Invalidate();
	}

	void CListUI::SetDisabledItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sDisabledImage = pStrImage;
		Invalidate();
	}

	DWORD CListUI::GetDisabledItemTextColor() const
	{
		return m_ListInfo.dwDisabledTextColor;
	}

	DWORD CListUI::GetDisabledItemBkColor() const
	{
		return m_ListInfo.dwDisabledBkColor;
	}

	LPCTSTR CListUI::GetDisabledItemImage() const
	{
		return m_ListInfo.sDisabledImage;
	}

	DWORD CListUI::GetItemLineColor() const
	{
		return m_ListInfo.dwLineColor;
	}

	void CListUI::SetItemLineColor(DWORD dwLineColor)
	{
		m_ListInfo.dwLineColor = dwLineColor;
		Invalidate();
	}

	bool CListUI::IsItemShowHtml()
	{
		return m_ListInfo.bShowHtml;
	}

	void CListUI::SetItemShowHtml(bool bShowHtml)
	{
		if( m_ListInfo.bShowHtml == bShowHtml ) return;

		m_ListInfo.bShowHtml = bShowHtml;
		NeedUpdate();
	}

	void CListUI::SetMultiExpanding(bool bMultiExpandable)
	{
		m_ListInfo.bMultiExpandable = bMultiExpandable;
	}

	bool CListUI::ExpandItem(int iIndex, bool bExpand /*= true*/)
	{
		if( m_iExpandedItem >= 0 && !m_ListInfo.bMultiExpandable) {
			CControlUI* pControl = GetItemAt(m_iExpandedItem);
			if( pControl != NULL ) {
				IListItemUI* pItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
				if( pItem != NULL ) pItem->Expand(false);
			}
			m_iExpandedItem = -1;
		}
		if( bExpand ) {
			CControlUI* pControl = GetItemAt(iIndex);
			if( pControl == NULL ) return false;
			if( !pControl->IsVisible() ) return false;
			IListItemUI* pItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pItem == NULL ) return false;
			m_iExpandedItem = iIndex;
			if( !pItem->Expand(true) ) {
				m_iExpandedItem = -1;
				return false;
			}
		}
		NeedUpdate();
		return true;
	}

	int CListUI::GetExpandedItem() const
	{
		return m_iExpandedItem;
	}

	void CListUI::EnsureVisible(int iIndex)
	{
		RECT rcItem = m_pList->GetItemAt(iIndex)->GetPos();
		RECT rcList = m_pList->GetPos();
		RECT rcListInset = m_pList->GetInset();

		rcList.left += rcListInset.left;
		rcList.top += rcListInset.top;
		rcList.right -= rcListInset.right;
		rcList.bottom -= rcListInset.bottom;

		CScrollBarUI* pHorizontalScrollBar = m_pList->GetHorizontalScrollBar();
		if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();

		int iPos = m_pList->GetScrollPos().cy;
		if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
		int dx = 0;
		if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
		if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
		Scroll(0, dx);
	}

	void CListUI::Scroll(int dx, int dy)
	{
		if( dx == 0 && dy == 0 ) return;
		SIZE sz = m_pList->GetScrollPos();
		m_pList->SetScrollPos(CSize(sz.cx + dx, sz.cy + dy));
	}

	void CListUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("header")) == 0 ) GetHeader()->SetVisible(_tcscmp(pstrValue, _T("hidden")) != 0);
		else if( _tcscmp(pstrName, _T("headerbkimage")) == 0 ) GetHeader()->SetBkImage(pstrValue);
		else if( _tcscmp(pstrName, _T("scrollselect")) == 0 ) SetScrollSelect(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("multiexpanding")) == 0 ) SetMultiExpanding(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("multipleitem")) == 0 ) SetMultipleItem(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("showvline")) == 0 ) SetShowVLine(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("showhline")) == 0 ) SetShowHLine(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("itemfont")) == 0 ) m_ListInfo.nFont = _ttoi(pstrValue);
		else if( _tcscmp(pstrName, _T("itemalign")) == 0 ) {
			if( _tcsstr(pstrValue, _T("left")) != NULL ) {
				m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
				m_ListInfo.uTextStyle |= DT_LEFT;
			}
			if( _tcsstr(pstrValue, _T("center")) != NULL ) {
				m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
				m_ListInfo.uTextStyle |= DT_CENTER;
			}
			if( _tcsstr(pstrValue, _T("right")) != NULL ) {
				m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
				m_ListInfo.uTextStyle |= DT_RIGHT;
			}
		}
		else if( _tcscmp(pstrName, _T("itemendellipsis")) == 0 ) {
			if( _tcscmp(pstrValue, _T("true")) == 0 ) m_ListInfo.uTextStyle |= DT_END_ELLIPSIS;
			else m_ListInfo.uTextStyle &= ~DT_END_ELLIPSIS;
		}    
		else if( _tcscmp(pstrName, _T("itemtextpadding")) == 0 ) {
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetItemTextPadding(rcTextPadding);
		}
		else if( _tcscmp(pstrName, _T("itemcheckimgsize")) == 0 ) {
			SIZE szCheckImg;
			LPTSTR pstr = NULL;
			szCheckImg.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szCheckImg.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    		
			SetCheckImgSize(szCheckImg);
		}
		else if( _tcscmp(pstrName, _T("itemiconimgsize")) == 0 ) {
			SIZE szIconImg;
			LPTSTR pstr = NULL;
			szIconImg.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szIconImg.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    		
			SetIconImgSize(szIconImg);
		}
		else if( _tcscmp(pstrName, _T("itemtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itembkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemBkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itembkimage")) == 0 ) SetItemBkImage(pstrValue);
		else if( _tcscmp(pstrName, _T("itemaltbk")) == 0 ) SetAlternateBk(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("itemselectedtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedItemTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemselectedbkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedItemBkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemselectedimage")) == 0 ) SetSelectedItemImage(pstrValue);
		else if( _tcscmp(pstrName, _T("itemhottextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetHotItemTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemhotbkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetHotItemBkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemhotimage")) == 0 ) SetHotItemImage(pstrValue);
		else if( _tcscmp(pstrName, _T("itemdisabledtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetDisabledItemTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemdisabledbkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetDisabledItemBkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemdisabledimage")) == 0 ) SetDisabledItemImage(pstrValue);
		else if( _tcscmp(pstrName, _T("itemlinecolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemLineColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemshowhtml")) == 0 ) SetItemShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
		else CVerticalLayoutUI::SetAttribute(pstrName, pstrValue);
	}

	IListCallbackUI* CListUI::GetTextCallback() const
	{
		return m_pCallback;
	}

	void CListUI::SetTextCallback(IListCallbackUI* pCallback)
	{
		m_pCallback = pCallback;
	}

	SIZE CListUI::GetScrollPos() const
	{
		return m_pList->GetScrollPos();
	}

	SIZE CListUI::GetScrollRange() const
	{
		return m_pList->GetScrollRange();
	}

	void CListUI::SetScrollPos(SIZE szPos)
	{
		m_pList->SetScrollPos(szPos);
	}

	void CListUI::LineUp()
	{
		m_pList->LineUp();
	}

	void CListUI::LineDown()
	{
		m_pList->LineDown();
	}

	void CListUI::PageUp()
	{
		m_pList->PageUp();
	}

	void CListUI::PageDown()
	{
		m_pList->PageDown();
	}

	void CListUI::HomeUp()
	{
		m_pList->HomeUp();
	}

	void CListUI::EndDown()
	{
		m_pList->EndDown();
	}

	void CListUI::LineLeft()
	{
		m_pList->LineLeft();
	}

	void CListUI::LineRight()
	{
		m_pList->LineRight();
	}

	void CListUI::PageLeft()
	{
		m_pList->PageLeft();
	}

	void CListUI::PageRight()
	{
		m_pList->PageRight();
	}

	void CListUI::HomeLeft()
	{
		m_pList->HomeLeft();
	}

	void CListUI::EndRight()
	{
		m_pList->EndRight();
	}

	void CListUI::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
	{
		m_pList->EnableScrollBar(bEnableVertical, bEnableHorizontal);
	}

	CScrollBarUI* CListUI::GetVerticalScrollBar() const
	{
		return m_pList->GetVerticalScrollBar();
	}

	CScrollBarUI* CListUI::GetHorizontalScrollBar() const
	{
		return m_pList->GetHorizontalScrollBar();
	}

	bool CListUI::SelectMultiItem( int iIndex, bool bTakeFocus /*= false*/ )
	{
			if (m_bSingleSel)
			{
				return SelectItem(iIndex, bTakeFocus);
			}

			if( iIndex < 0 ) return false;
			CControlUI* pControl = GetItemAt(iIndex);
			if( pControl == NULL ) return false;
			if( !pControl->IsVisible() ) return false;
			if( !pControl->IsEnabled() ) return false;
			IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pListItem == NULL ) return false;

			if (m_aSelItems.Find((LPVOID)iIndex) >= 0)
				return false;

			if(m_bSingleSel && m_aSelItems.GetSize() > 0) {
				CControlUI* pControl = GetItemAt((int)m_aSelItems.GetAt(0));
				if( pControl != NULL) {
					IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
					if( pListItem != NULL ) pListItem->Select(false);
				}		
			}	

			if( !pListItem->Select(true) ) {		
				return false;
			}

			m_aSelItems.Add((LPVOID)iIndex);

			EnsureVisible(iIndex);
			if( bTakeFocus ) pControl->SetFocus();
			if( m_pManager != NULL ) {
				m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, iIndex);
			}

			return true;
	}

	void CListUI::SetSingleSelect( bool bSingleSel )
	{
			m_bSingleSel = bSingleSel;
			UnSelectAllItems();
	}

	bool CListUI::GetSingleSelect() const
	{
			return m_bSingleSel;
	}

	bool CListUI::UnSelectItem( int iIndex )
	{
			if( iIndex < 0 ) return false;
			CControlUI* pControl = GetItemAt(iIndex);
			if( pControl == NULL ) return false;
			if( !pControl->IsVisible() ) return false;
			if( !pControl->IsEnabled() ) return false;
			IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pListItem == NULL ) return false;

			int aIndex = m_aSelItems.Find((LPVOID)iIndex);
			if (aIndex < 0)
				return false;

			if( !pListItem->Select(false) ) {		
				return false;
			}

			m_aSelItems.Remove(aIndex);

			return true;
	}

	void CListUI::SelectAllItems()
	{
			UnSelectAllItems();
			CControlUI* pControl;
			for (int i = 0; i < GetCount(); ++i)
			{
				pControl = GetItemAt(i);
				if(pControl == NULL)
					continue;
				if(!pControl->IsVisible())
					continue;
				if(!pControl->IsEnabled())
					continue;
				IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
				if( pListItem == NULL )
					continue;
				if( !pListItem->Select(true) )
					continue;
				m_aSelItems.Add((LPVOID)i);
			}
	}

	void CListUI::UnSelectAllItems()
	{
			CControlUI* pControl;
			int itemIndex;
			for (int i = 0; i < m_aSelItems.GetSize(); ++i)
			{
				itemIndex = (int)m_aSelItems.GetAt(i);
				pControl = GetItemAt(itemIndex);
				if(pControl == NULL)
					continue;
				if(!pControl->IsVisible())
					continue;
				if(!pControl->IsEnabled())
					continue;
				IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
				if( pListItem == NULL )
					continue;
				if( !pListItem->Select(false) )
					continue;		
			}
			m_aSelItems.Empty();
	}

	int CListUI::GetSelectItemCount() const
	{
		return m_aSelItems.GetSize();
	}

	int CListUI::GetNextSelItem( int nItem ) const
	{
		if (m_aSelItems.GetSize() <= 0)
			return -1;

		if (nItem < 0)
		{
			return (int)m_aSelItems.GetAt(0);
		}
		int aIndex = m_aSelItems.Find((LPVOID)nItem);
		if (aIndex < 0)
			return -1;
		if (aIndex + 1 > m_aSelItems.GetSize() - 1)
			return -1;
		return (int)m_aSelItems.GetAt(aIndex + 1);
	}

	void CListUI::SetCheckImgSize( SIZE szCheckImg )
	{
		m_ListInfo.szCheckImg = szCheckImg;
	}

	void CListUI::SetIconImgSize( SIZE szIconImg )
	{
			m_ListInfo.szIconImg = szIconImg;
	}

	void CListUI::SetShowVLine( bool bVLine )
	{
		m_ListInfo.bShowVLine = bVLine;
	}

	void CListUI::SetShowHLine( bool bHLine )
	{
		m_ListInfo.bShowHLine = bHLine;
	}

	SIZE CListUI::GetCheckImgSize() const
	{
		return m_ListInfo.szCheckImg;
	}

	SIZE CListUI::GetIconImgSize() const
	{
			return m_ListInfo.szIconImg;
	}

	bool CListUI::IsShowVLine() const
	{
		return m_ListInfo.bShowVLine;
	}

	bool CListUI::IsShowHLine() const
	{
		return m_ListInfo.bShowHLine;
	}

	BOOL CListUI::SortItems( PULVCompareFunc pfnCompare, UINT_PTR dwData )
	{
			if (!m_pList)
				return FALSE;
			return m_pList->SortItems(pfnCompare, dwData);	
	}

	void CListUI::SetMultipleItem( bool bMultipleable )
	{
			m_bSingleSel = !bMultipleable;
	}

	bool CListUI::GetMultipleItem() const
	{
		return !m_bSingleSel;
	}



	/////////////////////////////////////////////////////////////////////////////////////
	//
	//


	CListBodyUI::CListBodyUI(CListUI* pOwner) : m_pOwner(pOwner)
	{
		ASSERT(m_pOwner);
	}

	void CListBodyUI::SetScrollPos(SIZE szPos)
	{
		int cx = 0;
		int cy = 0;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
			m_pVerticalScrollBar->SetScrollPos(szPos.cy);
			cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
			m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
			cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if( cx == 0 && cy == 0 ) return;

		if( cx != 0 && m_pOwner ) {
			CListHeaderUI* pHeader = m_pOwner->GetHeader();
			if( pHeader == NULL ) return;
			TListInfoUI* pInfo = m_pOwner->GetListInfo();
			pInfo->nColumns = MIN(pHeader->GetCount(), UILIST_MAX_COLUMNS);

			if( !pHeader->IsVisible() ) {
				for( int it = 0; it < pHeader->GetCount(); it++ ) {
					static_cast<CControlUI*>(pHeader->GetItemAt(it))->SetInternVisible(true);
				}
			}
			for( int i = 0; i < pInfo->nColumns; i++ ) {
				CControlUI* pControl = static_cast<CControlUI*>(pHeader->GetItemAt(i));
				if( !pControl->IsVisible() ) continue;
				if( pControl->IsFloat() ) continue;

				RECT rcPos = pControl->GetPos();
				rcPos.left -= cx;
				rcPos.right -= cx;
				pControl->SetPos(rcPos);
				pInfo->rcColumn[i] = pControl->GetPos();
			}
			if( !pHeader->IsVisible() ) {
				for( int it = 0; it < pHeader->GetCount(); it++ ) {
					static_cast<CControlUI*>(pHeader->GetItemAt(it))->SetInternVisible(false);
				}
			}
		}

		RECT rcPos;
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;

			rcPos = pControl->GetPos();
			rcPos.left -= cx;
			rcPos.right -= cx;
			rcPos.top -= cy;
			rcPos.bottom -= cy;
			pControl->SetPos(rcPos);
		}

		Invalidate();


	}

	void CListBodyUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		rc = m_rcItem;

		// Adjust for inset
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

		// Determine the minimum size
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
			szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();

		int cxNeeded = 0;
		int nAdjustables = 0;
		int cyFixed = 0;
		int nEstimateNum = 0;
		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			if( sz.cy == 0 ) {
				nAdjustables++;
			}
			else {
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;

			RECT rcPadding = pControl->GetPadding();
			sz.cx = MAX(sz.cx, 0);
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			cxNeeded = MAX(cxNeeded, sz.cx);
			nEstimateNum++;
		}
		cyFixed += (nEstimateNum - 1) * m_iChildPadding;

		if( m_pOwner ) {
			CListHeaderUI* pHeader = m_pOwner->GetHeader();
			if( pHeader != NULL && pHeader->GetCount() > 0 ) {
				cxNeeded = MAX(0, pHeader->EstimateSize(CSize(rc.right - rc.left, rc.bottom - rc.top)).cx);
				if ( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
				{
					int nOffset = (NULL!=m_pHorizontalScrollBar)?m_pHorizontalScrollBar->GetScrollPos():0; // modify by watertoeast, 2015-11-2
					RECT rcHeader = pHeader->GetPos();
					rcHeader.left = rc.left - nOffset;
					pHeader->SetPos(rcHeader);
				}
			}
		}

		// Place elements
		int cyNeeded = 0;
		int cyExpand = 0;
		if( nAdjustables > 0 ) cyExpand = MAX(0, (szAvailable.cy - cyFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosY = rc.top;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			iPosY -= m_pVerticalScrollBar->GetScrollPos();
		}
		int iPosX = rc.left;
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			iPosX -= m_pHorizontalScrollBar->GetScrollPos();
		}
		int iAdjustable = 0;
		int cyFixedRemaining = cyFixed;
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}

			RECT rcPadding = pControl->GetPadding();
			szRemaining.cy -= rcPadding.top;
			SIZE sz = pControl->EstimateSize(szRemaining);
			if( sz.cy == 0 ) {
				iAdjustable++;
				sz.cy = cyExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if( iAdjustable == nAdjustables ) {
					sz.cy = MAX(0, szRemaining.cy - rcPadding.bottom - cyFixedRemaining);
				} 
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			else {
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				cyFixedRemaining -= sz.cy;
			}

			sz.cx = MAX(cxNeeded, szAvailable.cx - rcPadding.left - rcPadding.right);

			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

			RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
			pControl->SetPos(rcCtrl);

			iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
			cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
			szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
		}
		cyNeeded += (nEstimateNum - 1) * m_iChildPadding;

		if( m_pHorizontalScrollBar != NULL ) {
			if( cxNeeded > rc.right - rc.left ) {
				if( m_pHorizontalScrollBar->IsVisible() ) {
					m_pHorizontalScrollBar->SetScrollRange(cxNeeded - (rc.right - rc.left));
				}
				else {
					m_pHorizontalScrollBar->SetVisible(true);
					m_pHorizontalScrollBar->SetScrollRange(cxNeeded - (rc.right - rc.left));
					m_pHorizontalScrollBar->SetScrollPos(0);
					rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
				}
			}
			else {
				if( m_pHorizontalScrollBar->IsVisible() ) {
					m_pHorizontalScrollBar->SetVisible(false);
					m_pHorizontalScrollBar->SetScrollRange(0);
					m_pHorizontalScrollBar->SetScrollPos(0);
					rc.bottom += m_pHorizontalScrollBar->GetFixedHeight();
				}
			}
		}

		// Process the scrollbar
		ProcessScrollBar(rc, cxNeeded, cyNeeded);
	}

	void CListBodyUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}

		if( m_pOwner != NULL ) m_pOwner->DoEvent(event); else CControlUI::DoEvent(event);
	}

	int __cdecl CListBodyUI::ItemComareFunc( const void *item1, const void *item2 )
	{
		CControlUI *pControl1 = *(CControlUI**)item1;
		CControlUI *pControl2 = *(CControlUI**)item2;
		return m_pCompareFunc((UINT_PTR)pControl1, (UINT_PTR)pControl2, m_compareData);
	}

	int __cdecl CListBodyUI::ItemComareFunc( void *pvlocale, const void *item1, const void *item2 )
	{
		CListBodyUI *pThis = (CListBodyUI*)pvlocale;
		if (!pThis || !item1 || !item2)
			return 0;
		return pThis->ItemComareFunc(item1, item2);
	}

	BOOL CListBodyUI::SortItems( PULVCompareFunc pfnCompare, UINT_PTR dwData )
	{
		if (!pfnCompare)
			return FALSE;
		m_pCompareFunc = pfnCompare;
		CControlUI **pData = (CControlUI **)m_items.GetData();
		qsort_s(m_items.GetData(), m_items.GetSize(), sizeof(CControlUI*), CListBodyUI::ItemComareFunc, this);	
		IListItemUI *pItem = NULL;
		for (int i = 0; i < m_items.GetSize(); ++i)
		{
			pItem = (IListItemUI*)(static_cast<CControlUI*>(m_items[i])->GetInterface(TEXT("ListItem")));
			if (pItem)
			{
				pItem->SetIndex(i);
				pItem->Select(false);
			}
		}
		m_pOwner->SelectItem(-1);
		if (m_pManager)
		{
			SetPos(GetPos());
			Invalidate();
		}

		return TRUE;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CListHeaderUI::CListHeaderUI():
	m_bIsScaleHeader(false)
	{
	}

	LPCTSTR CListHeaderUI::GetClass() const
	{
		return _T("ListHeaderUI");
	}

	LPVOID CListHeaderUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_LISTHEADER) == 0 ) return this;
		return CHorizontalLayoutUI::GetInterface(pstrName);
	}

	SIZE CListHeaderUI::EstimateSize(SIZE szAvailable)
	{
		SIZE cXY = {0, m_cxyFixed.cy};
		if( cXY.cy == 0 && m_pManager != NULL ) {
			for( int it = 0; it < m_items.GetSize(); it++ ) {
				cXY.cy = MAX(cXY.cy,static_cast<CControlUI*>(m_items[it])->EstimateSize(szAvailable).cy);
			}
			int nMin = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 6;
			cXY.cy = MAX(cXY.cy,nMin);
		}

		for( int it = 0; it < m_items.GetSize(); it++ ) {
			cXY.cx +=  static_cast<CControlUI*>(m_items[it])->EstimateSize(szAvailable).cx;
		}

		return cXY;
	}

	void CListHeaderUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		rc = m_rcItem;

		// Adjust for inset
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		if( m_items.GetSize() == 0) {
			return;
		}


		// Determine the width of elements that are sizeable
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

		int nAdjustables = 0;
		int cxFixed = 0;
		int nEstimateNum = 0;
		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			if( sz.cx == 0 ) {
				nAdjustables++;
			}
			else {
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			}
			cxFixed += sz.cx +  pControl->GetPadding().left + pControl->GetPadding().right;
			nEstimateNum++;
		}
		cxFixed += (nEstimateNum - 1) * m_iChildPadding;

		int cxExpand = 0;
		int cxNeeded = 0;
		if( nAdjustables > 0 ) cxExpand = MAX(0, (szAvailable.cx - cxFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosX = rc.left;

		int iAdjustable = 0;
		int cxFixedRemaining = cxFixed;

		int nHeaderWidth = GetWidth();
		CListUI *pList = static_cast<CListUI*>(GetParent());
		if (pList != NULL)
		{
			CScrollBarUI* pVScroll = pList->GetVerticalScrollBar();
			if (pVScroll != NULL)
				nHeaderWidth -= pVScroll->GetWidth();
		}
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}
			RECT rcPadding = pControl->GetPadding();
			szRemaining.cx -= rcPadding.left;

			SIZE sz = {0,0};
			if (m_bIsScaleHeader)
			{
				CListHeaderItemUI* pHeaderItem = static_cast<CListHeaderItemUI*>(pControl);
				sz.cx = int(nHeaderWidth * (float)pHeaderItem->GetScale() / 100);
			}
			else
			{
				sz = pControl->EstimateSize(szRemaining);
			}

			if( sz.cx == 0 ) {
				iAdjustable++;
				sz.cx = cxExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if( iAdjustable == nAdjustables ) {
					sz.cx = MAX(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
				}
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			}
			else {
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

				cxFixedRemaining -= sz.cx;
			}

			sz.cy = pControl->GetFixedHeight();
			if( sz.cy == 0 ) sz.cy = rc.bottom - rc.top - rcPadding.top - rcPadding.bottom;
			if( sz.cy < 0 ) sz.cy = 0;
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();

			RECT rcCtrl = { iPosX + rcPadding.left, rc.top + rcPadding.top, iPosX + sz.cx + rcPadding.left + rcPadding.right, rc.top + rcPadding.top + sz.cy};
			pControl->SetPos(rcCtrl);
			iPosX += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right;
			cxNeeded += sz.cx + rcPadding.left + rcPadding.right;
			szRemaining.cx -= sz.cx + m_iChildPadding + rcPadding.right;
		}
		cxNeeded += (nEstimateNum - 1) * m_iChildPadding;
	}

	void CListHeaderUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("scaleheader")) == 0 ) SetScaleHeader(_tcscmp(pstrValue, _T("true")) == 0);
		else CHorizontalLayoutUI::SetAttribute(pstrName, pstrValue);
	}

	void CListHeaderUI::SetScaleHeader(bool bIsScale)
	{
		m_bIsScaleHeader = bIsScale;
	}

	bool CListHeaderUI::IsScaleHeader() const
	{
		return m_bIsScaleHeader;
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CListHeaderItemUI::CListHeaderItemUI() : m_bDragable(true), m_uButtonState(0), m_iSepWidth(4),
		m_uTextStyle(DT_VCENTER | DT_CENTER | DT_SINGLELINE), m_dwTextColor(0), m_iFont(-1), m_bShowHtml(false),m_nScale(0)
	{
		SetTextPadding(CDuiRect(2, 0, 2, 0));
		ptLastMouse.x = ptLastMouse.y = 0;
		SetMinWidth(16);
	}

	LPCTSTR CListHeaderItemUI::GetClass() const
	{
		return _T("ListHeaderItemUI");
	}

	LPVOID CListHeaderItemUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_LISTHEADERITEM) == 0 ) return this;
		return CControlUI::GetInterface(pstrName);
	}

	UINT CListHeaderItemUI::GetControlFlags() const
	{
		if( IsEnabled() && m_iSepWidth != 0 ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	void CListHeaderItemUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	bool CListHeaderItemUI::IsDragable() const
	{
		return m_bDragable;
	}

	void CListHeaderItemUI::SetDragable(bool bDragable)
	{
		m_bDragable = bDragable;
		if ( !m_bDragable ) m_uButtonState &= ~UISTATE_CAPTURED;
	}

	DWORD CListHeaderItemUI::GetSepWidth() const
	{
		return m_iSepWidth;
	}

	void CListHeaderItemUI::SetSepWidth(int iWidth)
	{
		m_iSepWidth = iWidth;
	}

	DWORD CListHeaderItemUI::GetTextStyle() const
	{
		return m_uTextStyle;
	}

	void CListHeaderItemUI::SetTextStyle(UINT uStyle)
	{
		m_uTextStyle = uStyle;
		Invalidate();
	}

	DWORD CListHeaderItemUI::GetTextColor() const
	{
		return m_dwTextColor;
	}


	void CListHeaderItemUI::SetTextColor(DWORD dwTextColor)
	{
		m_dwTextColor = dwTextColor;
	}

	RECT CListHeaderItemUI::GetTextPadding() const
	{
		return m_rcTextPadding;
	}

	void CListHeaderItemUI::SetTextPadding(RECT rc)
	{
		m_rcTextPadding = rc;
		Invalidate();
	}

	void CListHeaderItemUI::SetFont(int index)
	{
		m_iFont = index;
	}

	bool CListHeaderItemUI::IsShowHtml()
	{
		return m_bShowHtml;
	}

	void CListHeaderItemUI::SetShowHtml(bool bShowHtml)
	{
		if( m_bShowHtml == bShowHtml ) return;

		m_bShowHtml = bShowHtml;
		Invalidate();
	}

	LPCTSTR CListHeaderItemUI::GetNormalImage() const
	{
		return m_sNormalImage;
	}

	void CListHeaderItemUI::SetNormalImage(LPCTSTR pStrImage)
	{
		m_sNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CListHeaderItemUI::GetHotImage() const
	{
		return m_sHotImage;
	}

	void CListHeaderItemUI::SetHotImage(LPCTSTR pStrImage)
	{
		m_sHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CListHeaderItemUI::GetPushedImage() const
	{
		return m_sPushedImage;
	}

	void CListHeaderItemUI::SetPushedImage(LPCTSTR pStrImage)
	{
		m_sPushedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CListHeaderItemUI::GetFocusedImage() const
	{
		return m_sFocusedImage;
	}

	void CListHeaderItemUI::SetFocusedImage(LPCTSTR pStrImage)
	{
		m_sFocusedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CListHeaderItemUI::GetSepImage() const
	{
		return m_sSepImage;
	}

	void CListHeaderItemUI::SetSepImage(LPCTSTR pStrImage)
	{
		m_sSepImage = pStrImage;
		Invalidate();
	}

	void CListHeaderItemUI::SetScale(int nScale)
	{
		m_nScale = nScale;
	}

	int CListHeaderItemUI::GetScale() const
	{
		return m_nScale;
	}

	void CListHeaderItemUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("dragable")) == 0 ) SetDragable(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("sepwidth")) == 0 ) SetSepWidth(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("align")) == 0 ) {
			if( _tcsstr(pstrValue, _T("left")) != NULL ) {
				m_uTextStyle &= ~(DT_CENTER | DT_RIGHT);
				m_uTextStyle |= DT_LEFT;
			}
			if( _tcsstr(pstrValue, _T("center")) != NULL ) {
				m_uTextStyle &= ~(DT_LEFT | DT_RIGHT);
				m_uTextStyle |= DT_CENTER;
			}
			if( _tcsstr(pstrValue, _T("right")) != NULL ) {
				m_uTextStyle &= ~(DT_LEFT | DT_CENTER);
				m_uTextStyle |= DT_RIGHT;
			}
		}
		else if( _tcscmp(pstrName, _T("endellipsis")) == 0 ) {
			if( _tcscmp(pstrValue, _T("true")) == 0 ) m_uTextStyle |= DT_END_ELLIPSIS;
			else m_uTextStyle &= ~DT_END_ELLIPSIS;
		}    
		else if( _tcscmp(pstrName, _T("font")) == 0 ) SetFont(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("textcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("textpadding")) == 0 ) {
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetTextPadding(rcTextPadding);
		}
		else if( _tcscmp(pstrName, _T("showhtml")) == 0 ) SetShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("pushedimage")) == 0 ) SetPushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("sepimage")) == 0 ) SetSepImage(pstrValue);
		else if( _tcscmp(pstrName, _T("scale")) == 0 ) {
			LPTSTR pstr = NULL;
			SetScale(_tcstol(pstrValue, &pstr, 10)); 

		}
		else CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CListHeaderItemUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
		{
			if( !IsEnabled() ) return;
			RECT rcSeparator = GetThumbRect();
			if (m_iSepWidth>=0)//111024 by cddjr, ���ӷָ������򣬷����û��϶�
				rcSeparator.left-=4;
			else
				rcSeparator.right+=4;
			if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
				if( m_bDragable ) {
					m_uButtonState |= UISTATE_CAPTURED;
					ptLastMouse = event.ptMouse;
				}
			}
			else {
				m_uButtonState |= UISTATE_PUSHED;
				m_pManager->SendNotify(this, DUI_MSGTYPE_HEADERCLICK);
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 )
				m_pManager->SendNotify(this,DUI_MSGTYPE_HEADWIDTHCHANGED);

			if((m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				m_uButtonState &= ~UISTATE_CAPTURED;
				if( GetParent() ) 
					GetParent()->NeedParentUpdate();
			}
			else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
				m_uButtonState &= ~UISTATE_PUSHED;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				RECT rc = m_rcItem;
				if( m_iSepWidth >= 0 ) {
					rc.right -= ptLastMouse.x - event.ptMouse.x;
				}
				else {
					rc.left -= ptLastMouse.x - event.ptMouse.x;
				}

				if( rc.right - rc.left > GetMinWidth() ) {
					m_cxyFixed.cx = rc.right - rc.left;
					ptLastMouse = event.ptMouse;
					if( GetParent() ) 
						GetParent()->NeedParentUpdate();
				}
			}
			return;
		}
		if( event.Type == UIEVENT_SETCURSOR )
		{
			RECT rcSeparator = GetThumbRect();
			if (m_iSepWidth>=0)//111024 by cddjr, ���ӷָ������򣬷����û��϶�
				rcSeparator.left-=4;
			else
				rcSeparator.right+=4;
			if( IsEnabled() && m_bDragable && ::PtInRect(&rcSeparator, event.ptMouse) ) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
				return;
			}
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		CControlUI::DoEvent(event);
	}

	SIZE CListHeaderItemUI::EstimateSize(SIZE szAvailable)
	{
		if( m_cxyFixed.cy == 0 ) return CSize(m_cxyFixed.cx, m_pManager->GetDefaultFontInfo()->tm.tmHeight + 14);
		return CControlUI::EstimateSize(szAvailable);
	}

	RECT CListHeaderItemUI::GetThumbRect() const
	{
		if( m_iSepWidth >= 0 ) return CDuiRect(m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
		else return CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom);
	}

	void CListHeaderItemUI::PaintStatusImage(HDC hDC)
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;

		if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
			if( m_sPushedImage.IsEmpty() && !m_sNormalImage.IsEmpty() ) DrawImage(hDC, (LPCTSTR)m_sNormalImage);
			if( !DrawImage(hDC, (LPCTSTR)m_sPushedImage) ) m_sPushedImage.Empty();
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( m_sHotImage.IsEmpty() && !m_sNormalImage.IsEmpty() ) DrawImage(hDC, (LPCTSTR)m_sNormalImage);
			if( !DrawImage(hDC, (LPCTSTR)m_sHotImage) ) m_sHotImage.Empty();
		}
		else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
			if( m_sFocusedImage.IsEmpty() && !m_sNormalImage.IsEmpty() ) DrawImage(hDC, (LPCTSTR)m_sNormalImage);
			if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
		}
		else {
			if( !m_sNormalImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sNormalImage) ) m_sNormalImage.Empty();
			}
		}

		if( !m_sSepImage.IsEmpty() ) {
			RECT rcThumb = GetThumbRect();
			rcThumb.left -= m_rcItem.left;
			rcThumb.top -= m_rcItem.top;
			rcThumb.right -= m_rcItem.left;
			rcThumb.bottom -= m_rcItem.top;

			m_sSepImageModify.Empty();
			m_sSepImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
			if( !DrawImage(hDC, (LPCTSTR)m_sSepImage, (LPCTSTR)m_sSepImageModify) ) m_sSepImage.Empty();
		}
	}

	void CListHeaderItemUI::PaintText(HDC hDC)
	{
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();

		RECT rcText = m_rcItem;
		rcText.left += m_rcTextPadding.left;
		rcText.top += m_rcTextPadding.top;
		rcText.right -= m_rcTextPadding.right;
		rcText.bottom -= m_rcTextPadding.bottom;

		if( m_sText.IsEmpty() ) return;
		int nLinks = 0;
		if( m_bShowHtml )
			CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_sText, m_dwTextColor, \
			NULL, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
		else
			CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, m_dwTextColor, \
			m_iFont, DT_SINGLELINE | m_uTextStyle);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CListElementUI::CListElementUI() : 
		m_iIndex(-1),
		m_pOwner(NULL), 
		m_bSelected(false),
		m_uButtonState(0)
	{
	}

	LPCTSTR CListElementUI::GetClass() const
	{
		return _T("ListElementUI");
	}

	UINT CListElementUI::GetControlFlags() const
	{
		return UIFLAG_WANTRETURN;
	}

	LPVOID CListElementUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_LISTITEM) == 0 ) return static_cast<IListItemUI*>(this);
		if( _tcscmp(pstrName, DUI_CTR_LISTELEMENT) == 0 ) return static_cast<CListElementUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	IListOwnerUI* CListElementUI::GetOwner()
	{
		return m_pOwner;
	}

	void CListElementUI::SetOwner(CControlUI* pOwner)
	{
		m_pOwner = static_cast<IListOwnerUI*>(pOwner->GetInterface(_T("IListOwner")));
	}

	void CListElementUI::SetVisible(bool bVisible)
	{
		CControlUI::SetVisible(bVisible);
		if( !IsVisible() && m_bSelected)
		{
			m_bSelected = false;
			if( m_pOwner != NULL ) m_pOwner->SelectItem(-1);
		}
	}

	void CListElementUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	int CListElementUI::GetIndex() const
	{
		return m_iIndex;
	}

	void CListElementUI::SetIndex(int iIndex)
	{
		m_iIndex = iIndex;
	}

	void CListElementUI::Invalidate()
	{
		if( !IsVisible() ) return;

		if( GetParent() ) {
			CContainerUI* pParentContainer = static_cast<CContainerUI*>(GetParent()->GetInterface(_T("Container")));
			if( pParentContainer ) {
				RECT rc = pParentContainer->GetPos();
				RECT rcInset = pParentContainer->GetInset();
				rc.left += rcInset.left;
				rc.top += rcInset.top;
				rc.right -= rcInset.right;
				rc.bottom -= rcInset.bottom;
				CScrollBarUI* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
				if( pVerticalScrollBar && pVerticalScrollBar->IsVisible() ) rc.right -= pVerticalScrollBar->GetFixedWidth();
				CScrollBarUI* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
				if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

				RECT invalidateRc = m_rcItem;
				if( !::IntersectRect(&invalidateRc, &m_rcItem, &rc) ) 
				{
					return;
				}

				CControlUI* pParent = GetParent();
				RECT rcTemp;
				RECT rcParent;
				while( pParent = pParent->GetParent() )
				{
					rcTemp = invalidateRc;
					rcParent = pParent->GetPos();
					if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
					{
						return;
					}
				}

				if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
			}
			else {
				CControlUI::Invalidate();
			}
		}
		else {
			CControlUI::Invalidate();
		}
	}

	bool CListElementUI::Activate()
	{
		if( !CControlUI::Activate() ) return false;
		if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMACTIVATE);
		return true;
	}

	bool CListElementUI::IsSelected() const
	{
		return m_bSelected;
	}

	bool CListElementUI::Select(bool bSelect)
	{
		if( !IsEnabled() ) return false;
		if( bSelect == m_bSelected ) return true;
		m_bSelected = bSelect;
		if( bSelect && m_pOwner != NULL ) m_pOwner->SelectItem(m_iIndex);
		Invalidate();

		return true;
	}

	bool CListElementUI::IsExpanded() const
	{
		return false;
	}

	bool CListElementUI::Expand(bool /*bExpand = true*/)
	{
		return false;
	}

	void CListElementUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_DBLCLICK )
		{
			if( IsEnabled() ) {
				Activate();
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_KEYDOWN && IsEnabled() )
		{
			if( event.chKey == VK_RETURN ) {
				Activate();
				Invalidate();
				return;
			}
		}
		// An important twist: The list-item will send the event not to its immediate
		// parent but to the "attached" list. A list may actually embed several components
		// in its path to the item, but key-presses etc. needs to go to the actual list.
		if( m_pOwner != NULL ) m_pOwner->DoEvent(event); else CControlUI::DoEvent(event);
	}

	void CListElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("selected")) == 0 ) Select();
		else CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CListElementUI::DrawItemBk(HDC hDC, const RECT& rcItem)
	{
		ASSERT(m_pOwner);
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		DWORD iBackColor = 0;
		if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) iBackColor = pInfo->dwBkColor;
		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			iBackColor = pInfo->dwHotBkColor;
		}
		if( IsSelected() ) {
			iBackColor = pInfo->dwSelectedBkColor;
		}
		if( !IsEnabled() ) {
			iBackColor = pInfo->dwDisabledBkColor;
		}

		if ( iBackColor != 0 ) {
			CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(iBackColor));
		}

		if( !IsEnabled() ) {
			if( !pInfo->sDisabledImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)pInfo->sDisabledImage) ) pInfo->sDisabledImage.Empty();
				else return;
			}
		}
		if( IsSelected() ) {
			if( !pInfo->sSelectedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)pInfo->sSelectedImage) ) pInfo->sSelectedImage.Empty();
				else return;
			}
		}
		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( !pInfo->sHotImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)pInfo->sHotImage) ) pInfo->sHotImage.Empty();
				else return;
			}
		}

		if( !m_sBkImage.IsEmpty() ) {
			if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sBkImage) ) m_sBkImage.Empty();
			}
		}

		if( m_sBkImage.IsEmpty() ) {
			if( !pInfo->sBkImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)pInfo->sBkImage) ) pInfo->sBkImage.Empty();
				else return;
			}
		}

		if ( pInfo->dwLineColor != 0 && pInfo->bShowHLine ) {
			RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 1, m_rcItem.right, m_rcItem.bottom - 1 };
			CRenderEngine::DrawLine(hDC, rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CListLabelElementUI::CListLabelElementUI()
	{
	}

	LPCTSTR CListLabelElementUI::GetClass() const
	{
		return _T("ListLabelElementUI");
	}

	LPVOID CListLabelElementUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_LISTLABELELEMENT) == 0 ) return static_cast<CListLabelElementUI*>(this);
		return CListElementUI::GetInterface(pstrName);
	}

	void CListLabelElementUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else CListElementUI::DoEvent(event);
			return;
		}		
		if( event.Type == UIEVENT_MOUSEMOVE ) 
		{
			return;
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
		{
			if( IsEnabled() ){
				//           m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
				Select();
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP ) 
		{
			if( IsEnabled() ){
				m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( (m_uButtonState & UISTATE_HOT) != 0 ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		CListElementUI::DoEvent(event);
	}

	SIZE CListLabelElementUI::EstimateSize(SIZE szAvailable)
	{
		if( m_pOwner == NULL ) return CSize(0, 0);

		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		SIZE cXY = m_cxyFixed;
		if( cXY.cy == 0 && m_pManager != NULL ) {
			cXY.cy = m_pManager->GetFontInfo(pInfo->nFont)->tm.tmHeight + 8;
			cXY.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
		}

		if( cXY.cx == 0 && m_pManager != NULL ) {
			RECT rcText = { 0, 0, 9999, cXY.cy };
			if( pInfo->bShowHtml ) {
				int nLinks = 0;
				CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, NULL, NULL, nLinks, DT_SINGLELINE | DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
			}
			else {
				CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, pInfo->nFont, DT_SINGLELINE | DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
			}
			cXY.cx = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right;        
		}

		return cXY;
	}

	void CListLabelElementUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
		DrawItemBk(hDC, m_rcItem);
		DrawItemText(hDC, m_rcItem);
	}

	void CListLabelElementUI::DrawItemText(HDC hDC, const RECT& rcItem)
	{
		if( m_sText.IsEmpty() ) return;

		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		DWORD iTextColor = pInfo->dwTextColor;
		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			iTextColor = pInfo->dwHotTextColor;
		}
		if( IsSelected() ) {
			iTextColor = pInfo->dwSelectedTextColor;
		}
		if( !IsEnabled() ) {
			iTextColor = pInfo->dwDisabledTextColor;
		}
		int nLinks = 0;
		RECT rcText = rcItem;
		rcText.left += pInfo->rcTextPadding.left;
		rcText.right -= pInfo->rcTextPadding.right;
		rcText.top += pInfo->rcTextPadding.top;
		rcText.bottom -= pInfo->rcTextPadding.bottom;

		if( pInfo->bShowHtml )
			CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_sText, iTextColor, \
			NULL, NULL, nLinks, DT_SINGLELINE | pInfo->uTextStyle);
		else
			CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, iTextColor, \
			pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CListTextElementUI::CListTextElementUI() : m_nLinks(0), m_nHoverLink(-1), m_pOwner(NULL)
	{
		::ZeroMemory(&m_rcLinks, sizeof(m_rcLinks));
	}

	CListTextElementUI::~CListTextElementUI()
	{
		CDuiString* pText;
		for( int it = 0; it < m_aTexts.GetSize(); it++ ) {
			pText = static_cast<CDuiString*>(m_aTexts[it]);
			if( pText ) delete pText;
		}
		m_aTexts.Empty();
	}

	LPCTSTR CListTextElementUI::GetClass() const
	{
		return _T("ListTextElementUI");
	}

	LPVOID CListTextElementUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_LISTTEXTELEMENT) == 0 ) return static_cast<CListTextElementUI*>(this);
		return CListLabelElementUI::GetInterface(pstrName);
	}

	UINT CListTextElementUI::GetControlFlags() const
	{
		return UIFLAG_WANTRETURN | ( (IsEnabled() && m_nLinks > 0) ? UIFLAG_SETCURSOR : 0);
	}

	LPCTSTR CListTextElementUI::GetText(int iIndex) const
	{
		CDuiString* pText = static_cast<CDuiString*>(m_aTexts.GetAt(iIndex));
		if( pText ) return pText->GetData();
		return NULL;
	}

	void CListTextElementUI::SetText(int iIndex, LPCTSTR pstrText)
	{
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( m_aTexts.GetSize() < pInfo->nColumns ) { m_aTexts.Add(NULL); }

		CDuiString* pText = static_cast<CDuiString*>(m_aTexts[iIndex]);
		if( (pText == NULL && pstrText == NULL) || (pText && *pText == pstrText) ) return;

		if ( pText ) //by cddjr 2011/10/20
			pText->Assign(pstrText);
		else
			m_aTexts.SetAt(iIndex, new CDuiString(pstrText));
		Invalidate();
	}

	void CListTextElementUI::SetOwner(CControlUI* pOwner)
	{
		CListElementUI::SetOwner(pOwner);
		m_pOwner = static_cast<IListUI*>(pOwner->GetInterface(_T("IList")));
	}

	CDuiString* CListTextElementUI::GetLinkContent(int iIndex)
	{
		if( iIndex >= 0 && iIndex < m_nLinks ) return &m_sLinks[iIndex];
		return NULL;
	}

	void CListTextElementUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else CListLabelElementUI::DoEvent(event);
			return;
		}

		// When you hover over a link
		if( event.Type == UIEVENT_SETCURSOR ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
					return;
				}
			}      
		}
		if( event.Type == UIEVENT_BUTTONUP && IsEnabled() ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					m_pManager->SendNotify(this, DUI_MSGTYPE_LINK, i);
					return;
				}
			}
		}
		if( m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE ) {
			int nHoverLink = -1;
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					nHoverLink = i;
					break;
				}
			}

			if(m_nHoverLink != nHoverLink) {
				Invalidate();
				m_nHoverLink = nHoverLink;
			}
		}
		if( m_nLinks > 0 && event.Type == UIEVENT_MOUSELEAVE ) {
			if(m_nHoverLink != -1) {
				Invalidate();
				m_nHoverLink = -1;
			}
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
		{
			if( IsEnabled() ) {					
				//	m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);						
				if (m_pOwner)
				{
					if (m_pOwner->GetSingleSelect())
					{
						if (!IsSelected())
							m_pOwner->SelectItem(m_iIndex);
					}				
					else
					{
						if ((GetKeyState(VK_CONTROL) & 0x8000))
						{	
							if (IsSelected())
							{
								m_pOwner->UnSelectItem(m_iIndex);
							}
							else
							{
								m_pOwner->SelectMultiItem(m_iIndex);
							}
						}
						else
						{
							if ((m_pOwner->GetSelectItemCount() == 1 && IsSelected()))
							{								
							}						
							else
							{
								m_pOwner->SelectItem(m_iIndex);	
							}
						}					
					}
				}
				Invalidate();
			}
			m_pOwner->DoEvent(event);
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP ) 
		{
			if( IsEnabled() ){
				m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
			}
			return;
		}
		CListLabelElementUI::DoEvent(event);
	}

	SIZE CListTextElementUI::EstimateSize(SIZE szAvailable)
	{
		TListInfoUI* pInfo = NULL;
		if( m_pOwner ) pInfo = m_pOwner->GetListInfo();

		SIZE cXY = m_cxyFixed;
		if( cXY.cy == 0 && m_pManager != NULL ) {
			cXY.cy = m_pManager->GetFontInfo(pInfo->nFont)->tm.tmHeight + 8;
			if( pInfo ) cXY.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
		}

		return cXY;
	}

	void CListTextElementUI::DrawItemText(HDC hDC, const RECT& rcItem)
	{
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		DWORD iTextColor = pInfo->dwTextColor;

		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			iTextColor = pInfo->dwHotTextColor;
		}
		if( IsSelected() ) {
			iTextColor = pInfo->dwSelectedTextColor;
		}
		if( !IsEnabled() ) {
			iTextColor = pInfo->dwDisabledTextColor;
		}
		IListCallbackUI* pCallback = m_pOwner->GetTextCallback();
		//ASSERT(pCallback);
		//if( pCallback == NULL ) return;

		m_nLinks = 0;
		int vLineColumns = pInfo->nColumns - 1;
		int nLinks = lengthof(m_rcLinks);
		for( int i = 0; i < pInfo->nColumns; i++ )
		{
			RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.bottom };
			RECT rcItemLine = rcItem;
			rcItem.left += pInfo->rcTextPadding.left;
			rcItem.right -= pInfo->rcTextPadding.right;
			rcItem.top += pInfo->rcTextPadding.top;
			rcItem.bottom -= pInfo->rcTextPadding.bottom;

			CDuiString strText;//��ʹ��LPCTSTR����������̫�� by cddjr 2011/10/20
			if( pCallback ) strText = pCallback->GetItemText(this, m_iIndex, i);
			else strText.Assign(GetText(i));
			if( pInfo->bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, m_pManager, rcItem, strText.GetData(), iTextColor, \
				&m_rcLinks[m_nLinks], &m_sLinks[m_nLinks], nLinks, DT_SINGLELINE | pInfo->uTextStyle);
			else
				CRenderEngine::DrawText(hDC, m_pManager, rcItem, strText.GetData(), iTextColor, \
				pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);

			m_nLinks += nLinks;
			nLinks = lengthof(m_rcLinks) - m_nLinks; 

			if(pInfo->dwLineColor != 0 && pInfo->bShowVLine && i < vLineColumns){
				RECT nRc;
				nRc.left	= rcItemLine.right-1;
				nRc.top		= rcItemLine.top;
				nRc.right	= rcItemLine.right-1;
				nRc.bottom	= rcItemLine.bottom;

				CRenderEngine::DrawLine(hDC,nRc,1,pInfo->dwLineColor);
			}
		}
		for( int i = m_nLinks; i < lengthof(m_rcLinks); i++ ) {
			::ZeroMemory(m_rcLinks + i, sizeof(RECT));
			((CDuiString*)(m_sLinks + i))->Empty();
		}
	}

	bool CListTextElementUI::Select( bool bSelect /*= true*/ )
	{
		if( !IsEnabled() )
			return false;
		if( bSelect == m_bSelected )
			return true;

		m_bSelected = bSelect;
		Invalidate();

		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CListImageTextElementUI::CListImageTextElementUI() : m_aStartCheck(sizeof(bool)), m_aCheck(sizeof(bool))
	{
	}

	CListImageTextElementUI::~CListImageTextElementUI()
	{
		CDuiString* pText;

		for( int it = 0; it < m_aImgNor.GetSize(); it++ ) {
			pText = static_cast<CDuiString*>(m_aImgNor[it]);
			if( pText ) delete pText;
		}
		m_aImgNor.Empty();

		for( int it = 0; it < m_aImgSel.GetSize(); it++ ) {
			pText = static_cast<CDuiString*>(m_aImgSel[it]);
			if( pText ) delete pText;
		}
		m_aImgSel.Empty();

		for( int it = 0; it < m_aImgNorCheck.GetSize(); it++ ) {
			pText = static_cast<CDuiString*>(m_aImgNorCheck[it]);
			if( pText ) delete pText;
		}
		m_aImgNorCheck.Empty();

		for( int it = 0; it < m_aImgSelCheck.GetSize(); it++ ) {
			pText = static_cast<CDuiString*>(m_aImgSelCheck[it]);
			if( pText ) delete pText;
		}
		m_aImgSelCheck.Empty();
	}

	LPCTSTR CListImageTextElementUI::GetClass() const
	{
		return _T("ListImageTextElementUI");
	}

	LPVOID CListImageTextElementUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_LISTIMAGETEXTELEMENT) == 0 ) return static_cast<CListImageTextElementUI*>(this);
		return CListTextElementUI::GetInterface(pstrName);
	}

	LPCTSTR CListImageTextElementUI::GetNorImg(int iIndex) const
	{
		CDuiString* pText = static_cast<CDuiString*>(m_aImgNor.GetAt(iIndex));
		if( pText ) return pText->GetData();
		return NULL;
	}

	LPCTSTR CListImageTextElementUI::GetSelImg(int iIndex) const
	{
		CDuiString* pText = static_cast<CDuiString*>(m_aImgSel.GetAt(iIndex));
		if( pText ) return pText->GetData();
		return NULL;
	}

	void CListImageTextElementUI::SetNorImg(int iIndex, LPCTSTR pstrText)
	{
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( m_aImgNor.GetSize() < pInfo->nColumns ) { m_aImgNor.Add(NULL); }

		CDuiString* pText = static_cast<CDuiString*>(m_aImgNor[iIndex]);
		if( (pText == NULL && pstrText == NULL) || (pText && *pText == pstrText) ) return;

		if ( pText )
			pText->Assign(pstrText);
		else
			m_aImgNor.SetAt(iIndex, new CDuiString(pstrText));
		Invalidate();
	}

	void CListImageTextElementUI::SetSelImg(int iIndex, LPCTSTR pstrText)
	{
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( m_aImgSel.GetSize() < pInfo->nColumns ) { m_aImgSel.Add(NULL); }

		CDuiString* pText = static_cast<CDuiString*>(m_aImgSel[iIndex]);
		if( (pText == NULL && pstrText == NULL) || (pText && *pText == pstrText) ) return;

		if ( pText ) 
			pText->Assign(pstrText);
		else
			m_aImgSel.SetAt(iIndex, new CDuiString(pstrText));
		Invalidate();
	}

	void CListImageTextElementUI::SetCheckFlag(int iIndex, bool bStartCheck /*= TRUE*/)
	{
		if( m_pOwner == NULL ) return;
		bool bDefault = false;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( m_aStartCheck.GetSize() < pInfo->nColumns ) { m_aStartCheck.Add((LPVOID)(&bDefault)); }

		bool *pValue = (bool*)m_aStartCheck.GetAt(iIndex);	
		*pValue = bStartCheck;
	}

	bool CListImageTextElementUI::GetCheckFlag(int iIndex) const
	{
		bool *pValue = (bool*)m_aStartCheck.GetAt(iIndex);
		if (pValue) return *pValue;
		return false;
	}

	void CListImageTextElementUI::SetCheck(int iIndex, bool bCheck)
	{
		if( m_pOwner == NULL ) return;
		bool bDefault = false;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( m_aCheck.GetSize() < pInfo->nColumns ) { m_aCheck.Add((LPVOID)(&bDefault)); }

		bool *pValue = (bool*)m_aCheck.GetAt(iIndex);	
		*pValue = bCheck;
	}

	bool CListImageTextElementUI::GetCheck(int iIndex) const
	{
		bool *pValue = (bool*)m_aCheck.GetAt(iIndex);
		if (pValue) return *pValue;
		return false;
	}

	LPCTSTR CListImageTextElementUI::GetNorCheckImg(int iIndex) const
	{
		CDuiString* pText = static_cast<CDuiString*>(m_aImgNorCheck.GetAt(iIndex));
		if( pText ) return pText->GetData();
		return NULL;
	}

	LPCTSTR CListImageTextElementUI::GetSelCheckImg(int iIndex) const
	{
		CDuiString* pText = static_cast<CDuiString*>(m_aImgSelCheck.GetAt(iIndex));
		if( pText ) return pText->GetData();
		return NULL;
	}

	void CListImageTextElementUI::SetNorCheckImg(int iIndex, LPCTSTR pstrText)
	{
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( m_aImgNorCheck.GetSize() < pInfo->nColumns ) { m_aImgNorCheck.Add(NULL); }

		CDuiString* pText = static_cast<CDuiString*>(m_aImgNorCheck[iIndex]);
		if( (pText == NULL && pstrText == NULL) || (pText && *pText == pstrText) ) return;

		if ( pText )
			pText->Assign(pstrText);
		else
			m_aImgNorCheck.SetAt(iIndex, new CDuiString(pstrText));
		Invalidate();
	}

	void CListImageTextElementUI::SetSelCheckImg(int iIndex, LPCTSTR pstrText)
	{
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( m_aImgSelCheck.GetSize() < pInfo->nColumns ) { m_aImgSelCheck.Add(NULL); }

		CDuiString* pText = static_cast<CDuiString*>(m_aImgSelCheck[iIndex]);
		if( (pText == NULL && pstrText == NULL) || (pText && *pText == pstrText) ) return;

		if ( pText ) 
			pText->Assign(pstrText);
		else
			m_aImgSelCheck.SetAt(iIndex, new CDuiString(pstrText));
		Invalidate();
	}

	RECT CListImageTextElementUI::GetItemRect(int iIndex)
	{
		TListInfoUI* pInfo = m_pOwner->GetListInfo();		
		RECT rcRes = { pInfo->rcColumn[iIndex].left, m_rcItem.top, pInfo->rcColumn[iIndex].right, m_rcItem.bottom };
		return rcRes;
	}

	RECT CListImageTextElementUI::GetItemRectPadding(int iIndex)
	{	
		TListInfoUI* pInfo = m_pOwner->GetListInfo();	
		RECT rcRes = { pInfo->rcColumn[iIndex].left, m_rcItem.top, pInfo->rcColumn[iIndex].right, m_rcItem.bottom };
		rcRes.left += pInfo->rcTextPadding.left;
		rcRes.right -= pInfo->rcTextPadding.right;
		rcRes.top += pInfo->rcTextPadding.top;
		rcRes.bottom -= pInfo->rcTextPadding.bottom;
		return rcRes;
	}

	RECT CListImageTextElementUI::GetItemCheckRect(int iIndex)
	{
		RECT rcItemP = GetItemRectPadding(iIndex);
		RECT rcRes = {0, 0, 0, 0};
		if (IsRectEmpty(&rcItemP) || !GetCheckFlag(iIndex))
			return rcRes;	
		TListInfoUI* pInfo = m_pOwner->GetListInfo();	
		rcRes = rcItemP;	
		rcRes.right = rcRes.left + pInfo->szCheckImg.cx;
		if (rcRes.right > rcItemP.right)
			rcRes.right = rcItemP.right;	
		if (rcRes.left > rcRes.right)
			rcRes.left = rcRes.right;
		rcRes.top = rcRes.top + (rcItemP.bottom - rcItemP.top - pInfo->szCheckImg.cy) / 2;
		rcRes.bottom = rcRes.top + pInfo->szCheckImg.cy;
		if (rcRes.bottom > rcItemP.bottom)
			rcRes.bottom = rcItemP.bottom;
		if (rcRes.top > rcRes.bottom)
			rcRes.bottom = rcRes.top;

		return rcRes;
	}

	RECT CListImageTextElementUI::GetItemImgRect(int iIndex)
	{
		RECT rcItemP = GetItemRectPadding(iIndex);	
		RECT rcRes = {0, 0, 0, 0};
		if (IsRectEmpty(&rcItemP) || (!GetNorImg(iIndex) && !GetSelImg(iIndex)))
			return rcRes;	
		TListInfoUI* pInfo = m_pOwner->GetListInfo();	
		RECT rcItemCheck = GetItemCheckRect(iIndex);	
		rcRes = rcItemP;	
		if (!IsRectEmpty(&rcItemCheck))
		{
			rcRes.left = rcItemCheck.right + 2;
		}	
		rcRes.right = rcRes.left + pInfo->szIconImg.cx;
		if (rcRes.right > rcItemP.right)
			rcRes.right = rcItemP.right;	
		if (rcRes.left > rcRes.right)
			rcRes.left = rcRes.right;
		rcRes.top = rcRes.top + (rcItemP.bottom - rcItemP.top - pInfo->szIconImg.cy) / 2;
		rcRes.bottom = rcRes.top + pInfo->szIconImg.cy;
		if (rcRes.bottom > rcItemP.bottom)
			rcRes.bottom = rcItemP.bottom;
		if (rcRes.top > rcRes.bottom)
			rcRes.bottom = rcRes.top;

		return rcRes;
	}

	RECT CListImageTextElementUI::GetItemTextRect(int iIndex)
	{
		RECT rcRes = GetItemRectPadding(iIndex);
		RECT rcImg = GetItemImgRect(iIndex);
		RECT rcCheck =GetItemCheckRect(iIndex);
		if (!IsRectEmpty(&rcImg))
		{
			rcRes.left = rcImg.right + 2;
			if (rcRes.left > rcRes.right)
				rcRes.left = rcRes.right;
		}
		else
		{
			if (!IsRectEmpty(&rcCheck))
			{
				rcRes.left = rcCheck.right + 2;
				if (rcRes.left > rcRes.right)
					rcRes.left = rcRes.right;
			}
		}
		return rcRes;
	}

	int CListImageTextElementUI::HitTest(POINT pt, UINT* pFlags/* = NULL*/)
	{	
		if( m_pOwner == NULL ) return -1;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();	
		for( int i = 0; i < pInfo->nColumns; i++ )
		{
			RECT rc = GetItemRect(i);;		
			if (PtInRect(&rc, pt))
			{
				if (!pFlags)
					return i;
				*pFlags = 0;
				*pFlags |= ULVHT_ITEM;			
				rc = GetItemCheckRect(i);
				if (PtInRect(&rc, pt))
				{
					*pFlags |= ULVHT_CHECKBOX;
				}

				return i;
			}
		}

		return -1;
	}

	void CListImageTextElementUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else CListTextElementUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_BUTTONDOWN ) {
			SetFocus();

			if (IsEnabled())
			{
				UINT flags = 0;
				int iIndex = HitTest(event.ptMouse, &flags);
				if (flags & ULVHT_CHECKBOX && GetCheckFlag(iIndex))
				{			
					SetCheck(iIndex, !GetCheck(iIndex));
					Invalidate();
					m_pManager->SendNotify(this, _T("list_check_click"), m_iIndex, iIndex);
				}
			}
		}

		if( event.Type == UIEVENT_DBLCLICK )
		{
			if( IsEnabled() ) {		
				UINT flags = 0;
				int iIndex = HitTest(event.ptMouse, &flags);

				Invalidate();
				m_pManager->SendNotify(this, _T("list_item_dbclick"), m_iIndex, iIndex);
				return;
			}				
		}

		CListTextElementUI::DoEvent(event);
	}

	void CListImageTextElementUI::DrawItemText(HDC hDC, const RECT& rcItem)
	{
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		DWORD iTextColor = pInfo->dwTextColor;

		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			iTextColor = pInfo->dwHotTextColor;
		}
		if( IsSelected() ) {
			iTextColor = pInfo->dwSelectedTextColor;
		}
		if( !IsEnabled() ) {
			iTextColor = pInfo->dwDisabledTextColor;
		}

		int vLineColumns = pInfo->nColumns -1;
		IListCallbackUI* pCallback = m_pOwner->GetTextCallback();
		for( int i = 0; i < pInfo->nColumns; i++ )
		{
			LPCTSTR pImg;		
			if (GetCheckFlag(i))
			{
				RECT rcCheck = GetItemCheckRect(i);
				pImg = GetCheck(i) ? GetSelCheckImg(i) : GetNorCheckImg(i);
				if (pImg)
					CRenderEngine::DrawImageString(hDC, m_pManager, rcCheck, m_rcPaint, pImg, NULL);
			}

			RECT rcImg;
			rcImg = GetItemImgRect(i);		
			pImg = IsSelected() ? GetSelImg(i) : GetNorImg(i);
			if (pImg)
				CRenderEngine::DrawImageString(hDC, m_pManager, rcImg, m_rcPaint, pImg, NULL);

			RECT rcText = GetItemTextRect(i);
			RECT rcVLine = rcText;
			CDuiString strText;
			if( pCallback ) strText = pCallback->GetItemText(this, m_iIndex, i);
			else strText.Assign(GetText(i));
			if( pInfo->bShowHtml )
			{
				int nLinks = 0;
				CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, strText.GetData(), iTextColor, \
					NULL, NULL, nLinks, DT_SINGLELINE | pInfo->uTextStyle);
			}
			else
				CRenderEngine::DrawText(hDC, m_pManager, rcText, strText.GetData(), iTextColor, \
				pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);

			if(pInfo->dwLineColor != 0 && pInfo->bShowVLine && i < vLineColumns){
				RECT nRc;
				nRc.left	= rcVLine.right-1 + pInfo->rcTextPadding.right;
				nRc.top		= rcVLine.top - pInfo->rcTextPadding.top;
				nRc.right	= nRc.left;
				nRc.bottom	= rcVLine.bottom + pInfo->rcTextPadding.top;

				CRenderEngine::DrawLine(hDC,nRc,1,pInfo->dwLineColor);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	CListContainerElementUI::CListContainerElementUI() : 
		m_iIndex(-1),
		m_pOwner(NULL), 
		m_bSelected(false),
		m_uButtonState(0)
	{
	}

	LPCTSTR CListContainerElementUI::GetClass() const
	{
		return _T("ListContainerElementUI");
	}

	UINT CListContainerElementUI::GetControlFlags() const
	{
		return UIFLAG_WANTRETURN;
	}

	LPVOID CListContainerElementUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_LISTITEM) == 0 ) return static_cast<IListItemUI*>(this);
		if( _tcscmp(pstrName, DUI_CTR_LISTCONTAINERELEMENT) == 0 ) return static_cast<CListContainerElementUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	IListOwnerUI* CListContainerElementUI::GetOwner()
	{
		return m_pOwner;
	}

	void CListContainerElementUI::SetOwner(CControlUI* pOwner)
	{
		m_pOwner = static_cast<IListOwnerUI*>(pOwner->GetInterface(_T("IListOwner")));
	}

	void CListContainerElementUI::SetVisible(bool bVisible)
	{
		CContainerUI::SetVisible(bVisible);
		if( !IsVisible() && m_bSelected)
		{
			m_bSelected = false;
			if( m_pOwner != NULL ) m_pOwner->SelectItem(-1);
		}
	}

	void CListContainerElementUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	int CListContainerElementUI::GetIndex() const
	{
		return m_iIndex;
	}

	void CListContainerElementUI::SetIndex(int iIndex)
	{
		m_iIndex = iIndex;
	}

	void CListContainerElementUI::Invalidate()
	{
		if( !IsVisible() ) return;

		if( GetParent() ) {
			CContainerUI* pParentContainer = static_cast<CContainerUI*>(GetParent()->GetInterface(_T("Container")));
			if( pParentContainer ) {
				RECT rc = pParentContainer->GetPos();
				RECT rcInset = pParentContainer->GetInset();
				rc.left += rcInset.left;
				rc.top += rcInset.top;
				rc.right -= rcInset.right;
				rc.bottom -= rcInset.bottom;
				CScrollBarUI* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
				if( pVerticalScrollBar && pVerticalScrollBar->IsVisible() ) rc.right -= pVerticalScrollBar->GetFixedWidth();
				CScrollBarUI* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
				if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

				RECT invalidateRc = m_rcItem;
				if( !::IntersectRect(&invalidateRc, &m_rcItem, &rc) ) 
				{
					return;
				}

				CControlUI* pParent = GetParent();
				RECT rcTemp;
				RECT rcParent;
				while( pParent = pParent->GetParent() )
				{
					rcTemp = invalidateRc;
					rcParent = pParent->GetPos();
					if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
					{
						return;
					}
				}

				if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
			}
			else {
				CContainerUI::Invalidate();
			}
		}
		else {
			CContainerUI::Invalidate();
		}
	}

	bool CListContainerElementUI::Activate()
	{
		if( !CContainerUI::Activate() ) return false;
		if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMACTIVATE);
		return true;
	}

	bool CListContainerElementUI::IsSelected() const
	{
		return m_bSelected;
	}

	bool CListContainerElementUI::Select(bool bSelect)
	{
		if( !IsEnabled() ) return false;
		if( bSelect == m_bSelected ) return true;
		m_bSelected = bSelect;
		if( bSelect && m_pOwner != NULL ) m_pOwner->SelectItem(m_iIndex);
		Invalidate();

		return true;
	}

	bool CListContainerElementUI::IsExpanded() const
	{
		return false;
	}

	bool CListContainerElementUI::Expand(bool /*bExpand = true*/)
	{
		return false;
	}

	void CListContainerElementUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else CContainerUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_DBLCLICK )
		{
			if( IsEnabled() ) {
				Activate();
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_KEYDOWN && IsEnabled() )
		{
			if( event.chKey == VK_RETURN ) {
				Activate();
				Invalidate();
				return;
			}
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
		{
			if( IsEnabled() ){
				Select();
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP ) 
		{
			if( IsEnabled() ){
				m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( (m_uButtonState & UISTATE_HOT) != 0 ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_TIMER )
		{
			m_pManager->SendNotify(this, DUI_MSGTYPE_TIMER, event.wParam, event.lParam);
			return;
		}
		if( event.Type == UIEVENT_TIMER )
		{
			m_pManager->SendNotify(this, DUI_MSGTYPE_TIMER, event.wParam, event.lParam);
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			if( IsContextMenuUsed() ) {
				m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
				return;
			}
		}

		// An important twist: The list-item will send the event not to its immediate
		// parent but to the "attached" list. A list may actually embed several components
		// in its path to the item, but key-presses etc. needs to go to the actual list.
		if( m_pOwner != NULL ) m_pOwner->DoEvent(event); else CControlUI::DoEvent(event);
	}

	void CListContainerElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("selected")) == 0 ) Select();
		else CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CListContainerElementUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
		DrawItemBk(hDC, m_rcItem);
		CContainerUI::DoPaint(hDC, rcPaint);
	}

	void CListContainerElementUI::DrawItemText(HDC hDC, const RECT& rcItem)
	{
		return;
	}

	void CListContainerElementUI::DrawItemBk(HDC hDC, const RECT& rcItem)
	{
		ASSERT(m_pOwner);
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		DWORD iBackColor = 0;
		if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) iBackColor = pInfo->dwBkColor;

		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			iBackColor = pInfo->dwHotBkColor;
		}
		if( IsSelected() ) {
			iBackColor = pInfo->dwSelectedBkColor;
		}
		if( !IsEnabled() ) {
			iBackColor = pInfo->dwDisabledBkColor;
		}
		if ( iBackColor != 0 ) {
			CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(iBackColor));
		}

		if( !IsEnabled() ) {
			if( !pInfo->sDisabledImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)pInfo->sDisabledImage) ) pInfo->sDisabledImage.Empty();
				else return;
			}
		}
		if( IsSelected() ) {
			if( !pInfo->sSelectedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)pInfo->sSelectedImage) ) pInfo->sSelectedImage.Empty();
				else return;
			}
		}
		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( !pInfo->sHotImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)pInfo->sHotImage) ) pInfo->sHotImage.Empty();
				else return;
			}
		}
		if( !m_sBkImage.IsEmpty() ) {
			if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sBkImage) ) m_sBkImage.Empty();
			}
		}

		if( m_sBkImage.IsEmpty() ) {
			if( !pInfo->sBkImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)pInfo->sBkImage) ) pInfo->sBkImage.Empty();
				else return;
			}
		}

		if ( pInfo->dwLineColor != 0 && pInfo->bShowHLine ) {
			RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 1, m_rcItem.right, m_rcItem.bottom - 1 };
			CRenderEngine::DrawLine(hDC, rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
		}
	}

	void CListContainerElementUI::SetPos(RECT rc)
	{	
		CHorizontalLayoutUI::SetPos(rc);
		if( m_pOwner == NULL ) return;		

		CListUI* pList = static_cast<CListUI*>(m_pOwner);
		if (pList == NULL ||  _tcscmp(_T("ListUI"), pList->GetClass()) != 0 ) return;

		CListHeaderUI *pHeader = pList->GetHeader();
		if (pHeader == NULL || !pHeader->IsVisible())
			return;

		int nCount = m_items.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			CControlUI *pListItem = static_cast<CControlUI*>(m_items[i]);

			CControlUI *pHeaderItem = pHeader->GetItemAt(i);
			if (pHeaderItem == NULL)
				return;

			RECT rcHeaderItem = pHeaderItem->GetPos();
			if (pListItem != NULL && !(rcHeaderItem.left ==0 && rcHeaderItem.right ==0) )
			{
				RECT rt = pListItem->GetPos();
				rt.left =rcHeaderItem.left;
				rt.right = rcHeaderItem.right;
				pListItem->SetPos(rt);
			}

		}
	}
} // namespace UiLib
