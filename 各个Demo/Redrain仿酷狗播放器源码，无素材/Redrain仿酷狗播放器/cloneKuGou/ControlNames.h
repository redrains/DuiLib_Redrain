#ifndef CONTROL_NAMES_H
#define CONTROL_NAMES_H

const TCHAR STR_MUSIC_FILE_FILTER[] =
	_T("Music Files(*.mp3,*.ogg,etc)\0*.mp3;*.ogg;*.wav;*.aac;*.oga;*.ac3;*.flac;\0\0");

const TCHAR STR_SUPPORT_FILE_EXT[] =
	_T("*.mp3;*.ogg;*.wav;*.aac;*.oga;*.ac3;*.flac;");

//////////////////////////////////////////////////////

const TCHAR MainWndTitle[] =		   _T("Redrain 音乐盒 测试版1.0");

const TCHAR kMainWndDefaultBkImage[] = _T("UI\\BKImage\\2.jpg");
const TCHAR kDefaultLogoPath1[] =	   _T("UI\\LeftTab\\listitem\\logo.png");
const TCHAR kDefaultLogoPath2[] =	   _T("UI\\LeftTab\\listitem\\logo2.png");
const DWORD kMainWndDefaultBkColor   = 0xFF47484B;
const int	kMainWndDefaultcx	     = 1000;
const int	kMainWndDefaultMincx	 = 960;
const int	kMainWndDefaultHidecx	 = 365;
const int	kMainWndDefaultcy		 = 670;

const int	kMainWndDefaultListTrans = 77;
////////////////////////////////////////////////////////
//  Menu

const TCHAR kMenuWndTitle[] =				_T("MainWndTitle");
	const TCHAR kMenuWndTitleSetting[] =	_T("Menu_Main_Settings");
const TCHAR kMenuTray[] =					_T("AppTray");
	const TCHAR kMenuMainExit[] =			_T("Menu_Main_Exit");
const TCHAR kMenuMusicItem[] =				_T("MusicItem");
const TCHAR kMenuMusicGroupButton[] =		_T("GroupMenuBtn");
const TCHAR kMenuRadioItem[] =				_T("RadioItem");

	const TCHAR kMenuLoopPalySingle[] =		_T("Menu_Loop_PlsySingle");
	const TCHAR kMenuLoopLoopSingle[] =		_T("Menu_Loop_SingleLoop");	
	const TCHAR kMenuLoopLoopList[] =		_T("Menu_Loop_ListLoop");
	const TCHAR kMenuLoopPlayList[] =		_T("Menu_Loop_PlayList");
	const TCHAR kMenuLoopRandom[] =			_T("Menu_Loop_Random");

////////////////////////////////////////////////////////
//  Music_Control

const TCHAR kLblPlayTime[] =				_T("lbl_play_time");
const TCHAR kLblTotalTime[] =				_T("lbl_total_time");
const TCHAR kSliderPlayProcess[] =			_T("slider_play_process");
const TCHAR kBtnMusicLrc[] =				_T("btn_music_lrc");
const TCHAR kBtnMusicPlay[] =				_T("btn_music_play");
const TCHAR kBtnMusicPause[] =				_T("btn_music_pause");
const TCHAR kBtnMusicSound[] =				_T("btn_music_volume");
const TCHAR kBtnMusicPrev[] =				_T("btn_music_prev");
const TCHAR kBtnMusicNext[] =				_T("btn_music_next");
const TCHAR kBtnMusicLoop[] =				_T("btn_music_loop");
const TCHAR kBtnAddMusic[] =				_T("btn_add_music");
	
//////////////////////////////////////////////////////////
//标题栏杂项

const TCHAR kWndBackGround[] =		_T("background");

const TCHAR kLblMainWndTitle[] =	_T("Lbl_Main_Title");

const TCHAR kBtnSkinPicker[] =		_T("Btn_Title_SkinPicker");
const TCHAR kBtnTitleHideRight[] =	_T("Btn_Main_HideRight");
const TCHAR kBtnTitleShowRight[] =	_T("Btn_Main_ShowRight");
const TCHAR kBtnTitleRemote[] =		_T("Btn_Title_Remote");
const TCHAR kBtnGoBack[] =			_T("Btn_Goback");
const TCHAR kBtnRefresh[] =			_T("Btn_Refresh");

const TCHAR kEdtTitleSearch[] =		_T("Edt_Title_Search");
const TCHAR kBtnTitleSearch[] =		_T("Btn_Title_Search");
	
/////////////////////////////////////////////////////////

const TCHAR kOptTopMusic[] =		_T("TopTabMusic");
const TCHAR kOptTopRadio[] =		_T("TopTabRadio");
const TCHAR kOptTopMv[]	   =		_T("TopTabMv");
const TCHAR kOptTopLive[] =			_T("TopTabLive");
const TCHAR kOptTopLrc[] =			_T("TopTabLrc");

const TCHAR kOptTopMusicURL[] =		_T("http://www.kugou.com/");
const TCHAR kOptTopRadioURL[] =		_T("http://www.kugou.com/fmweb/html/index.html");
const TCHAR kOptTopMvURL[]	   =	_T("http://www.kugou.com/mvweb/html/");
const TCHAR kOptTopLiveURL[] =		_T("http://fanxing.kugou.com/index.php?countId=15738&countRnd=_fx_countRnd_1406085000");

/////////////////////////////////////////////////////////

const TCHAR kLeftTabBk[] =			_T("LeftTabBk");
const TCHAR kLeftTabListBk[] =		_T("LeftTabListBk");

const TCHAR kOptLeftMusic[] =		_T("LeftTabMusic");
	const TCHAR kMusicList[] =		_T("MusicList");
	const TCHAR kMusicItem[] =		_T("MusicItem");
	
const TCHAR kOptLeftFavorite[] =	_T("LeftTabFavorite");
	const TCHAR kFavorite[] =		_T("Favorite");
const TCHAR kOptLeftRadio[] =		_T("LeftTabRadio");
	const TCHAR kRadioGroup[] =		_T("RadioGroup");
	const TCHAR kRadioList[] =		_T("RadioList");
const TCHAR kOptLeftLocal[] =		_T("LeftTabLocal");
const TCHAR kOptLeftTemp[] =		_T("LeftTabTemp");
const TCHAR kOptLeftDownload[] =	_T("LeftTabDownload");
	const TCHAR kDownloadList[] =	_T("DownloadList");
const TCHAR kOptLeftMore[] =		_T("LeftTabMore");
	const TCHAR kMorecList[] =		_T("MoreList");

/////////////////////////////////////////////////////////
//	主界面的各个外层大布局

const TCHAR kMainTitleRight[] = _T("Main_Title_Right");
const TCHAR kMainMusicBank[] =	_T("Main_Music_Bank");
const TCHAR kMainShowRight[] =	_T("Main_Show_Right");
const TCHAR kLblBottomInfo[] =	_T("lbl_Main_Bottom_Info");
const TCHAR kMainStatus[] =		_T("Main_Status");

const TCHAR kTabLeft[] =		_T("LeftTab");
const TCHAR kTabTop[] =			_T("TopTab");

/////////////////////////////////////////////////////////
//	音乐信息弹出窗体
const TCHAR kMusicInfoLogo[] =		_T("Music_Info_Logo");
const TCHAR kMusicInfoMusicName[] =	_T("Music_Info_MusicName");
const TCHAR kMusicInfoMusicSize[] =	_T("Music_Info_MusicSize");
const TCHAR kMusicInfoMusicFormat[] =_T("Music_Info_MusicFormat");
const TCHAR kMusicInfoMusicNum[] =	_T("Music_Info_MusicNum");

#endif  //CONTROL_NAMES_H