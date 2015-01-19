#include "..\duilib.h"

///////////////////////////////////////////////////////////////////////
//
//
CHotButtonUI::CHotButtonUI()
{

}
LPCTSTR CHotButtonUI::GetClass() const
{
	return kHotButtonClassName;
}

LPVOID CHotButtonUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, kHotButtonInterface) == 0 ) return static_cast<CHotButtonUI*>(this);
	return CButtonUI::GetInterface(pstrName);
}

void CHotButtonUI::DoEvent(TEventUI& event)
{
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pParent != NULL ) m_pParent->DoEvent(event);
		else CButtonUI::DoEvent(event);
		return;
	}

	if( event.Type == UIEVENT_MOUSEENTER )
	{
		if( IsEnabled() ) {
			m_uButtonState |= UISTATE_HOT;
			m_pManager->SendNotify(this, DUI_MSGTYPE_BUTTON_HOT, event.wParam, event.lParam);
			Invalidate();
		}
		return;
	}

	CButtonUI::DoEvent(event);
}
