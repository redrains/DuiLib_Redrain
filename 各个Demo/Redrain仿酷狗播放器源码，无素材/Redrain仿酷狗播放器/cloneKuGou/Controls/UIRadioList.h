#ifndef UIRADIO_LIST_H
#define UIRADIO_LIST_H


const int kRadioListItemNormalHeight =		35;
const int kRadioListItemActivatedHeight =	100;

const TCHAR kRadioTabControlName[] =	_T("RadioListTab");

//const TCHAR kLogoButtonControlName[] =		_T("logo");  已经在UIMusicList.h定义
const TCHAR kSmallLogoButtonControlName[] =	_T("logo_small");

const TCHAR kRadioNameControlName[] =		_T("RadioName");
//const TCHAR kMusicNameControlName[] =		_T("MusicName"); 已经在UIMusicList.h定义
const TCHAR kRadioHotTabControlName[] =		_T("PlayListOperation1");

const TCHAR kMusicClassControlName[] =		_T("MusicClass");
//const TCHAR kMusicName2ControlName[] =		_T("MusicName2"); 已经在UIMusicList.h定义
const TCHAR kAudienceNumControlName[] =		_T("AudienceNum");


const TCHAR kRadioListItemUIClassName[] =	_T("RadioListItemUI");
const TCHAR kRadioListItemUIInferFace[]=	_T("RadioListItem");

struct RadioListItemInfo
{
	CDuiString logo;
	CDuiString logo_small;
	CDuiString music_name;
	CDuiString radio_name;
	CDuiString audience_num;
};

///////////////////////////////////////////////////////////////////////
//
//
class CRadioListItemUI : public CTreeNodeUI
{
public:
	CRadioListItemUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetPlaying(bool bPlay = true);
	bool Hot(bool bHot = true);

	bool Activate();
	void DoEvent(TEventUI& event);	

private:
	CTabLayoutUI *m_pRadioListTab;
	CHorizontalLayoutUI *m_pRadioListHotTab;
	bool		 bActivated;
};

#endif // UIRADIO_LIST_H