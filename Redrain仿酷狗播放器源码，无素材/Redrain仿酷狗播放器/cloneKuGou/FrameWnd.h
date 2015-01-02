#pragma once

#include "DropTargetEx.h"
#ifdef _DEBUG
#       pragma comment(lib, "../Common/lib/UiLib_ud.lib")
#else
#       pragma comment(lib, "../Common/lib/UiLib_u.lib")
#endif

class CFrameWnd: public WindowImplBase
{
public:
	explicit CFrameWnd(LPCTSTR pszXMLPath);

	LPCTSTR GetWindowClassName() const;
	CDuiString GetSkinFile();
	CDuiString GetSkinFolder();
// 	UILIB_RESOURCETYPE GetResourceType() const;
// 	LPCTSTR GetResourceID() const;

     void InitWindow();
     void Notify(TNotifyUI& msg);

	 CControlUI* CreateControl(LPCTSTR pstrClassName);
	 LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	 CPaintManagerUI* GetMainWndPaintManager();

	 void SetListTrans(int nValue);
	 int  GetListTrans() const;
	 void SetIsSkinTrans(bool bTrans);
	 bool GetIsSkinTrans() const;
	 void SetSkinTrans(int nValue);
	 int  GetSkinTrans() const;
	 void SetBkColor(DWORD dwBackColor);
	 DWORD GetBkColor() const;
	 void SetBkImage(CDuiString strBkImage);
	 LPCTSTR GetBkImage();
	 void LoadDefaultState();

	 //歌曲播放回调函数
	 static void player_proc(CMusicPlayer* pPlayer,PLAY_MSG msg,WPARAM wParam,LPARAM lParam,void* arg);
	 static MEDIA* PlayNextMedia(CMusicPlayer* pPlayer);
	 static MEDIA* PlayPrevMedia(CMusicPlayer* pPlayer);
	 static MEDIA* PlayRandomMedia(CMusicPlayer* pPlayer);

	 //主要用于获取音乐列表的范围来判断拖动时的图标样式
	 bool GetLeftListPos(RECT & rc) const;

	 //添加窗体开启时的动画播放效果,测试后发现动画特效和wke不兼容，要么注释OnPlayAnimation的调用，要么让wke的SetUrl不工作。
	 void OnPlayAnimation();

	 void OnSelectChanged( TNotifyUI &msg );
	 void OnShowMenu( TNotifyUI &msg );
	 static void OnDropFiles(CFrameWnd *pMainWnd, HDROP hDrop);

	 void AddMusicsToGroup(CMusicListUI* pMusicList, LPCTSTR szGroupName, 
		 vector<CDuiString>& fileNames, CDuiString szLogoPath = kDefaultLogoPath1,
		 bool bCheckExist = true, bool bAutoRefreshIndex = true);
	 TStreamTime GetMusicLenght(CDuiString &MusicPath);
	 MEDIA* GetCurMedia();
	 CMusicListUI* GetMusicList();
	 void SetLoopState(LOOP_STATE state);
	 LOOP_STATE GetLoopState();
	 void SetPlayBtnState(bool bIsPlay);

	 UINT MessageBox( LPCTSTR lpText, UINT uIcon = MB_ICONWARNING , LPCTSTR lpCaption = _T("提示"),UINT uType = MB_OK );

private:
	CDuiString		m_strXMLPath;
	CDropTargetEx	m_DropTarget;	//使窗体支持拖放操作
	CWndShadow		m_WndShadow;	//窗体阴影，默认并没有使用
	CDuiString		m_strBkImage;   //当隐藏音乐库时保存原来的背景信息用来恢复

	//音乐控制布局
	CMusicPlayer	m_MusicPlayer;	
	MEDIA			*m_pCurMedia;
	ZPlay			*m_pZplayer;		 //播放器指针,用来计算音乐长度
	CButtonUI		*m_pBtnMusicPlay;
	CButtonUI		*m_pBtnMusicPause;
	CButtonUI		*m_pBtnMusicLrc;
	CButtonUI		*m_pBtnAddMusic;
	CButtonUI		*m_pBtnMusicSound;
	CButtonUI		*m_pBtnMusicLoop;
	CLabelUI		*m_pLblPlayTime;
	CLabelUI		*m_pLblTotalTime;
	CSliderUI		*m_pSliderPlayProcess;
	CLabelUI		*m_pLblMainWndTitile;  //主窗体的标题，当播放音乐时应该改为音乐的名字
		

	//右侧标题栏
	CButtonUI				*m_pBtnGoBack;
	CButtonUI				*m_pBtnRefresh;
	CTabLayoutUI			*m_pTabTop;


	//各大布局
	CVerticalLayoutUI		*m_pBackGround;
	CHorizontalLayoutUI		*m_pWndTitle;
	CVerticalLayoutUI		*m_pMainTitleRight;
	CVerticalLayoutUI		*m_pMainMusicBank;
	CHorizontalLayoutUI		*m_pMainShowRight;
	CHorizontalLayoutUI		*m_pMainStatus;
	CLabelUI				*m_pLblBottomInfo;


	//左侧Tab栏控件
	CVerticalLayoutUI		*m_pLeftTabBk;
	CVerticalLayoutUI		*m_pLeftTabListBk;
	CAnimationTabLayoutUI	*m_pTabLeft;
	CMusicListUI			*m_pMusicList;
	CMusicListUI			*m_pRadioList;
	CMusicListUI			*m_pLocaltion;
	CMusicListUI			*m_pDownload;
	CTileLayoutUI			*m_pMoreList;


	//wke浏览器控件
	CWkeWebkitUI	*m_pWke;

	map<CDuiString,bool> m_MenuCheckInfo;	//保存菜单的设置信息,应该为每个菜单都声明一个map,我这里只做了循环方式菜单的功能
	map<CDuiString,bool> m_MenuLoopCheckInfo;

	int				m_nListTrans;
	bool			m_bSkinTrans;
	int				m_nSkinTrans;
};