#pragma once
using namespace libZPlay;

enum PLAY_MSG
{
	MSG_OPEN = 0,    //打开
	MSG_PLAY,        //播放
	MSG_PLAYING,     //正在播放
	MSG_PAUSE,       //暂停
	MSG_RESUME,      //继续
	MSG_STOP,        //停止
	MSG_SEEK,        //播放位置
	MSG_PREV,        //上一首
	MSG_NEXT,        //下一首
	MSG_VOLUME,      //音量
	MSG_SET_LOOP     //设置循环状态
};

enum LOOP_STATE
{
	LOOP_SINGLE = 1,    //单曲循环
	PLAY_SINGLE = 2,    //单曲播放
	LOOP_LIST = 4,      //列表循环
	PLAY_LIST = 8,      //顺序播放
	LOOP_RANDOM = 16,   //随机播放
	EXIT_APP = 32		//程序退出
};

class CMusicListItemUI;
typedef struct tagMEDIA
{
    CDuiString  name;              //名称
	CDuiString  pathFileName;      //文件路径名
	CDuiString  logoPath;		   //logo图片的路径
	CDuiString	exName;			   //扩展名
	UINT		playNum;		   //播放次数
	CDuiString	size;			   //大小  (M)
	TStreamTime totalTime;         //总时间	
	CMusicListItemUI* pControl;    //绑定的控件指针
	CDuiString  tag;			   //保留字段，可以存放额外信息
	tagMEDIA():pControl(NULL)
	{
		ZeroMemory(&totalTime, sizeof(totalTime));
	}
}MEDIA;

enum PLAYER_STATE
{
	P_PLAY = 0,
	P_PAUSE,
	P_STOP
};

class CMusicPlayer;
class CFrameWnd;
typedef void (*PLAY_PROC)(CMusicPlayer*,PLAY_MSG,WPARAM,LPARAM,void*);
typedef MEDIA* (*GET_MEDIA)(CMusicPlayer*);
unsigned __stdcall playing_proc( void* pArguments );
class CMusicPlayer
{
public:
	CMusicPlayer(CFrameWnd *MainFrame);
	~CMusicPlayer(void);
	CFrameWnd *GetMainFrame() const;

	static CDuiString TimeToString(int ms);
    TStreamTime GetMusicLenght();	 //获取歌曲事件长度
	TID3InfoExW GetMusicID3Info();	 //歌曲的的信息，作者，标题，年代，详见下方注释

	void InitPlayer(PLAY_PROC playProc,GET_MEDIA getNext,GET_MEDIA getPrev,GET_MEDIA getRandom);  //初始化
	void OpenMedia(MEDIA* media, BOOL IsManual = false);    //打开媒体对象，IsManual手动操作打开媒体？
	void Play();
	void Pause();
	void Resume();					  //从暂停恢复
	void Stop();
	void Seek(int);					  //设置播放位置
	void PlayNext();				  //播放下一首
	void PlayPrev();				  //播放上一首
	void PlayRandom();				  //播放随机

	void SetVolume(int volume);       //设置音量
	int  GetVolume();
	int  GetBitrate(int fAverage);		//获取比特率
	void SetMixChannels(int fEnable);	//设置通道混合
	int  GetMixChannels();
	void SetEcho(int fEnable);			//设置回音
	int	 GetEcho();	
	void SetEqualizer(int fEnable);		//设置均衡器
	int  GetEqualizer();
	void SetStereoCut(int fEnable);		//设置去掉人声
	int  GetStereoCut();
	bool SetReverseMode(int fEnable);	//设置倒退播放
	int  GetReverseMode();
	void SetTempo(UINT nTempo);			//设置速度
	UINT GetTempo();
	void SetPitch(UINT nPitch);			//设置音调
	UINT GetPitch();
	void SetRate(UINT nRate);			//设置速率
	UINT GetRate();	
	void SetLoopState(LOOP_STATE state);//设置循环状态
	LOOP_STATE GetLoopState();
	PLAYER_STATE GetPlayState();		//获取播放状态


	UINT CMusicPlayer::MessageBox( LPCTSTR lpText, UINT uIcon = MB_ICONWARNING , LPCTSTR lpCaption = _T("提示"),UINT uType = MB_OK );

	PLAY_PROC m_PlayProc;				//播放回调函数指针
	GET_MEDIA m_PlayNextMedia;			//获取下一首Media的回调函数
	GET_MEDIA m_PlayPrevMedia;			//获取上一首Media回调函数
	GET_MEDIA m_PlayRandomMedia;			//获取当前列表随机一首Media回调函数

	MEDIA *m_CurMedia;					//当前打开的Media
	ZPlay *m_pZplayer;					//播放器指针
	BOOL		m_bIsManualSwitch;		//手动切换音乐，则不执行CallBack中的相关代码
private:
	LOOP_STATE m_CurLoopState;
	PLAYER_STATE m_PlayerState;
	TStreamStatus m_status;	
	int			m_volume;	

	CFrameWnd	*m_pMainFrame;
};