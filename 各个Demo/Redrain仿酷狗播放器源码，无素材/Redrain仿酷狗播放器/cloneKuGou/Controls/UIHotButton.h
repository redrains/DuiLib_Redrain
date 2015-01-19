#ifndef UI_HOT_BUTTON_H
#define UI_HOT_BUTTON_H

const TCHAR kHotButtonClassName[] =	_T("HotButtonUI");
const TCHAR kHotButtonInterface[] =	_T("HotButton");

#define DUI_MSGTYPE_BUTTON_HOT       (_T("buttonhot"))

class CHotButtonUI : public CButtonUI
{
public:
	CHotButtonUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void DoEvent(TEventUI& event);
};

#endif // UI_HOT_BUTTON_H