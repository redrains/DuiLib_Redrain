#include "..\duilib.h"
#include "..\MusicInfoDialog.h"

///////////////////////////////////////////////////////////////////////
//
//
CMusicListItemUI::CMusicListItemUI():
	m_pMusicListTab(NULL),
	m_pMusicListHotTab(NULL),
	bActivated(false),
	m_pMusicInfoDlg(NULL)
{

}
LPCTSTR CMusicListItemUI::GetClass() const
{
	return _T("TreeNodeUI");
}

LPVOID CMusicListItemUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, kMusicListItemUIInferFace) == 0 ) return static_cast<CMusicListItemUI*>(this);
	return CTreeNodeUI::GetInterface(pstrName);
}

MEDIA* CMusicListItemUI::GetMedia()
{
	return (MEDIA*)GetTag();
}

bool CMusicListItemUI::Activate()
{
	if( !CContainerUI::Activate() ) return false;
	if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMACTIVATE);
	
	static_cast<CMusicListUI*>(GetOwner())->SetPlayingItem(GetIndex());
	bActivated = true;
	return true;
}

void CMusicListItemUI::SetPlaying(bool bPlay /*= true*/)
{
	if (m_pMusicListTab == NULL)
		m_pMusicListTab = static_cast<CTabLayoutUI*>(m_pManager->FindSubControlByName(this, kMusicTabControlName));
	if (m_pMusicListTab == NULL)
		return;

	
	if (bPlay)
	{		
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		m_pMusicListTab->SelectItem(1);
		SetFixedHeight(kMusicListItemActivatedHeight);
		SetBkImage(pInfo->sDisabledImage);
		m_pMusicListTab->SetFixedHeight(kMusicListItemActivatedHeight);
	} 
	else
	{
		m_pMusicListTab->SelectItem(0);
		SetFixedHeight(kMusicListItemNormalHeight);
		SetBkImage(_T(""));
		m_pMusicListTab->SetFixedHeight(kMusicListItemNormalHeight);
	}
}

bool CMusicListItemUI::Hot(bool bHot)
{

	if( !IsEnabled()) return false;
	if (m_pMusicListHotTab == NULL)
		m_pMusicListHotTab = static_cast<CTabLayoutUI*>(m_pManager->FindSubControlByName(this, kMusicHotTabControlName));

	if ( m_pMusicListHotTab != NULL )
	{
		CButtonUI	*pBtnPalyButton = static_cast<CButtonUI*>(m_pManager->FindSubControlByName(this, kPlayControlControlName));
		CLabelUI	*pLblMusicIndex = static_cast<CLabelUI*>(m_pManager->FindSubControlByName(this, kMusicIndexControlName));
		if ( pBtnPalyButton == NULL || pLblMusicIndex == NULL )
			return false;
		if (bHot)
		{
			m_pMusicListHotTab->SelectItem(1);
			pBtnPalyButton->SetVisible(true);
			pLblMusicIndex->SetVisible(false);
		}
		else
		{
			m_pMusicListHotTab->SelectItem(0);
			pBtnPalyButton->SetVisible(false);
			pLblMusicIndex->SetVisible(true);
		}
	}
	Invalidate();
	return true;
}

void CMusicListItemUI::DoEvent(TEventUI& event)
{
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Hot();
				Invalidate();
			}
			if (m_pMusicInfoDlg == NULL)
			{
				m_pMusicInfoDlg = new CMusicInfoDialog;
				m_pMusicInfoDlg->Init(_T("xmls\\MusicList\\MusicInfoWnd.xml"),m_pManager->GetPaintWindow(), 357);
				m_pMusicInfoDlg->ResetWindow(GetPos().top - 10, GetMedia());
			}

// 			CMusicInfoDialog *pMusicInfoDlg = (static_cast<CMusicListGroupUI*>(GetParentNode()))->GetMusicList()->GetMusicInfoDialog();
// 			if (pMusicInfoDlg != NULL)
// 			{
// 				pMusicInfoDlg->ResetWindow(GetPos().top - 10, GetMedia());
// 				pMusicInfoDlg->ShowWindow();
// 			}			

			return;
		}
		else if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( (m_uButtonState & UISTATE_HOT) != 0 ) {
				m_uButtonState &= ~UISTATE_HOT;
				Hot(false);
				Invalidate();
			}
			if (m_pMusicInfoDlg != NULL)
			{
				m_pMusicInfoDlg->Close();
				m_pMusicInfoDlg = NULL;
			}
// 			CMusicInfoDialog *pMusicInfoDlg = (static_cast<CMusicListGroupUI*>(GetParentNode()))->GetMusicList()->GetMusicInfoDialog();
// 			if (pMusicInfoDlg != NULL)
// 			{
// 				pMusicInfoDlg->ShowWindow(false);
// 			}
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

void CMusicListItemUI::DrawItemBk(HDC hDC, const RECT& rcItem)
{	
	if (static_cast<CMusicListUI*>(GetOwner())->GetPlayingItem() == GetIndex()) return;
	CListContainerElementUI::DrawItemBk(hDC, rcItem);
	if (IsSelected() ) return;

	TListInfoUI* pInfo = m_pOwner->GetListInfo();
	if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		if( !pInfo->sHotImage.IsEmpty() ) {
			CRenderEngine::DrawRect(hDC, m_rcItem, 1, pInfo->dwDisabledBkColor);
		}
	}
	
}
///////////////////////////////////////////////////////////////////////
//
//

CMusicListGroupUI::CMusicListGroupUI():m_pMusicList(NULL)
{

}
LPCTSTR CMusicListGroupUI::GetClass() const
{
	return _T("TreeNodeUI");
}

LPVOID CMusicListGroupUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, kMusicListGroupUIInferFace) == 0 ) return static_cast<CMusicListGroupUI*>(this);
	return CTreeNodeUI::GetInterface(pstrName);
}


void CMusicListGroupUI::DoEvent(TEventUI& event)
{
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN &&
		event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else CContainerUI::DoEvent(event);
			return;
	}
	
	if( event.Type == UIEVENT_BUTTONDOWN)
	{
		return;
	}	
	else if( event.Type == UIEVENT_CONTEXTMENU )
	{
		if( IsContextMenuUsed() ) {
			m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
		}
	}
	else
	{
		CTreeNodeUI::DoEvent(event);
	}
}

bool CMusicListGroupUI::AdjustIndexs(int nStartPos)
{
	TCHAR szBuf[MAX_PATH] = {0};
	CMusicListItemUI *pListElement = NULL;
	CLabelUI* id = NULL;
	int nSum = GetTreeNodes().GetSize();
	for (int i = 0; i < nSum; i++ )
	{
		pListElement = static_cast<CMusicListItemUI*>(GetTreeNodes().GetAt(i));
		if (pListElement == NULL)
			return false;
		id = static_cast<CLabelUI*>(GetTreeView()->GetManager()->FindSubControlByName(pListElement, kMusicIndexControlName));
		if (id != NULL)
		{
			int index = 0;

			_stprintf_s(szBuf, MAX_PATH - 1, _T("%2.2d"), i + 1);
			id->SetText(szBuf);
		}
		else
			return false;
	}
	return true;
}


void CMusicListGroupUI::SetMusicList(CMusicListUI* pMusicList)
{
	if (pMusicList != NULL)
		m_pMusicList = pMusicList;
}

CMusicListUI* CMusicListGroupUI::GetMusicList()
{
	return m_pMusicList;
}

CStdStringPtrMap *CMusicListGroupUI::GetGroupItemHash() 
{
	return &m_mGroupItemHash;
}

bool CMusicListGroupUI::HasMusic(LPCTSTR szMusicPath)
{
	if(m_mGroupItemHash.Find(szMusicPath) != NULL)
		return true;
	else
		return false;
}

bool CMusicListGroupUI::Add(CMusicListItemUI* pControl, bool bAutoRefreshIndex)
{
	bool nRet = CTreeNodeUI::Add(pControl);

	MEDIA * pMedia = pControl->GetMedia();
	if (nRet)
		m_mGroupItemHash.Insert(pMedia->pathFileName.GetData(), pMedia);

	if (bAutoRefreshIndex)
	{
		int nIndex = GetTreeNodes().GetSize() + 1;
		
		if (nRet)
		{
			CDuiString text;
			text.Format(_T("%s[%d]"), m_GroupName.GetData(), GetTreeNodes().GetSize());
			CTreeNodeUI::SetItemText(text.GetData());
		}

		AdjustIndexs(nIndex);
	} 

	return nRet;
}
bool CMusicListGroupUI::AddAt(CMusicListItemUI* pControl, int iIndex, bool bAutoRefreshIndex)
{
	bool nRet = CTreeNodeUI::AddAt(pControl, iIndex);

	MEDIA * pMedia = pControl->GetMedia();
	if (nRet)
		m_mGroupItemHash.Insert(pMedia->pathFileName.GetData(), pMedia);

	if (bAutoRefreshIndex)
	{
		if (nRet)
		{
			CDuiString text;
			text.Format(_T("%s[%d]"), m_GroupName.GetData(), GetTreeNodes().GetSize());
			CTreeNodeUI::SetItemText(text.GetData());
		}
		AdjustIndexs(iIndex);
	}

	return nRet;

}
bool CMusicListGroupUI::RemoveAt(CMusicListItemUI* pControl, bool bAutoRefreshIndex )
{
	int nIndex = GetTreeNodes().Find(pControl);
	bool nRet = CTreeNodeUI::RemoveAt(pControl);

	MEDIA * pMedia = pControl->GetMedia();
	if (nRet)
		m_mGroupItemHash.Remove(pMedia->pathFileName.GetData());

	if (bAutoRefreshIndex)
	{
		if (nRet)
		{
			CDuiString text;
			text.Format(_T("%s[%d]"), m_GroupName.GetData(), GetTreeNodes().GetSize());
			CTreeNodeUI::SetItemText(text.GetData());
		}
		AdjustIndexs(nIndex);
	}

	return nRet;
}

void CMusicListGroupUI::SetItemText( LPCTSTR pstrValue )
{
	m_GroupName = pstrValue;
	CTreeNodeUI::SetItemText(pstrValue);
}

///////////////////////////////////////////////////////////////////////
//
//
CMusicListUI::CMusicListUI(CPaintManagerUI& paint_manager)
	: delay_deltaY_(0)
	, delay_number_(0)
	, delay_left_(0)
	, paint_manager_(paint_manager)
	,m_nPlayingItemIndex(-1)
//	,m_pMusicInfoDlg(NULL)
{
	SetItemShowHtml(true);
}

CMusicListUI::~CMusicListUI()
{
	delete (MEDIA*)GetTag();

}

double CMusicListUI::CalculateDelay(double state)
{
	return pow(state, 2);
}

CControlUI* CMusicListUI::CreateControl(LPCTSTR pstrClass) 
{
	if (_tcsicmp(pstrClass, kMusicListItemUIInferFace) == 0)
		return	new CMusicListItemUI();
	else if(_tcsicmp(pstrClass, kMusicListGroupUIInferFace) == 0)
		return	new CMusicListGroupUI();
	else if(_tcsicmp(pstrClass, kRadioListItemUIInferFace) == 0)
		return	new CRadioListItemUI();
	return NULL;
}
// 为音乐列表添加信息弹出窗体，暂时弃用，改为为每个音乐项目添加信息弹出窗体
// void CMusicListUI::CreateMusicInfoDialog()
// {
// 	m_pMusicInfoDlg = new CMusicInfoDialog;
// 	m_pMusicInfoDlg->Init(_T("xmls\\MusicList\\MusicInfoWnd.xml"),m_pManager->GetPaintWindow(), 357);
// 
// }
// 
// 
// CMusicInfoDialog* CMusicListUI::GetMusicInfoDialog()
// {
// 	return m_pMusicInfoDlg;
// }

void CMusicListUI::DoEvent(TEventUI& event) 
{
	if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
	{
		if (m_pParent != NULL)
			m_pParent->DoEvent(event);
		else
			CVerticalLayoutUI::DoEvent(event);
		return;
	}

	if (event.Type == UIEVENT_TIMER && event.wParam == SCROLL_TIMERID)
	{
		if (delay_left_ > 0)
		{
			--delay_left_;
			SIZE sz = GetScrollPos();
			LONG lDeltaY =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaY_);
			if ((lDeltaY > 0 && sz.cy != 0)  || (lDeltaY < 0 && sz.cy != GetScrollRange().cy ))
			{
				sz.cy -= lDeltaY;
				SetScrollPos(sz);
				return;
			}
		}
		delay_deltaY_ = 0;
		delay_number_ = 0;
		delay_left_ = 0;
		m_pManager->KillTimer(this, SCROLL_TIMERID);
		return;
	}
	if (event.Type == UIEVENT_SCROLLWHEEL)
	{
		LONG lDeltaY = 0;
		if (delay_number_ > 0)
			lDeltaY =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaY_);
		switch (LOWORD(event.wParam))
		{
		case SB_LINEUP:
			if (delay_deltaY_ >= 0)
				delay_deltaY_ = lDeltaY + 8;
			else
				delay_deltaY_ = lDeltaY + 12;
			break;
		case SB_LINEDOWN:
			if (delay_deltaY_ <= 0)
				delay_deltaY_ = lDeltaY - 8;
			else
				delay_deltaY_ = lDeltaY - 12;
			break;
		}
		if
			(delay_deltaY_ > 100) delay_deltaY_ = 100;
		else if
			(delay_deltaY_ < -100) delay_deltaY_ = -100;

		delay_number_ = (DWORD)sqrt((double)abs(delay_deltaY_)) * 5;
		delay_left_ = delay_number_;
		m_pManager->SetTimer(this, SCROLL_TIMERID, 50U);
		return;
	}

	CListUI::DoEvent(event);
}

bool CMusicListUI::AddGroup(LPCTSTR szGroupName, int nIndex)
{
	CMusicListGroupUI* pListGroup = NULL;
	if( !m_dlgGroupBuilder.GetMarkup()->IsValid() ) {
		pListGroup = static_cast<CMusicListGroupUI*>(m_dlgGroupBuilder.Create(_T("xmls\\MusicList\\music_list_group.xml"), (UINT)0, this, &paint_manager_));
	}
	else {
		pListGroup = static_cast<CMusicListGroupUI*>(m_dlgGroupBuilder.Create(this, &paint_manager_));
	}
	if (pListGroup == NULL)
		return false;

//	pListGroup->SetUserData(szGroupName);
	
	pListGroup->SetName(szGroupName);
	pListGroup->SetItemText(szGroupName);
	pListGroup->SetMusicList(this);

	if (nIndex == -1)
	{
		if (!Add(pListGroup))
		{
			delete pListGroup;
			return false;
		}
	} 
	else
	{
		if (!AddAt(pListGroup, nIndex))
		{
			delete pListGroup;
			return false;
		}
	}


	return true;
}

CMusicListGroupUI *CMusicListUI::GetGroup(LPCTSTR szGroupName)
{
	CMusicListGroupUI *pGroup = static_cast<CMusicListGroupUI*>(m_pManager->FindSubControlByName(this,szGroupName));
	if ( pGroup == NULL )
		return NULL;

	return pGroup;
}

CStdStringPtrMap *CMusicListUI::GetGroupItemHash(LPCTSTR szGroupName)
{
	CMusicListGroupUI *pGroup = static_cast<CMusicListGroupUI*>(m_pManager->FindSubControlByName(this,szGroupName));
	if ( pGroup == NULL )
		return NULL;

	return pGroup->GetGroupItemHash();
}


// 早期的AddMusic函数，弃用
// bool CMusicListUI::AddMusic(LPCTSTR szGroupName, const MusicListItemInfo& item, bool bAutoRefreshIndex, int nIndex)
// {
// 	TCHAR szBuf[MAX_PATH] = {0};
// 
//     CMusicListItemUI* pListElement = NULL;
//     if( !m_dlgItemBuilder.GetMarkup()->IsValid() ) {
//         pListElement = static_cast<CMusicListItemUI*>(m_dlgItemBuilder.Create(_T("xmls\\MusicList\\music_list_item.xml"), (UINT)0, this, &paint_manager_));
//     }
//     else {
//         pListElement = static_cast<CMusicListItemUI*>(m_dlgItemBuilder.Create(this, &paint_manager_));
//     }
//     if (pListElement == NULL)
//         return NULL;
// 
// 	pListElement->SetUserData(item.path.GetData());
// 	CButtonUI* log_button = static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(pListElement, kLogoButtonControlName));
// 	if (log_button != NULL)
// 	{
// 		if (!item.logo.IsEmpty())
// 		{
// 			log_button->SetBkImage(item.logo.GetData());
// 		}
// 	}
// 
// 	CLabelUI* music_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kMusicNameControlName));
// 	CLabelUI* music_name2 = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kMusicName2ControlName));
// 	if (music_name != NULL && music_name2 != NULL)
// 	{
// 		music_name->SetShowHtml(true);
// 		music_name->SetText(item.music_name.GetData());
// 		music_name2->SetShowHtml(true);
// 		music_name2->SetText(item.music_name.GetData());
// 	}
// 
// 	CLabelUI* music_length = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kMusicLengthControlName));
// 	CLabelUI* music_curpos = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kMusicCurPosControlName));
// 	if (music_length != NULL && music_curpos != NULL)
// 	{
// 		music_length->SetText(item.sum_time.GetData());
// 		_stprintf_s(szBuf, MAX_PATH - 1, _T("00:00/%s"),item.sum_time.GetData());
// 		music_curpos->SetText(szBuf);
// 	}
// 
// 	CMusicListGroupUI *pDefaultGroup = static_cast<CMusicListGroupUI*>(m_pManager->FindSubControlByName(this,szGroupName));
// 	if ( pDefaultGroup == NULL )
// 		return false;
// 	if (bAutoRefreshIndex)
// 	{
// 		if (nIndex == -1)
// 		{
// 			if (!pDefaultGroup->Add(pListElement))
// 			{
// 				delete pListElement;
// 				return false;
// 			}
// 		} 
// 		else
// 		{
// 			if (!pDefaultGroup->AddAt(pListElement, nIndex))
// 			{
// 				delete pListElement;
// 				return false;
// 			}
// 		}
// 	} 
// 	else
// 	{
// 		if (nIndex == -1)
// 		{
// 			if (!pDefaultGroup->CTreeNodeUI::Add(pListElement))
// 			{
// 				delete pListElement;
// 				return false;
// 			}
// 		} 
// 		else
// 		{
// 			if (!pDefaultGroup->CTreeNodeUI::AddAt(pListElement, nIndex))
// 			{
// 				delete pListElement;
// 				return false;
// 			}
// 		}
// 	}
// 
// 	return true;
// }

bool CMusicListUI::AddMusic(LPCTSTR szGroupName, MEDIA* item, bool bAutoRefreshIndex, int nIndex)
{
	TCHAR szBuf[MAX_PATH] = {0};

	CMusicListItemUI* pListElement = NULL;
	if( !m_dlgItemBuilder.GetMarkup()->IsValid() ) {
		pListElement = static_cast<CMusicListItemUI*>(m_dlgItemBuilder.Create(_T("xmls\\MusicList\\music_list_item.xml"), (UINT)0, this, &paint_manager_));
	}
	else {
		pListElement = static_cast<CMusicListItemUI*>(m_dlgItemBuilder.Create(this, &paint_manager_));
	}
	if (pListElement == NULL)
		return NULL;


	item->pControl = pListElement;
	pListElement->SetTag((UINT_PTR)item);

	CButtonUI* log_button = static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(pListElement, kLogoButtonControlName));
	if (log_button != NULL)
	{
		if (!item->logoPath.IsEmpty())
		{
			log_button->SetBkImage(item->logoPath.GetData());
		}
	}

	CLabelUI* music_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kMusicNameControlName));
	CLabelUI* music_name2 = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kMusicName2ControlName));
	if (music_name != NULL && music_name2 != NULL)
	{
		music_name->SetShowHtml(true);
		music_name->SetText(item->name.GetData());
		music_name2->SetShowHtml(true);
		music_name2->SetText(item->name.GetData());
	}

	CLabelUI* music_length = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kMusicLengthControlName));
	CLabelUI* music_curpos = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kMusicCurPosControlName));
	CDuiString strSumTime = CMusicPlayer::TimeToString(item->totalTime.sec);
	if (music_length != NULL && music_curpos != NULL)
	{
		music_length->SetText(strSumTime.GetData());
		_stprintf_s(szBuf, MAX_PATH - 1, _T("00:00/%s"),strSumTime.GetData());
		music_curpos->SetText(szBuf);
	}

	CMusicListGroupUI *pDefaultGroup = static_cast<CMusicListGroupUI*>(m_pManager->FindSubControlByName(this,szGroupName));
	if ( pDefaultGroup == NULL )
		return false;
	if (bAutoRefreshIndex)
	{
		if (nIndex == -1)
		{
			if (!pDefaultGroup->Add(pListElement))
			{
				delete pListElement;
				return false;
			}
		} 
		else
		{
			if (!pDefaultGroup->AddAt(pListElement, nIndex))
			{
				delete pListElement;
				return false;
			}
		}
	} 
	else
	{
		if (nIndex == -1)
		{
			if (!pDefaultGroup->Add(pListElement, false))
			{
				delete pListElement;
				return false;
			}
		} 
		else
		{
			if (!pDefaultGroup->AddAt(pListElement, nIndex, false))
			{
				delete pListElement;
				return false;
			}
		}
	}

	return true;
}

bool CMusicListUI::DeleteMusic(LPCTSTR szGroupName, CMusicListItemUI* item, bool bAutoRefreshIndex )
{
	CMusicListGroupUI *pGroup = static_cast<CMusicListGroupUI*>(m_pManager->FindSubControlByName(this,szGroupName));
	if ( pGroup == NULL )
		return false;

	if (bAutoRefreshIndex)
	{
		if (!pGroup->RemoveAt(item))
			return false;
	}
	else
	{
		if (!pGroup->RemoveAt(item, false))
			return false;
	}

	return true;
}


bool CMusicListUI::DeleteMusicAll(LPCTSTR szGroupName)
{
	CMusicListGroupUI *pGroup = static_cast<CMusicListGroupUI*>(m_pManager->FindSubControlByName(this,szGroupName));
	if ( pGroup == NULL )
		return false;

	pGroup->RemoveAll();
	return true;
}

bool CMusicListUI::HasMusic(LPCTSTR szGroupName, LPCTSTR szMusicPath)
{
	CMusicListGroupUI *pDefaultGroup = static_cast<CMusicListGroupUI*>(m_pManager->FindSubControlByName(this,szGroupName));
	if ( pDefaultGroup == NULL )
		return false;

	return pDefaultGroup->HasMusic(szMusicPath);

}


bool CMusicListUI::AddRadio(const RadioListItemInfo& item, int nIndex)
{
	TCHAR szBuf[MAX_PATH] = {0};

	CRadioListItemUI* pListElement = NULL;
	if( !m_dlgRadioBuilder.GetMarkup()->IsValid() ) {
		pListElement = static_cast<CRadioListItemUI*>(m_dlgRadioBuilder.Create(_T("xmls\\MusicList\\radio_list_item.xml"), (UINT)0, this, &paint_manager_));
	}
	else {
		pListElement = static_cast<CRadioListItemUI*>(m_dlgRadioBuilder.Create(this, &paint_manager_));
	}
	if (pListElement == NULL)
		return NULL;


	CButtonUI* log_small_button = static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(pListElement, kSmallLogoButtonControlName));
	if (log_small_button != NULL)
	{
		if (!item.logo_small.IsEmpty())
		{
			log_small_button->SetBkImage(item.logo_small.GetData());
		}
	}

	CButtonUI* log_button = static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(pListElement, kLogoButtonControlName));
	if (log_button != NULL)
	{
		if (!item.logo.IsEmpty())
		{
			log_button->SetBkImage(item.logo.GetData());
		}
	}

	CLabelUI* music_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kMusicNameControlName));
	CLabelUI* music_name2 = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kMusicName2ControlName));
	if (music_name != NULL && music_name2 != NULL)
	{
		music_name->SetShowHtml(true);
		music_name->SetText(item.music_name.GetData());
		music_name2->SetShowHtml(true);
		music_name2->SetText(item.music_name.GetData());
	}

	CLabelUI* radio_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kRadioNameControlName));
	if (radio_name != NULL )
	{
		radio_name->SetText(item.radio_name.GetData());
	}

	CLabelUI* audience_num = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kAudienceNumControlName));
	if (audience_num != NULL )
	{
		audience_num->SetText(item.audience_num.GetData());
	}

	CLabelUI* music_class = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kMusicClassControlName));
	if (music_class != NULL )
	{
		music_class->SetText(item.radio_name.GetData());
	}

	CTreeNodeUI *pDefaultGroup = static_cast<CTreeNodeUI*>(m_pManager->FindSubControlByName(this,kRadioGroup));
	if ( pDefaultGroup == NULL )
		return false;

	if (nIndex == -1)
	{
		if (!pDefaultGroup->Add(pListElement))
		{
			delete pListElement;
			return false;
		}
	} 
	else
	{
		if (!pDefaultGroup->AddAt(pListElement, nIndex))
		{
			delete pListElement;
			return false;
		}
	}
	
	return true;
}

bool CMusicListUI::DeleteRadio(CMusicListItemUI* item)
{
	CTreeNodeUI *pGroup = static_cast<CTreeNodeUI*>(m_pManager->FindSubControlByName(this,kRadioGroup));
	if ( pGroup == NULL )
		return false;

	return pGroup->Remove(item);
}


bool CMusicListUI::DeleteRadioAll()
{
	CTreeNodeUI *pGroup = static_cast<CTreeNodeUI*>(m_pManager->FindSubControlByName(this,kRadioGroup));
	if ( pGroup == NULL )
		return false;

	pGroup->RemoveAll();
	return true;
}

bool CMusicListUI::SetPlayingItem(int iIndex, bool bTakeFocus)
{
	if (!__super::SelectItem(iIndex, bTakeFocus))
		return false;

	if (m_nPlayingItemIndex != -1)
	{
		CMusicListItemUI* pListItem = static_cast<CMusicListItemUI*>(GetItemAt(m_nPlayingItemIndex));
		pListItem->SetPlaying(false);
	}

	m_nPlayingItemIndex = iIndex;
	if (m_nPlayingItemIndex != -1)
	{
		CMusicListItemUI* pListItem = static_cast<CMusicListItemUI*>(GetItemAt(m_nPlayingItemIndex));
		pListItem->SetPlaying(true);
	}

	return true;
}

bool CMusicListUI::SetRadioPlayingItem(int iIndex, bool bTakeFocus)
{
	if (!__super::SelectItem(iIndex, bTakeFocus))
		return false;

	if (m_nPlayingItemIndex != -1)
	{
		CRadioListItemUI* pListItem = static_cast<CRadioListItemUI*>(GetItemAt(m_nPlayingItemIndex));
		pListItem->SetPlaying(false);
	}

	m_nPlayingItemIndex = iIndex;
	if (m_nPlayingItemIndex != -1)
	{
		CRadioListItemUI* pListItem = static_cast<CRadioListItemUI*>(GetItemAt(m_nPlayingItemIndex));
		pListItem->SetPlaying(true);
	}

	return true;
}
int	 CMusicListUI::GetPlayingItem()
{
	return m_nPlayingItemIndex;
}
bool CMusicListUI::SelectItem(int iIndex, bool bTakeFocus)
{
	 return __super::SelectItem(iIndex, bTakeFocus);
}