#include "..\duilib.h"


///////////////////////////////////////////////////////////////////////
//
//
CRadioListItemUI::CRadioListItemUI():
	m_pRadioListTab(NULL),
	m_pRadioListHotTab(NULL),
	bActivated(false)
{

}
LPCTSTR CRadioListItemUI::GetClass() const
{
	return _T("TreeNodeUI");
}

LPVOID CRadioListItemUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, kRadioListItemUIInferFace) == 0 ) return static_cast<CRadioListItemUI*>(this);
	return CTreeNodeUI::GetInterface(pstrName);
}

bool CRadioListItemUI::Activate()
{
	if( !CContainerUI::Activate() ) return false;
	if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMACTIVATE);
	
	static_cast<CMusicListUI*>(GetOwner())->SetRadioPlayingItem(GetIndex());
	bActivated = true;
	return true;
}

void CRadioListItemUI::SetPlaying(bool bPlay /*= true*/)
{
	if (m_pRadioListTab == NULL)
		m_pRadioListTab = static_cast<CTabLayoutUI*>(m_pManager->FindSubControlByName(this, kRadioTabControlName));
	if (m_pRadioListTab == NULL)
		return;
	
	if (bPlay)
	{		
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		m_pRadioListTab->SelectItem(1);
		SetFixedHeight(kRadioListItemActivatedHeight);
		SetBkImage(pInfo->sDisabledImage);
		m_pRadioListTab->SetFixedHeight(kRadioListItemActivatedHeight);
	} 
	else
	{
		m_pRadioListTab->SelectItem(0);
		SetFixedHeight(kRadioListItemNormalHeight);
		SetBkImage(_T(""));
		m_pRadioListTab->SetFixedHeight(kRadioListItemNormalHeight);
	}
}

bool CRadioListItemUI::Hot(bool bHot)
{

	if( !IsEnabled()) return false;
	if (m_pRadioListHotTab == NULL)
		m_pRadioListHotTab = static_cast<CHorizontalLayoutUI*>(m_pManager->FindSubControlByName(this, kRadioHotTabControlName));

	if ( m_pRadioListHotTab != NULL )
	{
		if (bHot)
			m_pRadioListHotTab->SetVisible(true);
		else
			m_pRadioListHotTab->SetVisible(false);
	}
	Invalidate();
	return true;
}

void CRadioListItemUI::DoEvent(TEventUI& event)
{

		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Hot();
				Invalidate();
			}
			return;
		}
		else if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( (m_uButtonState & UISTATE_HOT) != 0 ) {
				m_uButtonState &= ~UISTATE_HOT;
				Hot(false);
				Invalidate();
			}
			return;
		}	
		else if( event.Type == UIEVENT_CONTEXTMENU )
		{
			if( IsContextMenuUsed() ) {
				m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
			}
		}
		else
			CTreeNodeUI::DoEvent(event);
}