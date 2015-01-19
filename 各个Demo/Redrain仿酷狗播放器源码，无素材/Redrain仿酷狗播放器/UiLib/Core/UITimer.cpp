#include "StdAfx.h"
#include "UITimer.h"

namespace UiLib{

	static void CALLBACK TimerProc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
	{
		CDuiTimerBase* pTimerUI = (CDuiTimerBase*)dwUser;

		if(!pTimerUI){
			timeKillEvent(uTimerID);
			timeEndPeriod(uMsg);
			return;
		}

		if(pTimerUI->GetObj() && pTimerUI->GetFun() && pTimerUI->GetParam() )
			pTimerUI->OnTimer(pTimerUI,pTimerUI->GetParam());
		else if(pTimerUI->GetHwnd() || pTimerUI->GetLParam())
			pTimerUI->OnTimer(pTimerUI,pTimerUI->GetHwnd(),pTimerUI->GetLParam(),pTimerUI->GetWParam());
		else
			pTimerUI->OnTimer(pTimerUI);

		pTimerUI->CalInterval();

		if(pTimerUI->FinshTimered()){
			pTimerUI->KillDuiTimer();
		}
	}

	CDuiTimerBase::CDuiTimerBase( void )
	{
		m_pObject		= NULL;
		m_pFun			= NULL;
		m_pParam		= NULL;
		m_iCurTimer		= NULL;
		m_iInterval		= NULL;
		m_iTotalTimer	= NULL;
		m_hWnd			= NULL;
		m_lParam		= NULL;
		m_wParam		= NULL;
		m_bAutoStart	= true;
		m_bLoop			= false;
		m_bReverse		= false;
		m_bInverted		= false;
	}

	CDuiTimerBase::CDuiTimerBase( void* pObj,void* pFun,int iInterval,int iTotalTimer /*= NULL*/,bool bAutoRun /*= true*/,bool bLoop /*= false*/,bool bRevers /*= false*/ )
	{
		SetTimerParam(pObj,pFun,NULL,iInterval,iTotalTimer,bAutoRun,bLoop,bRevers);

		TIMECAPS tc;
		if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR)
		{
			m_uTimerAccuracy=min(max(tc.wPeriodMin,1),tc.wPeriodMax);
			timeBeginPeriod(m_uTimerAccuracy);
		}
	}


	CDuiTimerBase::CDuiTimerBase( void* pObj,void* pFun,void* pParam,int iInterval,int iTotalTimer /*= NULL*/,bool bAutoRun /*= true*/,bool bLoop /*= false*/,bool bRevers /*= false*/ )
	{
		SetTimerParam(pObj,pFun,pParam,iInterval,iTotalTimer,bAutoRun,bLoop,bRevers);

		TIMECAPS tc;
		if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR)
		{
			m_uTimerAccuracy=min(max(tc.wPeriodMin,1),tc.wPeriodMax);
			timeBeginPeriod(m_uTimerAccuracy);
		}
	}

	CDuiTimerBase::CDuiTimerBase( void* pObj,void* pFun,HWND hWnd,LPARAM lParam,WPARAM wParam,int iInterval,int iTotalTimer /*= NULL*/,bool bAutoRun /*= true*/,bool bLoop /*= false*/,bool bRevers /*= false*/ )
	{
		SetTimerParam(pObj,pFun,hWnd,lParam,wParam,iInterval,iTotalTimer,bAutoRun,bLoop,bRevers);

		TIMECAPS tc;
		if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR)
		{
			m_uTimerAccuracy=min(max(tc.wPeriodMin,1),tc.wPeriodMax);
			timeBeginPeriod(m_uTimerAccuracy);
		}
	}

	CDuiTimerBase::CDuiTimerBase( HWND hWnd,LPARAM lParam,WPARAM wParam,int iInterval,int iTotalTimer /*= NULL*/,bool bAutoRun /*= true*/,bool bLoop /*= false*/,bool bRevers /*= false*/ )
	{
		SetTimerParam(hWnd,lParam,wParam,iInterval,iTotalTimer,bAutoRun,bLoop,bRevers);

		TIMECAPS tc;
		if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR)
		{
			m_uTimerAccuracy=min(max(tc.wPeriodMin,1),tc.wPeriodMax);
			timeBeginPeriod(m_uTimerAccuracy);
		}
	}

	CDuiTimerBase::CDuiTimerBase( const CDuiTimerBase& rhs )
	{
		m_pObject		= rhs.GetObj();
		m_pFun			= rhs.GetFun();
		m_pParam		= rhs.GetParam();
		m_iCurTimer		= rhs.GetCurTimer();
		m_iInterval		= rhs.GetInterval();
		m_iTotalTimer	= rhs.GetTotalTimer();
		m_uTimerID		= rhs.GetTimerID();
		m_uTimerAccuracy= rhs.GetTimerAccuracy();
		m_hWnd			= rhs.GetHwnd();
		m_lParam		= rhs.GetLParam();
		m_wParam		= rhs.GetWParam();
		m_bAutoStart	= rhs.GetAutoStart();
		m_bLoop			= rhs.IsLoop();
		m_bReverse		= rhs.IsRevers();
		m_bInverted		= rhs.IsInverted();
	}

	CDuiTimerBase::~CDuiTimerBase( void )
	{
		InnerKillTimer();
	}

	bool CDuiTimerBase::Equals( const CDuiTimerBase& rhs ) const
	{
		if(m_pObject != rhs.GetObj())
			return false;
		else if(m_pFun != rhs.GetFun())
			return false;
		else if(m_pParam != rhs.GetParam())
			return false;
		else if(m_iInterval != rhs.GetInterval())
			return false;
		else if(m_iTotalTimer != rhs.GetTotalTimer())
			return false;
		else if(m_hWnd != rhs.GetHwnd())
			return false;
		else if(m_lParam != rhs.GetLParam())
			return false;
		else if(m_wParam != rhs.GetWParam())
			return false;

		return true;
	}

	void CDuiTimerBase::SetTimerParam( void* pObj,void* pFun,void* pParam,int iInterval,int iTotalTimer /*= NULL*/,bool bAutoRun /*= true*/,bool bLoop /*= false*/,bool bRevers /*= false*/ )
	{
		m_pObject		= pObj;
		m_pFun			= pFun;
		m_pParam		= pParam;
		m_iCurTimer		= iInterval < iTotalTimer?iInterval:iTotalTimer;
		m_iInterval		= iInterval < 0?0:iInterval;
		m_iTotalTimer	= m_iInterval > iTotalTimer?m_iInterval:iTotalTimer;
		m_iTotalTimer	= iTotalTimer == NULL ?NULL:m_iTotalTimer;
		m_hWnd			= NULL;
		m_lParam		= NULL;
		m_wParam		= NULL;
		m_bAutoStart	= bAutoRun;
		m_bLoop			= bLoop;
		m_bReverse		= bRevers;
		m_bInverted		= iInterval < iTotalTimer?false:true;
	}

	void CDuiTimerBase::SetTimerParam( void* pObj,void* pFun,HWND hWnd,LPARAM lParam,WPARAM wParam,int iInterval,int iTotalTimer /*= NULL*/,bool bAutoRun /*= true*/,bool bLoop /*= false*/,bool bRevers /*= false*/ )
	{
		m_pObject		= pObj;
		m_pFun			= pFun;
		m_pParam		= NULL;
		m_iCurTimer		= iInterval < iTotalTimer?iInterval:iTotalTimer;
		m_iInterval		= iInterval < 0?0:iInterval;
		m_iTotalTimer	= m_iInterval > iTotalTimer?m_iInterval:iTotalTimer;
		m_iTotalTimer	= iTotalTimer == NULL ?NULL:m_iTotalTimer;
		m_hWnd			= hWnd;
		m_lParam		= lParam;
		m_wParam		= wParam;
		m_bAutoStart	= bAutoRun;
		m_bLoop			= bLoop;
		m_bReverse		= bRevers;
		m_bInverted		= iInterval < iTotalTimer?false:true;
	}

	void CDuiTimerBase::SetTimerParam( HWND hWnd,LPARAM lParam,WPARAM wParam,int iInterval,int iTotalTimer /*= NULL*/,bool bAutoRun /*= true*/,bool bLoop /*= false*/,bool bRevers /*= false*/ )
	{
		m_pObject		= NULL;
		m_pFun			= NULL;
		m_pParam		= NULL;
		m_iCurTimer		= iInterval < iTotalTimer?iInterval:iTotalTimer;
		m_iInterval		= iInterval < 0?0:iInterval;
		m_iTotalTimer	= m_iInterval > iTotalTimer?m_iInterval:iTotalTimer;
		m_iTotalTimer	= iTotalTimer == NULL ?NULL:m_iTotalTimer;
		m_hWnd			= hWnd;
		m_lParam		= lParam;
		m_wParam		= wParam;
		m_bAutoStart	= bAutoRun;
		m_bLoop			= bLoop;
		m_bReverse		= bRevers;
		m_bInverted		= iInterval < iTotalTimer?false:true;
	}

	UINT CDuiTimerBase::CalInterval()
	{
		if(!IsInverted() && GetInterval() < GetTotalTimer()){
			if(GetCurTimer() + GetInterval() > GetTotalTimer() && GetCurTimer() < GetTotalTimer())
				return m_iCurTimer = GetTotalTimer();
			else if(GetCurTimer() + GetInterval() > GetTotalTimer() && IsRevers()){
				m_bInverted	= true;
				m_iCurTimer -= GetInterval();
				return GetCurTimer();
			}
			return m_iCurTimer += GetInterval();
		}
		else if(IsInverted() && GetInterval() < GetTotalTimer()){
			if(GetCurTimer() - GetInterval() < 0 && GetCurTimer() > 0)
				return m_iCurTimer = 0;
			else if(GetCurTimer() - GetInterval() < 0 && IsRevers()){
				m_bInverted = false;
				m_iCurTimer += GetInterval();
				return GetCurTimer();
			}
			return m_iCurTimer -= GetInterval();
		}

		return GetCurTimer();
	}

	bool CDuiTimerBase::FinshTimered()
	{
		if(!IsLoop() && GetInterval() < GetTotalTimer()){
			if(!IsRevers())
				return GetCurTimer() > GetTotalTimer();
			else if(IsRevers())
				return GetCurTimer() < 0;
		}
		else if(IsLoop() && GetInterval() < GetTotalTimer()){
			if(!IsRevers() && GetCurTimer() > GetTotalTimer())
				m_iCurTimer = 0;
			else if(IsRevers() && GetCurTimer() < 0)
				m_iCurTimer = GetTotalTimer();
		}
		if(GetInterval() >= GetTotalTimer())
			m_iCurTimer += GetInterval();

		return false;
	}

	bool CDuiTimerBase::InnerSetTimer()
	{
		if(m_uTimerID)
			InnerKillTimer();

		m_uTimerID		= timeSetEvent(m_iInterval,m_uTimerAccuracy,TimerProc,(DWORD)this,TIME_PERIODIC);

		if(m_uTimerID)
			return true;

		m_pObject		= NULL;
		m_pFun			= NULL;

		return false;
	}

	void CDuiTimerBase::InnerKillTimer()
	{
		timeKillEvent(m_uTimerID);
		timeEndPeriod(m_uTimerAccuracy);
		m_uTimerID = NULL;
		m_uTimerAccuracy = NULL;

		m_iCurTimer		= m_iInterval;
	}

	CTimerSource::~CTimerSource()
	{
		for( int i = 0; i < m_aDuiTimers.GetSize(); i++ ) {
			CDuiTimerBase* pObject = static_cast<CDuiTimerBase*>(m_aDuiTimers[i]);
			if( pObject) delete pObject;
			pObject = NULL;
		}
	}

	CTimerSource::operator bool()
	{
		return m_aDuiTimers.GetSize() > 0;
	}

	void CTimerSource::operator+= (const CDuiTimerBase& _DuiTimer)
	{ 
		for( int i = 0; i < m_aDuiTimers.GetSize(); i++ ) {
			CDuiTimerBase* pDuiTimer = static_cast<CDuiTimerBase*>(m_aDuiTimers[i]);
			if( pDuiTimer && pDuiTimer->Equals(_DuiTimer) )
				return;
		}

		CDuiTimerBase* pDuiTimer = _DuiTimer.Copy();
		m_aDuiTimers.Add(pDuiTimer);
		if(pDuiTimer->GetAutoStart())
			pDuiTimer->SetDuiTimer();
	}

	void CTimerSource::operator+= (CDuiTimerBase* _DuiTimer)
	{ 
		if(!_DuiTimer)
			return;

		for( int i = 0; i < m_aDuiTimers.GetSize(); i++ ) {
			CDuiTimerBase* pDuiTimer = static_cast<CDuiTimerBase*>(m_aDuiTimers[i]);
			if(pDuiTimer == _DuiTimer)
				return;

			if( pDuiTimer && pDuiTimer->Equals(*_DuiTimer)){
				m_aDuiTimers.Remove(i);
				delete pDuiTimer;
			}
		}

		m_aDuiTimers.Add(_DuiTimer);
		if(_DuiTimer->GetAutoStart())
			_DuiTimer->SetDuiTimer();
	}

	void CTimerSource::operator-= (const CDuiTimerBase& _DuiTimer) 
	{
		for( int i = 0; i < m_aDuiTimers.GetSize(); i++ ) {
			CDuiTimerBase* pDuiTimer = static_cast<CDuiTimerBase*>(m_aDuiTimers[i]);
			if( pDuiTimer && pDuiTimer->Equals(_DuiTimer) ) {
				delete pDuiTimer;
				m_aDuiTimers.Remove(i);
				return;
			}
		}
	}

	void CTimerSource::operator-= (const CDuiTimerBase* _DuiTimer) 
	{
		if(!_DuiTimer)
			return;

		for( int i = 0; i < m_aDuiTimers.GetSize(); i++ ) {
			CDuiTimerBase* pDuiTimer = static_cast<CDuiTimerBase*>(m_aDuiTimers[i]);
			if( pDuiTimer && (pDuiTimer->Equals(*_DuiTimer) || pDuiTimer == _DuiTimer)) {
				delete pDuiTimer;
				m_aDuiTimers.Remove(i);
				break;
			}
		}
		delete _DuiTimer;
		_DuiTimer = NULL;
	}

	void CTimerSource::KillTimers()
	{
		if(!m_aDuiTimers.GetSize())
			return;

		for( int i = 0; i < m_aDuiTimers.GetSize(); i++ ) {
			CDuiTimerBase* pDuiTimer = static_cast<CDuiTimerBase*>(m_aDuiTimers[i]);
			if( !pDuiTimer)
				continue;

			pDuiTimer->KillDuiTimer();
		}
	}

	void CTimerSource::SetTimers()
	{
		if(!m_aDuiTimers.GetSize())
			return;

		for( int i = 0; i < m_aDuiTimers.GetSize(); i++ ) {
			CDuiTimerBase* pDuiTimer = static_cast<CDuiTimerBase*>(m_aDuiTimers[i]);
			if( !pDuiTimer)
				continue;

			pDuiTimer->SetDuiTimer();
		}
	}

	void CTimerSource::ResetTimers()
	{
		for( int i = 0; i < m_aDuiTimers.GetSize(); i++ ) {
			CDuiTimerBase* pDuiTimer = static_cast<CDuiTimerBase*>(m_aDuiTimers[i]);
			if( !pDuiTimer)
				continue;

			pDuiTimer->KillDuiTimer();
			pDuiTimer->SetDuiTimer();
		}
	}

	void CTimerSource::RemoveTimerAll()
	{
		for( int i = 0; i < m_aDuiTimers.GetSize(); i++ ) {
			CDuiTimerBase* pDuiTimer = static_cast<CDuiTimerBase*>(m_aDuiTimers[i]);
			if( !pDuiTimer)
				continue;

			pDuiTimer->KillDuiTimer();
			delete pDuiTimer;
			m_aDuiTimers.Remove(i);
		}
	}

	bool CTimerSource::RunTimers()
	{
		for( int i = 0; i < m_aDuiTimers.GetSize(); i++ ) {
			CDuiTimerBase* pDuiTimer = static_cast<CDuiTimerBase*>(m_aDuiTimers[i]);
			if( !pDuiTimer)
				continue;

			if(pDuiTimer->GetTimerID() <= 0)
				return false;
		}
		return true;
	}

	CDuiTimer::CDuiTimer()
	{

	}

	CDuiTimer::CDuiTimer( const CDuiTimer& rhs ) :CDuiTimerBase(rhs)
	{

	}

	CDuiTimer::CDuiTimer( HWND hWnd,LPARAM lParam,WPARAM wParam,int iInterval,int iTotalTimer /*= NULL*/,bool bAutoRun /*= true*/,bool bLoop /*= false*/,bool bRevers /*= false*/ ) :
		CDuiTimerBase(hWnd,lParam,wParam,iInterval,iTotalTimer,bAutoRun,bLoop,bRevers)
	{

	}

	CDuiTimer::~CDuiTimer()
	{

	}

	bool CDuiTimer::SetDuiTimer( HWND hWnd,LPARAM lParam,WPARAM wParam,int iInterval,int iTotalTimer /*= NULL*/,bool bAutoRun /*= true*/,bool bLoop /*= false*/,bool bRevers /*= false*/ )
	{
		if(GetTimerID())
			KillDuiTimer();

		SetTimerParam(hWnd,lParam,wParam,iInterval,iTotalTimer,bAutoRun,bLoop,bRevers);

		return InnerSetTimer();
	}

	bool CDuiTimer::SetDuiTimer()
	{
		if((GetObj() && GetFun()) || GetHwnd())
			return InnerSetTimer();

		return false;
	}

	void CDuiTimer::KillDuiTimer()
	{
		InnerKillTimer();
	}

	CDuiTimer* CDuiTimer::Copy() const
	{
		return new CDuiTimer(*this);
	}
}