#include "..\duilib.h"

#pragma comment(lib,"libzplay.lib")

int __stdcall CallbackFunc(void* instance, void *user_data, TCallbackMessage message, unsigned  param1, unsigned  param2);

CDuiString CMusicPlayer::TimeToString(int time)
{
	CDuiString sTime,sMin,sSec;
	if (time/60<10)
	{
		sMin.Format(L"0%d",time/60);
	}else
	{
		sMin.Format(L"%d",time/60);
	}
	
	if (time%60<10)
	{
		sSec.Format(L"0%d",time%60);
	}else
	{
		sSec.Format(L"%d",time%60);
	}

	sTime.Append(sMin);
	sTime.Append(L":");
	sTime.Append(sSec);
	return sTime;
}

TStreamTime CMusicPlayer::GetMusicLenght()
{
	TStreamInfo pInfo = {0};
	if (m_pZplayer->OpenFileW(m_CurMedia->pathFileName.GetData(), sfAutodetect))
		m_pZplayer->GetStreamInfo(&pInfo);

	return pInfo.Length;
}

TID3InfoExW CMusicPlayer::GetMusicID3Info()
{
	TID3InfoExW pID3Info = {0};
	if (m_pZplayer->OpenFileW(m_CurMedia->pathFileName.GetData(), sfAutodetect))
	{
		if(!m_pZplayer->LoadID3ExW(&pID3Info, 1))
			memset(&pID3Info,0 , sizeof(TID3InfoExW));
	}
	return pID3Info;
}

CMusicPlayer::CMusicPlayer(CFrameWnd *pMainFrame):
	m_pMainFrame(pMainFrame),
	m_CurMedia(NULL),
	m_pZplayer(NULL),
	m_CurLoopState(LOOP_LIST),
	m_PlayerState(P_STOP),
	m_volume(100),
	m_bIsManualSwitch(false)
{
	memset(&m_status, 0, sizeof(TStreamStatus)) ;
}

CMusicPlayer::~CMusicPlayer(void)
{
	if (m_pZplayer)
	{
	//	if (GetPlayState() != P_STOP)
	//		m_pZplayer->Stop();	
		m_pZplayer->Close();
		m_pZplayer->Release();		
	}
}

CFrameWnd* CMusicPlayer::GetMainFrame() const
{
	return m_pMainFrame;
}

void CMusicPlayer::InitPlayer(PLAY_PROC playProc, GET_MEDIA getNext, GET_MEDIA getPrev, GET_MEDIA getRandom)
{
	m_pZplayer = CreateZPlay();
	if (!m_pZplayer)
		return;

	m_pZplayer->SetCallbackFunc(CallbackFunc, 
		(TCallbackMessage)(MsgPlay|MsgStop|MsgStopAsync|MsgPlayAsync|MsgWaveBuffer), this);

	m_PlayProc = playProc;
	m_PlayNextMedia = getNext;
	m_PlayRandomMedia = getRandom;
	m_PlayPrevMedia = getPrev;

	// 回音效果
	TEchoEffect effect[2];
	effect[0].nLeftDelay = 1000;
	effect[0].nLeftEchoVolume = 20;
	effect[0].nLeftSrcVolume = 80;
	effect[0].nRightDelay = 500;
	effect[0].nRightEchoVolume = 20;
	effect[0].nRightSrcVolume = 80;
	effect[1].nLeftDelay = 300;
	effect[1].nLeftEchoVolume = 20;
	effect[1].nLeftSrcVolume = 0;
	effect[1].nRightDelay = 300;
	effect[1].nRightEchoVolume = 20;
	effect[1].nRightSrcVolume = 0;
	// set echo effects
	m_pZplayer->SetEchoParam(effect, 2);
}

 int __stdcall  CallbackFunc(void* instance, void *user_data, TCallbackMessage message, unsigned  param1, unsigned  param2)
{
	ZPlay *myinstance = (ZPlay*) instance;
	CMusicPlayer *player = (CMusicPlayer*)user_data;
	if (message & MsgStopAsync )
	{
		if (!player->m_bIsManualSwitch)
		{
			DUI__Trace(_T("已经播放完了"));
			player->Stop();
			switch(player->GetLoopState())
			{
			case PLAY_SINGLE:
				{
					player->Stop();
				}
				break;
			case LOOP_SINGLE:
				{
					player->Play();
				}
				break;
			case LOOP_LIST:
				{
					player->PlayNext();
				};
				break;
			case PLAY_LIST:
				{
					player->PlayNext();
				};
				break;
			case LOOP_RANDOM:
				{
					player->PlayRandom();
				}
				break;
			case EXIT_APP:
				{
					DUI__Trace(_T("AppExit!"));
				}
				break;
			}
		}
		else
		{
			player->m_bIsManualSwitch = FALSE;
		}
	}
	else if(message & MsgWaveBuffer)
	{
		UINT num = 0;

		TStreamTime ttm;
		player->m_pZplayer->GetPosition(&ttm);
		num = ttm.sec;

		player->m_PlayProc(player,MSG_PLAYING,(WPARAM)player->m_CurMedia,num,NULL);
	}
	return 0;
}

void CMusicPlayer::PlayNext()
{
	MEDIA* media = m_PlayNextMedia(this);

}

void CMusicPlayer::PlayPrev()
{
	MEDIA* media = m_PlayPrevMedia(this);
}

void CMusicPlayer::PlayRandom()
{
	MEDIA* media = m_PlayRandomMedia(this);
}

void CMusicPlayer::OpenMedia(MEDIA* media, BOOL IsManual/* = false*/)
{
	m_CurMedia = media;
	//m_CurMedia->totalTime = GetMusicLenght();
	m_bIsManualSwitch = IsManual ? TRUE : FALSE;
	m_PlayProc(this,MSG_OPEN,(WPARAM)m_CurMedia,NULL,NULL);
}


UINT CMusicPlayer::MessageBox( LPCTSTR lpText, UINT uIcon/* = MB_ICONWARNING */, LPCTSTR lpCaption /*= _T("提示")*/,UINT uType /*= MB_OK */)
{
	CMessageDialog MessageBox;
	return MessageBox.DuiMessageBox(m_pMainFrame, lpText, lpCaption, uType, uIcon);
}

void CMusicPlayer::Play()
{
	if (m_CurMedia == NULL || m_CurMedia->pathFileName == _T(""))
		return;
	if (GetPlayState() != P_STOP)
	{
		m_pZplayer->Stop();
	}
	
	if (m_pZplayer->OpenFileW(m_CurMedia->pathFileName.GetData(), sfAutodetect) == 0)
	{
		m_pZplayer->Close();
		MessageBox( L"文件不符合要求", MB_ICONERROR, _T("错误！"), MB_OK);
					
		return;
	}
	if (m_pZplayer->Play() == 0)
	{
		m_pZplayer->Close();
	}

	m_PlayProc(this,MSG_PLAY,(WPARAM)m_CurMedia,NULL,NULL);
}

void CMusicPlayer::Pause()
{
	if (m_CurMedia == NULL || m_CurMedia->pathFileName == _T(""))
		return;

	m_pZplayer->Pause();

}

void CMusicPlayer::Resume()
{
	if (m_CurMedia == NULL || m_CurMedia->pathFileName == _T(""))
		return;

	m_pZplayer->Resume();

}

void CMusicPlayer::Stop()
{
	if (m_CurMedia == NULL || m_CurMedia->pathFileName == _T(""))
		return;

	m_pZplayer->Stop();

	m_PlayProc(this,MSG_STOP,(WPARAM)m_CurMedia,NULL,NULL);
}
void CMusicPlayer::Seek(int sec)
{
	if (m_CurMedia == NULL || m_CurMedia->pathFileName == _T(""))
		return;

	TStreamTime ttm;
	ttm.ms = sec * 1000;  //这里需要设置毫秒，否则Seek函数会失效
	m_pZplayer->Seek(tfMillisecond,&ttm,smFromBeginning);
	
}

void CMusicPlayer::SetVolume(int volume)
{
	m_pZplayer->SetMasterVolume(volume, volume);
	m_volume = volume;
	m_PlayProc(this,MSG_VOLUME,(WPARAM)m_CurMedia,m_volume,NULL);
}

int CMusicPlayer::GetVolume()
{
	return m_volume;
}

int CMusicPlayer::GetBitrate(int fAverage)
{
	return m_pZplayer->GetBitrate(0);
}

void  CMusicPlayer::SetMixChannels(int fEnable)
{
	m_pZplayer->MixChannels(fEnable, 50, 50);
}

int  CMusicPlayer::GetMixChannels()
{
	m_pZplayer->GetStatus(&m_status); 
	return m_status.fChannelMix;
}

void  CMusicPlayer::SetEcho(int fEnable)
{
	m_pZplayer->EnableEcho(fEnable);
}

int	 CMusicPlayer::GetEcho()
{
	m_pZplayer->GetStatus(&m_status); 
	return m_status.fEcho;
}

void  CMusicPlayer::SetEqualizer(int fEnable)
{
	m_pZplayer->EnableEqualizer(fEnable);

	if(fEnable)
	{
		m_pZplayer->SetEqualizerBandGain(0, 10000); // boost low frequenfy band for 10 dB
		m_pZplayer->SetEqualizerBandGain(1, 2000); //boost for 2 dB
		m_pZplayer->SetEqualizerBandGain(2, -8000); // cut this band for 8 dB

		m_pZplayer->SetEqualizerBandGain(5, 8000); // boost this band for 8 dB
		m_pZplayer->SetEqualizerBandGain(6, 8000); // boost this band for 8 dB

	}	
}

int  CMusicPlayer::GetEqualizer()
{
	m_pZplayer->GetStatus(&m_status); 
	return m_status.fEqualizer;
}

bool CMusicPlayer::SetReverseMode(int fEnable)
{
	if(m_pZplayer->ReverseMode(fEnable) == 0)
		return false;
	return true;
}

int CMusicPlayer::GetReverseMode()
{
	m_pZplayer->GetStatus(&m_status); 
	return m_status.fReverse;
}

void  CMusicPlayer::SetTempo(UINT nTempo)
{
	m_pZplayer->SetTempo(nTempo);
}
UINT CMusicPlayer::GetTempo()
{
	return m_pZplayer->GetTempo();
}

void CMusicPlayer::SetPitch(UINT nPitch)
{
	m_pZplayer->SetPitch(nPitch);
}
UINT CMusicPlayer::GetPitch()
{
	return m_pZplayer->GetPitch();
}

void CMusicPlayer::SetRate(UINT nRate)
{
	m_pZplayer->SetRate(nRate);
}

UINT CMusicPlayer::GetRate()
{
	return m_pZplayer->GetRate();
}

void  CMusicPlayer::SetStereoCut(int fEnable)
{
	m_pZplayer->StereoCut(fEnable, 0, 0);
}

int CMusicPlayer::GetStereoCut()
{
	m_pZplayer->GetStatus(&m_status); 
	return m_status.fVocalCut;
}

void CMusicPlayer::SetLoopState(LOOP_STATE state)
{
	m_CurLoopState = state;
}
LOOP_STATE CMusicPlayer::GetLoopState()
{
	return m_CurLoopState;
}

PLAYER_STATE CMusicPlayer::GetPlayState()
{
	m_pZplayer->GetStatus(&m_status); 
	if(m_status.fPlay)
		return P_PLAY;
	else if(m_status.fPause)
		return P_PAUSE;
	else
		return P_STOP;	
}