#ifndef UIMUSIC_LIST_H
#define UIMUSIC_LIST_H


const int kMusicListItemNormalHeight =		28;
const int kMusicListItemActivatedHeight =	53;

const TCHAR kLogoButtonControlName[] =		_T("logo");
const TCHAR kMusicNameControlName[] =		_T("MusicName");
const TCHAR kMusicName2ControlName[] =		_T("MusicName2");
const TCHAR kMusicIndexControlName[] =		_T("MusicIndex");
const TCHAR kPlayControlControlName[] =		_T("PlayListPlay");
const TCHAR kMusicLengthControlName[] =		_T("MusicLength");
const TCHAR kMusicCurPosControlName[] =		_T("MusicCurPos");
const TCHAR kListItemControlControlName[] =	_T("PlayListExternOperation");
const TCHAR kMusicTabControlName[] =		_T("PlayListTab");
const TCHAR kMusicHotTabControlName[] =		_T("PlayListControlButtonTab");

const TCHAR kMusicListUIClassName[] =		_T("MusicListUI");
const TCHAR kMusicListUIInterFace[] =		_T("MusicList");

const TCHAR kMusicListItemUIClassName[] =	_T("MusicListItemUI");
const TCHAR kMusicListItemUIInferFace[]=	_T("MusicListItem");

const TCHAR kMusicListGroupUIClassName[] =	_T("MusicListGroupUI");
const TCHAR kMusicListGroupUIInferFace[]=	_T("MusicListGroup");


///////////////////////////////////////////////////////////////////////
//
//
class CMusicInfoDialog;
class CMusicListItemUI : public CTreeNodeUI
{
public:
	CMusicListItemUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetPlaying(bool bPlay = true);
	bool Hot(bool bHot = true);

	MEDIA* GetMedia();

	bool Activate();
	void DoEvent(TEventUI& event);	
	void DrawItemBk(HDC hDC, const RECT& rcItem);
private:
	CTabLayoutUI *m_pMusicListTab;
	CTabLayoutUI *m_pMusicListHotTab;
	bool		 bActivated;
	CMusicInfoDialog *m_pMusicInfoDlg;
};

///////////////////////////////////////////////////////////////////////
//
//
class CMusicListUI;
class CMusicListGroupUI : public CTreeNodeUI
{
public:
	CMusicListGroupUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	bool Add(CMusicListItemUI* pControl, bool bAutoRefreshIndex = true);
	bool AddAt(CMusicListItemUI* pControl, int iIndex, bool bAutoRefreshIndex = true);
	bool RemoveAt(CMusicListItemUI* _pTreeNodeUI, bool bAutoRefreshIndex = true);

	void SetItemText( LPCTSTR pstrValue );
	bool AdjustIndexs(int nStartPos);

	void SetMusicList(CMusicListUI* pMusicList);
	CMusicListUI* GetMusicList();

	CStdStringPtrMap *GetGroupItemHash();
	bool HasMusic(LPCTSTR szMusicPath);

	void DoEvent(TEventUI& event);

private:
	CDuiString m_GroupName;
	CStdStringPtrMap m_mGroupItemHash;
	CMusicListUI *m_pMusicList;
};
///////////////////////////////////////////////////////////////////////
//
//
class CMusicListUI : public CTreeViewUI, public IDialogBuilderCallback
{
public:
	enum {SCROLL_TIMERID = 10};

	CMusicListUI(CPaintManagerUI& paint_manager);

	~CMusicListUI();
	double CalculateDelay(double state);
	void DoEvent(TEventUI& event);
//	void CreateMusicInfoDialog();  为音乐列表添加信息弹出窗体，暂时弃用，改为为每个音乐项目添加信息弹出窗体
//	CMusicInfoDialog* GetMusicInfoDialog();

	bool AddGroup(LPCTSTR szGroupName, int nIndex = -1);
	CMusicListGroupUI *GetGroup(LPCTSTR szGroupName);
	CStdStringPtrMap *GetGroupItemHash(LPCTSTR szGroupName);

//	bool AddMusic(LPCTSTR szGroupName, const MusicListItemInfo& item, bool bAutoRefreshIndex = true , int nIndex = -1); 早期的函数，弃用
	bool AddMusic(LPCTSTR szGroupName, MEDIA* item, bool bAutoRefreshIndex = true , int nIndex = -1);
	bool DeleteMusic(LPCTSTR szGroupName, CMusicListItemUI* item, bool bAutoRefreshIndex = true);
	bool DeleteMusicAll(LPCTSTR szGroupName);
	bool HasMusic(LPCTSTR szGroupName, LPCTSTR szMusicPath);

	bool AddRadio(const RadioListItemInfo& item, int nIndex = -1);
	bool DeleteRadio(CMusicListItemUI* item);
	bool DeleteRadioAll();

	bool SelectItem(int iIndex, bool bTakeFocus);
	bool SetPlayingItem(int iIndex, bool bTakeFocus = false);
	bool SetRadioPlayingItem(int iIndex, bool bTakeFocus = false);
	int	 GetPlayingItem();

	CControlUI* CreateControl(LPCTSTR pstrClass) ;

private:
	LONG	delay_deltaY_;
	DWORD	delay_number_;
	DWORD	delay_left_;

	CPaintManagerUI& paint_manager_;
    CDialogBuilder m_dlgItemBuilder;
	CDialogBuilder m_dlgGroupBuilder;
	CDialogBuilder m_dlgRadioBuilder;

	int		m_nPlayingItemIndex;
//	CMusicInfoDialog *m_pMusicInfoDlg;

};

#endif // UIMUSIC_LIST_H