#include "duilib.h"
#include "Controls\PopupWidget.h"
#include "Wininet.h"
#pragma  comment(lib, "Wininet.lib")


/////////////////////////////////////////////////////////////////////////////////////////
//////////////////
//////////////////
CFrameWnd::CFrameWnd( LPCTSTR pszXMLPath )
	:  m_strXMLPath(pszXMLPath),
	m_DropTarget(this),
	m_pLblMainWndTitile(NULL),
	m_pWndTitle(NULL),
	m_pTabLeft(NULL),
	m_pTabTop(NULL),
	m_pBtnGoBack(NULL),
	m_pBtnRefresh(NULL),
	m_pMainTitleRight(NULL),
	m_pMainMusicBank(NULL),
	m_pMainShowRight(NULL),
	m_pLblBottomInfo(NULL),
	m_pMainStatus(NULL),
	m_pLocaltion(NULL),
	m_pLeftTabListBk(NULL),
	m_pMusicList(NULL),
	m_pRadioList(NULL),
	m_pDownload(NULL),
	m_pMoreList(NULL),
	m_pWke(NULL),
	m_nListTrans(kMainWndDefaultListTrans),
	m_bSkinTrans(false),
	m_nSkinTrans(0),
	m_MusicPlayer(this),  
	m_pLblPlayTime(NULL),
	m_pLblTotalTime(NULL),
	m_pSliderPlayProcess(NULL),
	m_pBtnMusicPlay(NULL),
	m_pBtnMusicPause(NULL),
	m_pBtnMusicLrc(NULL),
	m_pBtnAddMusic(NULL),
	m_pBtnMusicSound(NULL),
	m_pBtnMusicLoop(NULL),
	m_pZplayer(NULL),
	m_pCurMedia(NULL)
{
}

LPCTSTR CFrameWnd::GetWindowClassName() const
{
	return _T("MainWnd");
}

CDuiString CFrameWnd::GetSkinFile()
{
	return m_strXMLPath;
}

CDuiString CFrameWnd::GetSkinFolder()
{
	return _T("");
}

CControlUI* CFrameWnd::CreateControl( LPCTSTR pstrClassName )
{
	if (_tcsicmp(pstrClassName, kMusicListUIInterFace) == 0)
		return  new CMusicListUI(m_PaintManager);
	else if (_tcsicmp(pstrClassName, kWebkitUIInterface) == 0)
		return  new CWkeWebkitUI();
	else if (_tcsicmp(pstrClassName, kHotButtonInterface) == 0)
		return  new CHotButtonUI();
	return NULL;
}

UINT CFrameWnd::MessageBox( LPCTSTR lpText, UINT uIcon/* = MB_ICONWARNING */, LPCTSTR lpCaption /*= _T("提示")*/,UINT uType /*= MB_OK */)
{
	CMessageDialog MessageBox;
	return MessageBox.DuiMessageBox(this, lpText, lpCaption, uType, uIcon);
}

// 	注释的代码可以让程序的资源集成到exe中，开发阶段不使用
// 	UILIB_RESOURCETYPE CFrameWnd::GetResourceType() const
// 	{
// 		return UILIB_ZIPRESOURCE;
// 	}
// 
// 
// 	LPCTSTR CFrameWnd::GetResourceID() const
// 	{
// 		return MAKEINTRESOURCE(IDR_ZIPRES1);
// 	}

CPaintManagerUI* CFrameWnd::GetMainWndPaintManager()
{
	return &m_PaintManager;
}

void CFrameWnd::SetListTrans(int nValue)
{
	m_nListTrans = nValue;
	CDuiString strBkTrans;
	strBkTrans.Format(_T("file='UI\\LeftTab\\List_bk.png' fade='%d'"), 255 - nValue);
	m_pLeftTabListBk->SetBkImage(strBkTrans.GetData());
}
int  CFrameWnd::GetListTrans() const
{
	return m_nListTrans;
}

void CFrameWnd::SetIsSkinTrans(bool bTrans)
{
	m_bSkinTrans = bTrans;
	if (!bTrans)
		SetSkinTrans(0);
}

bool CFrameWnd::GetIsSkinTrans() const
{
	return m_bSkinTrans;
}

void CFrameWnd::SetSkinTrans(int nValue)
{
	m_nSkinTrans = nValue;
	m_PaintManager.SetTransparent(255 - nValue);
}
int  CFrameWnd::GetSkinTrans() const
{
	return m_nSkinTrans;
}


DWORD CFrameWnd::GetBkColor() const
{
	if (m_pBackGround != NULL)
		return m_pBackGround->GetBkColor();
	return 0;
}

void CFrameWnd::SetBkColor(DWORD dwBackColor)
{
	if (m_pBackGround != NULL)
	{
		m_pBackGround->SetBkImage(_T(""));
		m_pBackGround->SetBkColor(dwBackColor);
		m_pBackGround->NeedUpdate();
	}
}

void CFrameWnd::SetBkImage(CDuiString strBkImage)
{
	if (m_pBackGround != NULL)
		m_pBackGround->SetBkImage(strBkImage.GetData());
}

LPCTSTR CFrameWnd::GetBkImage()
{
	if (m_pBackGround != NULL)
		return m_pBackGround->GetBkImage();
	return _T("");
}

void CFrameWnd::LoadDefaultState()
{
	SetBkColor((DWORD)kMainWndDefaultBkColor);
	SetBkImage(kMainWndDefaultBkImage);
	::SetWindowPos(m_hWnd, NULL, 0, 0, kMainWndDefaultcx, kMainWndDefaultcy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	SetListTrans(kMainWndDefaultListTrans);
	SetSkinTrans(0);
	CenterWindow();
}

//歌曲播放回调函数
void CFrameWnd::player_proc(CMusicPlayer* pPlayer,PLAY_MSG msg,WPARAM wParam,LPARAM lParam,void* arg)
{

	CFrameWnd* pFrame = pPlayer->GetMainFrame();
	MEDIA* media = (MEDIA*)wParam;
	if (pFrame == NULL || media == NULL)
		return;

	switch(msg)
	{

	case MSG_OPEN:
		{
			pFrame->m_pLblTotalTime->SetText(CMusicPlayer::TimeToString(media->totalTime.sec));
			pFrame->m_pSliderPlayProcess->SetMinValue(0);
			pFrame->m_pSliderPlayProcess->SetMaxValue(media->totalTime.sec);
			pFrame->m_pSliderPlayProcess->SetValue(0);
		}
		break;
	case MSG_PLAYING:
		{

			CDuiString playTime = CMusicPlayer::TimeToString(lParam);
			pFrame->m_pLblPlayTime->SetText(playTime.GetData());
			pFrame->m_pSliderPlayProcess->SetValue((int)lParam);

			CLabelUI* music_curpos = static_cast<CLabelUI*>(pFrame->GetMainWndPaintManager()->FindSubControlByName(pFrame->GetCurMedia()->pControl, kMusicCurPosControlName));
			CDuiString strSumTime = CMusicPlayer::TimeToString(media->totalTime.sec);
			if ( music_curpos != NULL)
			{
				TCHAR szBuf[MAX_PATH] = {0};
				_stprintf_s(szBuf, MAX_PATH - 1, _T("%s/%s"),playTime.GetData(), strSumTime.GetData());
				music_curpos->SetText(szBuf);
			}
		}
		break;
	case MSG_PLAY:
		{
			media->playNum += 1;
			if (pFrame->m_pLblMainWndTitile != NULL)
				pFrame->m_pLblMainWndTitile->SetText(media->name.GetData());
		}
		break;
	case MSG_PAUSE:
		{

		}
		break;
	case MSG_RESUME:
		{
		}
		break;
	case MSG_STOP:
		{
			if (pPlayer->GetPlayState() == P_STOP)
			{
				pFrame->SetPlayBtnState(false);
				pFrame->m_pSliderPlayProcess->SetValue(0);

				pFrame->m_pLblPlayTime->SetText(_T("00:00"));

				CLabelUI* music_curpos = static_cast<CLabelUI*>(pFrame->GetMainWndPaintManager()->FindSubControlByName(/*pCurMedia*/media->pControl, kMusicCurPosControlName));
				if ( music_curpos != NULL)
				{
					music_curpos->SetText(_T("00:00"));
				}

				if (pFrame->m_pLblMainWndTitile != NULL)
					pFrame->m_pLblMainWndTitile->SetText(MainWndTitle);

			}
		}
		break;
	case MSG_VOLUME:
		{
		}
		break;
	}
}

MEDIA* CFrameWnd::PlayNextMedia(CMusicPlayer* pPlayer)
{
	CFrameWnd* pFrame = pPlayer->GetMainFrame();
	if(pFrame == NULL)
		return NULL;
	MEDIA* pCurMedia = pFrame->GetCurMedia();
	if(pCurMedia == NULL)
		return NULL;
	CMusicListUI *pMusicList =  pFrame->GetMusicList(); 
	if(pMusicList == NULL)
		return NULL;
	CStdStringPtrMap *pGroupMusicMap = pMusicList->GetGroupItemHash(_T("默认列表"));
	int nIndex = pCurMedia->pControl->GetNodeIndex();
	CMusicListItemUI* pNextMusicItem = NULL;
	if (nIndex == pGroupMusicMap->GetSize()-1)
	{
		if (pPlayer->GetLoopState() == LOOP_LIST)
			pNextMusicItem = (CMusicListItemUI*)pMusicList->GetGroup(_T("默认列表"))->GetChildNode(0);
	}
	else
	{
		pNextMusicItem = (CMusicListItemUI*)pMusicList->GetGroup(_T("默认列表"))->GetChildNode(nIndex+1);
	}
	if (pNextMusicItem !=NULL)
	{
		pNextMusicItem->Activate();
		return pNextMusicItem->GetMedia();
	}
	return NULL;

}

MEDIA* CFrameWnd::PlayPrevMedia(CMusicPlayer* pPlayer)
{
	CFrameWnd* pFrame = pPlayer->GetMainFrame();
	if(pFrame == NULL)
		return NULL;
	MEDIA* pCurMedia = pFrame->GetCurMedia();
	if(pCurMedia == NULL)
		return NULL;
	CMusicListUI *pMusicList =  pFrame->GetMusicList(); 
	if(pMusicList == NULL)
		return NULL;
	CStdStringPtrMap *pGroupMusicMap = pMusicList->GetGroupItemHash(_T("默认列表"));
	int nIndex = pCurMedia->pControl->GetNodeIndex();
	CMusicListItemUI* pNextMusicItem = NULL;
	if (nIndex == 0)
	{
		pNextMusicItem = (CMusicListItemUI*)pMusicList->GetGroup(_T("默认列表"))->GetChildNode(pGroupMusicMap->GetSize()-1);
	}
	else
	{
		pNextMusicItem = (CMusicListItemUI*)pMusicList->GetGroup(_T("默认列表"))->GetChildNode(nIndex-1);
	}
	if (pNextMusicItem !=NULL)
	{
		pNextMusicItem->Activate();
		return pNextMusicItem->GetMedia();
	}
	return NULL;
}

MEDIA* CFrameWnd::PlayRandomMedia(CMusicPlayer* pPlayer)
{
	CFrameWnd* pFrame = pPlayer->GetMainFrame();
	if(pFrame == NULL)
		return NULL;

	CMusicListUI *pMusicList =  pFrame->GetMusicList(); 
	if(pMusicList == NULL)
		return NULL;

	CStdStringPtrMap *pGroupMusicMap = pMusicList->GetGroupItemHash(_T("默认列表"));

	int num = pGroupMusicMap->GetSize()-1;
	srand(timeGetTime());
	int nRandomIndex =rand()%num;

	CMusicListItemUI* pNextMusicItem = (CMusicListItemUI*)pMusicList->GetItemAt(nRandomIndex);

	if (pNextMusicItem !=NULL)
	{
		pNextMusicItem->Activate();
		return pNextMusicItem->GetMedia();
	}
	return NULL;

}

void CFrameWnd::AddMusicsToGroup(CMusicListUI* pMusicList, LPCTSTR szGroupName, vector<CDuiString>& fileNames, CDuiString szLogoPath, bool bCheckExist/* = true*/, bool bAutoRefreshIndex /*= true*/)
{
	CDuiString pathFileName;
	TCHAR       szName[_MAX_FNAME];
	TCHAR       szExt[_MAX_EXT];
	for (UINT i = 0;i < fileNames.size(); i++)
	{      

		if (bCheckExist)
			if (m_pMusicList->HasMusic(szGroupName,fileNames[i].GetData()))
				continue;

		_tsplitpath_s(fileNames[i].GetData(), NULL, 0, NULL, 0, szName, _MAX_FNAME, szExt, _MAX_EXT);
		_tcsupr_s(szExt, _MAX_EXT);
		
		MEDIA* pMedia = new MEDIA;
		pMedia->logoPath = szLogoPath.GetData();		
		pMedia->name = szName;			
		pMedia->exName = szExt;
		pMedia->playNum = 0;
		pMedia->size = CFileUtil::FileSizeToString(CFileUtil::GetFileSize(fileNames[i].GetData()));
		pMedia->pathFileName =fileNames[i];
		pMedia->totalTime = GetMusicLenght(fileNames[i]);

		pMusicList->AddMusic(szGroupName, pMedia, bAutoRefreshIndex);
	}
}

TStreamTime CFrameWnd::GetMusicLenght(CDuiString &MusicPath)
{
	TStreamInfo pInfo = {0};
	if (m_pZplayer->OpenFileW(MusicPath.GetData(), sfAutodetect))
		m_pZplayer->GetStreamInfo(&pInfo);

	return pInfo.Length;
}

MEDIA* CFrameWnd::GetCurMedia()
{
	return m_pCurMedia;
}

CMusicListUI* CFrameWnd::GetMusicList()
{
	return m_pMusicList;
}

void CFrameWnd::SetLoopState(LOOP_STATE state)
{
	m_MenuLoopCheckInfo.clear();

	m_MenuLoopCheckInfo.insert(map<CDuiString,bool>::value_type(kMenuLoopPalySingle, state & PLAY_SINGLE ?true:false));
	m_MenuLoopCheckInfo.insert(map<CDuiString,bool>::value_type(kMenuLoopLoopSingle, state & LOOP_SINGLE ?true:false));
	m_MenuLoopCheckInfo.insert(map<CDuiString,bool>::value_type(kMenuLoopLoopList, state & LOOP_LIST ?true:false));
	m_MenuLoopCheckInfo.insert(map<CDuiString,bool>::value_type(kMenuLoopPlayList, state & PLAY_LIST ?true:false));
	m_MenuLoopCheckInfo.insert(map<CDuiString,bool>::value_type(kMenuLoopRandom, state & LOOP_RANDOM ?true:false));

	
	if (m_pBtnMusicLoop != NULL)
	{
		CDuiString strNormal;
		CDuiString strHover;
		CDuiString strDown;
		int nImageLeft = 0;
		switch (state)
		{
		case LOOP_SINGLE:
			nImageLeft = 29;
			break;
		case PLAY_SINGLE:
			nImageLeft = 0;
			break;
		case LOOP_LIST:
			nImageLeft = 87;
			break;
		case PLAY_LIST:
			nImageLeft = 58;
			break;
		case LOOP_RANDOM:
			nImageLeft = 116;
			break;
		default:
			break;
		}
		strNormal.SmallFormat(_T("file='UI\\music_control\\repeat_normal.png' source='%d,0,%d,26'"),nImageLeft,nImageLeft+29);
		strHover.SmallFormat(_T("file='UI\\music_control\\repeat_hover.png' source='%d,0,%d,26'"),nImageLeft,nImageLeft+29);
		strDown.SmallFormat(_T("file='UI\\music_control\\repeat_down.png' source='%d,0,%d,26'"),nImageLeft,nImageLeft+29);
		m_pBtnMusicLoop->SetNormalImage(strNormal.GetData());
		m_pBtnMusicLoop->SetHotImage(strHover.GetData());
		m_pBtnMusicLoop->SetPushedImage(strDown.GetData());
	}
	
	m_MusicPlayer.SetLoopState(state);
}

LOOP_STATE CFrameWnd::GetLoopState()
{
	return m_MusicPlayer.GetLoopState();
}

void CFrameWnd::SetPlayBtnState(bool bIsPlay)
{
	if (m_pBtnMusicPlay != NULL && m_pBtnMusicPause != NULL)
	{
		m_pBtnMusicPlay->SetVisible(bIsPlay?false:true);
		m_pBtnMusicPause->SetVisible(bIsPlay?true:false);
	}
}

bool CFrameWnd::GetLeftListPos(RECT & rc) const
{
	if (m_pMusicList)
	{
		rc = m_pMusicList->GetPos();
		return true;
	}
	return false;

}

void CFrameWnd::InitWindow()
{
	CenterWindow();

	m_pLblMainWndTitile = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLblMainWndTitle));
	m_pWndTitle    = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(kMenuWndTitle));
	m_pTabLeft	=	static_cast<CAnimationTabLayoutUI*>(m_PaintManager.FindControl(kTabLeft));
	m_pTabTop		= static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabTop));
	m_pBtnGoBack	= static_cast<CButtonUI*>(m_PaintManager.FindControl(kBtnGoBack));
	m_pBtnRefresh   = static_cast<CButtonUI*>(m_PaintManager.FindControl(kBtnRefresh));
	m_pLeftTabListBk    = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(kLeftTabListBk));
	m_pLeftTabBk = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(kLeftTabBk));
	m_pMusicList	= static_cast<CMusicListUI*>(m_PaintManager.FindControl(kMusicList));
	m_pRadioList	=static_cast<CMusicListUI*>(m_PaintManager.FindControl(kRadioList));
	m_pDownload		= static_cast<CMusicListUI*>(m_PaintManager.FindControl(kDownloadList));
	m_pMoreList		=static_cast<CTileLayoutUI*>(m_PaintManager.FindControl(kMorecList));
	m_pMainTitleRight = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(kMainTitleRight));
	m_pMainMusicBank = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(kMainMusicBank));
	m_pMainShowRight = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(kMainShowRight));
	m_pMainStatus = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(kMainStatus));
	m_pLblBottomInfo = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLblBottomInfo));
	m_pBackGround = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(kWndBackGround));

	//给本地列表和下载列表增加分组
	if (m_pMusicList != NULL)
	{
		m_pMusicList->AddGroup(_T("默认列表"));
		m_pMusicList->AddGroup(_T("我喜爱的音乐"));
//		m_pMusicList->CreateMusicInfoDialog();弃用的函数
	}
	if (m_pDownload != NULL)
	{
		m_pDownload->AddGroup(_T("下载中"));
		m_pDownload->AddGroup(_T("已下载"));
	}


	//初始化“更多”列表
	TCHAR *szAppNames[] = {_T("传歌到手机"), _T("本地管理"), _T("铃声制作"), _T("繁星直播"), _T("在线KTV"), _T("格式转换"),
		_T("定时设置"), _T("网络测速"), _T("酷狗收音机"), _T("游戏中心"), _T("酷狗彩票"), _T("VIP中心")};
	if(m_pMoreList)
	{
		TCHAR szPath[MAX_PATH]={0};
		RECT rcTxt = {0,65,0,0};
		for (int i = 0;i < 12; i++)
		{
			CButtonUI *pBtn = new CButtonUI;
			_stprintf(szPath, _T("file='UI\\LeftTab\\more\\%d.png' dest='20,12,80,72'"), i + 1);
			pBtn->SetBkImage(szPath);
			pBtn->SetHotImage(_T("UI\\LeftTab\\more\\ListItem_bk_hover.png"));
			pBtn->SetPushedImage(_T("UI\\LeftTab\\more\\ListItem_bk_hover.png"));
			pBtn->SetText(szAppNames[i]);
			pBtn->SetTextPadding(rcTxt);
			pBtn->SetFixedHeight(105);
			pBtn->SetFixedWidth(100);
			m_pMoreList->Add(pBtn);
		}		
	}	

	m_pLblPlayTime = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLblPlayTime));
	m_pLblTotalTime = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLblTotalTime));
	m_pSliderPlayProcess = static_cast<CSliderUI*>(m_PaintManager.FindControl(kSliderPlayProcess));
	m_pBtnMusicPlay = static_cast<CButtonUI*>(m_PaintManager.FindControl(kBtnMusicPlay));
	m_pBtnMusicPause = static_cast<CButtonUI*>(m_PaintManager.FindControl(kBtnMusicPause));
	m_pBtnMusicLrc = static_cast<CButtonUI*>(m_PaintManager.FindControl(kBtnMusicLrc));
	m_pBtnAddMusic = static_cast<CButtonUI*>(m_PaintManager.FindControl(kBtnAddMusic));
	m_pBtnMusicSound = static_cast<CButtonUI*>(m_PaintManager.FindControl(kBtnMusicSound));
	m_pBtnMusicLoop = static_cast<CButtonUI*>(m_PaintManager.FindControl(kBtnMusicLoop));

	SetLoopState(LOOP_LIST);
	m_pZplayer = CreateZPlay();
	m_MusicPlayer.InitPlayer(player_proc,PlayNextMedia,PlayPrevMedia,PlayRandomMedia);


	//设置浏览器
	m_pWke = static_cast<CWkeWebkitUI*>(m_PaintManager.FindControl(_T("ie")));
	if (m_pWke)
	{
		m_pWke->SetURL(kOptTopMusicURL);
	}
// 	CWndShadow::Initialize(m_PaintManager.GetInstance());
// 	m_WndShadow.Create(m_hWnd);
// 	m_WndShadow.SetSize(4);
// 	m_WndShadow.SetPosition(0, 0);
//	m_WndShadow.SetColor( (COLORREF)RGB(255, 128, 0));
//	m_WndShadow.SetDarkness(150);
//	m_WndShadow.SetSharpness(5);


}

void CFrameWnd::OnPlayAnimation()
{
	//Duilib的动画类并不稳定，我这里只是测试所用，开启动画效果后会和wke内核冲突
	//这里获取音乐列表的位置并向右扩充两个元素来包含分割线
	CDuiRect rcList(m_pMusicList->GetPos());
	rcList.right += 2;

	m_PaintManager.AddAnimationJob(CDxAnimationUI(UIANIMTYPE_FLAT, 0, 600, 0x00935226, 0x00935226, m_pWndTitle->GetPos(), 40, 0, 4, 255, 0.3f));
	m_PaintManager.AddAnimationJob(CDxAnimationUI(UIANIMTYPE_FLAT, 50, 600, 0x00935226, 0x00935226, m_pLeftTabBk->GetPos(), 40, 0, 4, 255, 0.3f));
	m_PaintManager.AddAnimationJob(CDxAnimationUI(UIANIMTYPE_FLAT, 100, 600, 0x00935226, 0x00935226, rcList, 40, 0, 4, 255, 0.3f));
	m_PaintManager.AddAnimationJob(CDxAnimationUI(UIANIMTYPE_FLAT, 150, 600, 0x00935226, 0x00935226, m_pMainMusicBank->GetPos(), 40, 0, 4, 255, 0.3f));	
	m_PaintManager.AddAnimationJob(CDxAnimationUI(UIANIMTYPE_FLAT, 0, 600, 0x00935226, 0x00935226, m_pMainStatus->GetPos(), 0, -40, 4, 255, 0.3f));

}

void CFrameWnd::Notify( TNotifyUI& msg )
{
	CDuiString strCtrlName = msg.pSender->GetName();
	if( msg.sType == _T("windowinit") ) 
	{
		//OnPlayAnimation();

		m_DropTarget.DragDropRegister(m_hWnd);
		m_DropTarget.SetHDropCallBack(OnDropFiles);

	}	
	else if( msg.sType == _T("click") ) 
	{	
		if( strCtrlName == _T("closebtn") )
		{
			SetLoopState(EXIT_APP);
			ShowWindow(false);
			return; 
		}
		else if ( strCtrlName == kBtnGoBack)
		{

			if (m_pMusicList == NULL)
				return;

			vector<CDuiString> nameDemo1,nameDemo2;
			nameDemo1.push_back(_T("D:\\我的天空-Redrain仿酷狗.mp3"));
			nameDemo2.push_back(_T("D:\\lie to me.mp3"));

			for (UINT i = 0;i < 5; i++)
			{      
				AddMusicsToGroup(m_pMusicList, _T("默认列表"), nameDemo1, kDefaultLogoPath1, false);
				AddMusicsToGroup(m_pDownload, _T("已下载"), nameDemo1, kDefaultLogoPath1, false);
				AddMusicsToGroup(m_pMusicList, _T("我喜爱的音乐"), nameDemo2, kDefaultLogoPath2, false);
				AddMusicsToGroup(m_pDownload, _T("下载中"), nameDemo2, kDefaultLogoPath2, false);
			}


		} 
		else if( strCtrlName == kBtnRefresh )
		{
			if (m_pRadioList == NULL)
				return;

			RadioListItemInfo item;
			item.logo = _T("UI\\LeftTab\\radio\\logo1.png");
			item.logo_small = _T("UI\\LeftTab\\radio\\logo_small1.png");
			item.music_name = _T("Redrain测试音乐电台子项目");
			item.audience_num = _T("3064");
			item.radio_name = _T("网络红歌");
			m_pRadioList->AddRadio(item);

			RadioListItemInfo item2;
			item2.logo = _T("UI\\LeftTab\\radio\\logo2.png");
			item2.logo_small = _T("UI\\LeftTab\\radio\\logo_small2.png");
			item2.music_name = _T("凤凰传奇-序曲");
			item2.audience_num = _T("15021");
			item2.radio_name = _T("凤凰传奇电台");
			m_pRadioList->AddRadio(item2);

			RadioListItemInfo item3;
			item3.logo = _T("UI\\LeftTab\\radio\\logo3.png");
			item3.logo_small = _T("UI\\LeftTab\\radio\\logo_small3.png");
			item3.music_name = _T("亡灵序曲 - the dawn");
			item3.audience_num = _T("7492");
			item3.radio_name = _T("混合电台");
			m_pRadioList->AddRadio(item3);
		}
		else if( strCtrlName == kBtnSkinPicker )
		{
			CSkinPickerDialog *pSkinPicker = new CSkinPickerDialog(_T("xmls\\SkinPickerDialog.xml"),this);
			pSkinPicker->Create(m_hWnd,_T("皮肤与窗口调整"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
			pSkinPicker->CenterWindow();
			pSkinPicker->ShowModal();
			delete	pSkinPicker;
		}
		else if( strCtrlName == kBtnAddMusic ) 
		{
			vector<CDuiString> names;
			CFileUtil::OpenFile(STR_MUSIC_FILE_FILTER, GetHWND(),names);
			AddMusicsToGroup(m_pMusicList, _T("默认列表"), names);
		}
		else if( strCtrlName == kBtnMusicPlay ) 
		{
			m_MusicPlayer.Resume();
			if (m_MusicPlayer.GetPlayState() == P_PLAY)
				SetPlayBtnState(true);
		}
		else if( strCtrlName == kBtnMusicPause ) 
		{
			m_MusicPlayer.Pause();	
			if (m_MusicPlayer.GetPlayState() == P_PAUSE)
				SetPlayBtnState(false);
		}
		else if( strCtrlName == kBtnMusicPrev ) 
		{
			m_MusicPlayer.PlayPrev();	
		}
		else if( strCtrlName == kBtnMusicNext ) 
		{
			m_MusicPlayer.PlayNext();	
		}
		else if( strCtrlName == kBtnMusicLrc ) 
		{
			MessageBox(_T("歌词功能暂时未开发！"));	
		}		
		else if( strCtrlName == kMenuWndTitleSetting )
		{
			OnShowMenu(msg);
		}
		else if( strCtrlName == kMenuMusicGroupButton )
		{
			OnShowMenu(msg);
		}
		else if( strCtrlName == kBtnMusicLoop ) 
		{
			OnShowMenu(msg);			
		}
		else if( strCtrlName == kBtnTitleRemote ) 
		{
			MessageBox(_T("遥控器功能暂时未开发!"),MB_ICONASTERISK,_T("提示"),MB_OKCANCEL);			
		}
		else if( strCtrlName == kBtnTitleHideRight ) 
		{
			if(m_pMainTitleRight !=NULL && m_pMainMusicBank != NULL && m_pMainShowRight != NULL && m_pLblBottomInfo != NULL)
			{
				m_pMainShowRight->SetVisible(false);
				m_pMainMusicBank->SetVisible(false);
				m_pLblBottomInfo->SetVisible(false);
				m_pMainShowRight->SetVisible();
				
				//这里需要专门处理一下搜索栏的搜索按钮，否则他在缩小并恢复后会显示不正常
				CButtonUI *pSearchBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(kBtnTitleSearch));
				if (pSearchBtn)
					pSearchBtn->SetVisible(false);

				CControlUI* background = m_PaintManager.FindControl(kWndBackGround);
				if (background != NULL)
				{
					//这里修改了背景图片的属性，加了xtiled属性后背景图就不会背拉伸了
					m_strBkImage = background->GetBkImage();
					TCHAR szBuf[MAX_PATH] = {0};
					_stprintf_s(szBuf, MAX_PATH - 1, _T("file='%s' xtiled='true'"), m_strBkImage.GetData());
					background->SetBkImage(szBuf);					
				}

				m_PaintManager.SetMinInfo(kMainWndDefaultHidecx,kMainWndDefaultcy);
				m_PaintManager.SetMaxInfo(kMainWndDefaultHidecx,0);
				::SetWindowPos(m_hWnd, NULL, 0, 0, kMainWndDefaultHidecx, kMainWndDefaultcy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

				
			}			 	
		}
		else if( strCtrlName == kBtnTitleShowRight ) 
		{
			if(m_pMainTitleRight !=NULL && m_pMainMusicBank != NULL && m_pMainShowRight != NULL && m_pLblBottomInfo != NULL)
			{
				m_pMainShowRight->SetVisible();
				m_pMainMusicBank->SetVisible();
				m_pLblBottomInfo->SetVisible();
				m_pMainShowRight->SetVisible(false);

				CButtonUI *pSearchBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(kBtnTitleSearch));
				if (pSearchBtn)
					pSearchBtn->SetVisible();

				CControlUI* background = m_PaintManager.FindControl(kWndBackGround);
				if (background != NULL)
					background->SetBkImage(m_strBkImage);

				m_PaintManager.SetMinInfo(kMainWndDefaultMincx,kMainWndDefaultcy);
				m_PaintManager.SetMaxInfo(0,0);
				::SetWindowPos(m_hWnd, NULL, 0, 0, kMainWndDefaultcx, kMainWndDefaultcy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

			}			 	
		}	
		
	}
	else if ( msg.sType == _T("valuechanged") ||  msg.sType == _T("movevaluechanged"))
	{  
		if (strCtrlName == kWidgetSlider )
		{
			switch ((WidgetType)msg.pSender->GetTag())
			{
			case CHANGE_LIST_TRANS:
				{
					//改变播放列表的透明度
					SetListTrans(static_cast<CSliderUI*>(msg.pSender)->GetValue());
					break;
				}				
			case CHANGE_MAINWND_TRANS:
				{
					//改变皮肤整体透明度
					SetSkinTrans(static_cast<CSliderUI*>(msg.pSender)->GetValue());
					break;
				}				
			case CHANGE_VOLUME:
				{
					int nMusicVolume = static_cast<CSliderUI*>(msg.pSender)->GetValue();
					m_MusicPlayer.SetVolume(nMusicVolume);
					break;
				}				
			default:
				break;
			}
		}
		else if ( strCtrlName == kSliderPlayProcess)
		{
			if (m_MusicPlayer.GetPlayState() == P_PLAY)
			{
				int playProcess = m_pSliderPlayProcess->GetValue();
				m_MusicPlayer.Seek(playProcess);
			}

		}
	}
	else if( msg.sType == _T("itemactivate") ) 
	{	
		if( strCtrlName == kMusicItem ) 
		{			
			CMusicListItemUI *pMusicItem = static_cast<CMusicListItemUI*>(msg.pSender);	
			MEDIA *pMedia = pMusicItem->GetMedia();
			if (pMedia->name == _T("我的天空-Redrain仿酷狗") || pMedia->name == _T("lie to me"))
			{
				MessageBox(_T("这是Demo自动添加的音乐，没有设置正确的路径，你可以添加真实的音乐来测试"));
				return;
			}
			if(m_MusicPlayer.GetPlayState()!=P_STOP)
				m_MusicPlayer.OpenMedia(pMusicItem->GetMedia(), TRUE);
			else
				m_MusicPlayer.OpenMedia(pMusicItem->GetMedia(), FALSE);
			m_MusicPlayer.Play();
			if ( m_MusicPlayer.GetPlayState() != P_PLAY )
			{
				m_pCurMedia = NULL;
				SetPlayBtnState(false);
				return;
			}
			m_pCurMedia = pMusicItem->GetMedia();
			SetPlayBtnState(true);
		}

	}
	else if( msg.sType == _T("setfocus") ) 
	{	
		if( strCtrlName == kEdtTitleSearch ) 
		{
 			msg.pSender->SetBkColor(0xFFFFFFFE);
		}
	}
	else if( msg.sType == _T("killfocus") ) 
	{	
		if( strCtrlName == kEdtTitleSearch ) 
		{
 			msg.pSender->SetBkColor(0x00000000);
		}

	}	
	else if(msg.sType == _T("selectchanged"))
	{
		OnSelectChanged(msg);
	}
	else if(msg.sType == _T("menu"))
	{
		OnShowMenu(msg);
	}
	else if(msg.sType == DUI_MSGTYPE_BUTTON_HOT)
	{
		if( strCtrlName == kBtnMusicSound ) 
		{
			CPopupWidget* pWidget = new CPopupWidget;
			CPoint point;
			RECT rc	= m_pBtnMusicSound->GetPos();
			m_pBtnMusicSound->SetTag(m_MusicPlayer.GetVolume());
			point.x = rc.left - 3;
			point.y = rc.bottom + 1;
			pWidget->Init(_T("xmls\\Volume\\Volume.xml"), m_hWnd, &m_PaintManager, point, CHANGE_VOLUME, m_pBtnMusicSound);

			return;
		}	
	}

	__super::Notify(msg);
}

LRESULT CFrameWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_MENUCLICK)
	{
		CDuiString *strMenuName = (CDuiString*)wParam;
		bool bChecked = (bool)lParam;		 

		if ( *strMenuName == kMenuMainExit) 
		{
			SetLoopState(EXIT_APP);
			Close(IDCLOSE);
		}	
		else if( *strMenuName == kMenuWndTitleSetting )
		{
			CSettingDialog *pSettingDlg = new CSettingDialog(_T("xmls\\SettingsDialog.xml"),this);
			pSettingDlg->Create(m_hWnd,_T("选项设置"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
			pSettingDlg->CenterWindow();
			pSettingDlg->ShowModal();
			delete	pSettingDlg;
		}
		else if ( *strMenuName == kMenuLoopPalySingle) 
		{
			SetLoopState(PLAY_SINGLE);
		}
		else if ( *strMenuName == kMenuLoopLoopSingle) 
		{
			SetLoopState(LOOP_SINGLE);
		}
		else if ( *strMenuName == kMenuLoopLoopList) 
		{
			SetLoopState(LOOP_LIST);
		}
		else if ( *strMenuName == kMenuLoopPlayList) 
		{
			SetLoopState(PLAY_LIST);
		}
		else if ( *strMenuName == kMenuLoopRandom) 
		{
			SetLoopState(LOOP_RANDOM);
		}

		delete strMenuName;
	}
	else if (uMsg == WM_NCRBUTTONUP )
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		RECT rcCaption = m_PaintManager.GetCaptionRect();
		if( pt.x >= rcClient.left && pt.x < rcClient.right && pt.y >= rcClient.top && pt.y < rcCaption.bottom ) 
			m_PaintManager.SendNotify(m_pWndTitle, DUI_MSGTYPE_MENU, 0, 0);

	}
	else if( lParam == WM_LBUTTONDBLCLK ) 
	{
		ShowWindow(true);
	}
	else if(lParam == WM_RBUTTONUP)
	{
		::SetForegroundWindow(this->m_hWnd);

		CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
		CPoint point(0,0);
		GetCursorPos(&point);
		pMenu->Init(NULL, _T("xmls\\Menus\\Menu_Tray.xml"), point, &m_PaintManager, &m_MenuCheckInfo, true );

		::PostMessage(m_hWnd,WM_NULL,0,0);
	}
	
	bHandled = false;
	return 0;
}


void CFrameWnd::OnDropFiles(CFrameWnd *pMainWnd, HDROP hDrop)
{
	UINT nFileCount = ::DragQueryFile(hDrop, -1, NULL, 0);

 	TCHAR szFileName[_MAX_PATH] = _T("");
 	DWORD dwAttribute;
 	std::vector<CDuiString> vctString;
 
 	// 获取拖拽进来文件和文件夹
 	for (UINT i = 0; i < nFileCount; i++)
 	{
 		::DragQueryFile(hDrop, i, szFileName, sizeof(szFileName));
 		dwAttribute = ::GetFileAttributes(szFileName);
 
 		// 是否为文件夹
 		if ( dwAttribute & FILE_ATTRIBUTE_DIRECTORY )
 		{          
 			::SetCurrentDirectory(szFileName);       
 			CFileUtil::EnumerateFiles(vctString);	      
 		}
 		else
 		{
			if (CFileUtil::FindFileExt(szFileName, STR_SUPPORT_FILE_EXT))
 				vctString.push_back(szFileName);
 		}
 	}

	pMainWnd->AddMusicsToGroup(pMainWnd->GetMusicList(), _T("默认列表"), vctString);
	::DragFinish(hDrop);
}

void CFrameWnd::OnShowMenu( TNotifyUI &msg )
{
	CDuiString strName = msg.pSender->GetName();

	if(strName == kMenuWndTitle )
	{
		CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
		CPoint point(0,0);
		GetCursorPos(&point);
		pMenu->Init(NULL, _T("xmls\\Menus\\Menu_Main.xml"), point, &m_PaintManager, &m_MenuCheckInfo );

		return;
	}
	else if(strName == kMenuWndTitleSetting)
	{
		CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
		CPoint point(0,0);
		RECT rc	= msg.pSender->GetPos();
		point.x = rc.left;
		point.y = rc.bottom ;
		ClientToScreen(m_hWnd, &point);
		pMenu->Init(NULL, _T("xmls\\Menus\\Menu_Main.xml"), point, &m_PaintManager, &m_MenuCheckInfo );

		return;
	}
	else if(strName == kMenuMusicItem )
	{
		CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
		CPoint point = msg.ptMouse;
		ClientToScreen(m_hWnd, &point);
		pMenu->Init(NULL, _T("xmls\\Menus\\Menu_Music_Item.xml"), point, &m_PaintManager, &m_MenuCheckInfo );

		return;
	}
	else if(strName == kMenuMusicGroupButton || msg.pSender->GetInterface(kMusicListGroupUIInferFace)!= NULL )
	{
		CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
		CPoint point = msg.ptMouse;
		ClientToScreen(m_hWnd, &point);
		pMenu->Init(NULL, _T("xmls\\Menus\\Menu_Music_Group.xml"), point, &m_PaintManager, &m_MenuCheckInfo );

		return;
	}
	else if(strName == kMenuRadioItem )
	{
		CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
		CPoint point = msg.ptMouse;
		ClientToScreen(m_hWnd, &point);
		pMenu->Init(NULL, _T("xmls\\Menus\\Menu_Radio.xml"), point, &m_PaintManager, &m_MenuCheckInfo );

		return;
	}
	else if(strName == kBtnMusicLoop )
	{
		CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
		CPoint point(0,0);
		RECT rc	= m_pBtnMusicLoop->GetPos();
		point.x = rc.left;
		point.y = rc.bottom ;
		ClientToScreen(m_hWnd, &point);
		pMenu->Init(NULL, _T("xmls\\Menus\\Menu_List_Loop.xml"), point, &m_PaintManager, &m_MenuLoopCheckInfo );

		return;
	}

}

void CFrameWnd::OnSelectChanged(TNotifyUI& msg)
{
	if (m_pTabLeft == NULL)	return;

	CDuiString strName = msg.pSender->GetName();

	if(strName == kOptLeftMusic)
	{
		m_pTabLeft->SelectItem(0);
	}
	else if (strName == kOptLeftFavorite)
	{
		m_pTabLeft->SelectItem(1);
	}
	else if (strName == kOptLeftRadio)
	{
		m_pTabLeft->SelectItem(2);
	}
	else if (strName == kOptLeftLocal)
	{
		m_pTabLeft->SelectItem(3);
	}
	else if (strName == kOptLeftTemp)
	{
		m_pTabLeft->SelectItem(4);
	}
	else if (strName == kOptLeftDownload)
	{
		m_pTabLeft->SelectItem(5);
	}
	else if (strName == kOptLeftMore)
	{
		m_pTabLeft->SelectItem(6);
	}
	else if (strName == kOptTopMusic)
	{
		m_pTabTop->SelectItem(0);
		m_pWke->SetURL(kOptTopMusicURL);
	}
	else if (strName == kOptTopRadio)
	{
		m_pTabTop->SelectItem(0);
		m_pWke->SetURL(kOptTopRadioURL);
	}
	else if (strName == kOptTopMv)
	{
		m_pTabTop->SelectItem(0);
		m_pWke->SetURL(kOptTopMvURL);
	}
	else if (strName == kOptTopLive)
	{
		m_pTabTop->SelectItem(0);
		m_pWke->SetURL(kOptTopLiveURL);
	}
	else if (strName == kOptTopLrc)
	{
		m_pTabTop->SelectItem(1);

	}


}