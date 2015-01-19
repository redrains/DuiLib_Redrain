#ifndef UITimer_h__
#define UITimer_h__

#pragma once

namespace UiLib{

	union unUserData{
		RECT rcRect;
		SIZE szSize;
		UINT uUint;
		DWORD dwDword;
		UINT_PTR upUintPtr;
		int iInt;
		float fFloat;
		double dDouble;
		bool bBool;
		char* pchar;
		const char* pcpchar;
		wchar_t* pwchar;
		const wchar_t* pcwchar;
		CDuiString* pDuiString;
	};

	class CDuiTimerBase;

	class UILIB_API IDuiTimer
	{
	public:
		virtual bool SetDuiTimer(HWND hWnd,LPARAM lParam,WPARAM wParam,int iInterval,int iTotalTimer = NULL,bool bAutoRun = true,bool bLoop = false,bool bRevers = false) = 0;
		virtual bool SetDuiTimer() = 0;
		virtual void KillDuiTimer() = 0;
		virtual CDuiTimerBase* Copy() const = 0;

	public:

		virtual bool IsLoop() const = 0;
		virtual bool IsRevers() const = 0;
		virtual bool IsInverted() const = 0;
		virtual void SetLoop(bool bLoop) = 0;
		virtual void SetRevers(bool bReverse) = 0;
		virtual void SetInverted(bool bInverted) = 0;
		virtual void SetCurTimer(LONG iCurTimer) = 0;

		virtual LONG  GetInterval() const = 0;
		virtual LONG  GetTotalTimer() const = 0;
		virtual LONG  GetCurTimer() const = 0;
		virtual bool  GetAutoStart() const = 0;
		virtual bool  FinshTimered() = 0;
		virtual HWND  GetHwnd() const = 0;
		virtual LPARAM GetLParam() const = 0;
		virtual WPARAM GetWParam() const = 0;

	public:
		virtual void OnTimer(IDuiTimer* pTimer) = 0;
		virtual void OnTimer(IDuiTimer* pTimer,void* param) = 0;
		virtual void OnTimer(IDuiTimer* pTimer,HWND hWnd,LPARAM lParam,WPARAM wParam) = 0;

	public:
		void SetUserData(unUserData nUserData){m_UserData = nUserData;};
		unUserData GetUserData(){return m_UserData;};

	private:
		unUserData m_UserData;
	};

	class UILIB_API CDuiTimerBase : public IDuiTimer
	{
	public:
		virtual ~CDuiTimerBase(void);

	public:
		CDuiTimerBase(void);
		CDuiTimerBase(void* pObj,void* pFun,int iInterval,int iTotalTimer = NULL,bool bAutoRun = true,bool bLoop = false,bool bRevers = false);
		CDuiTimerBase(void* pObj,void* pFun,void* pParam,int iInterval,int iTotalTimer = NULL,bool bAutoRun = true,bool bLoop = false,bool bRevers = false);
		CDuiTimerBase(void* pObj,void* pFun,HWND hWnd,LPARAM lParam,WPARAM wParam,int iInterval,int iTotalTimer = NULL,bool bAutoRun = false,bool bLoop = true,bool bRevers = false);
		CDuiTimerBase(HWND hWnd,LPARAM lParam,WPARAM wParam,int iInterval,int iTotalTimer = NULL,bool bAutoRun = true,bool bLoop = false,bool bRevers = false);
		CDuiTimerBase(const CDuiTimerBase& rhs);

		bool Equals(const CDuiTimerBase& rhs) const;
		void operator() (IDuiTimer* pTimer){return OnTimer(pTimer);};
		void operator() (IDuiTimer* pTimer,void* param){return OnTimer(pTimer,param);};
		void operator() (IDuiTimer* pTimer,HWND hWnd,LPARAM lParam,WPARAM wParam){return OnTimer(pTimer,hWnd,lParam,wParam);};
		
	public:
		void SetTimerParam(void* pObj,void* pFun,void* pParam,int iInterval,int iTotalTimer = NULL,bool bAutoRun = true,bool bLoop = false,bool bRevers = false);
		void SetTimerParam(void* pObj,void* pFun,HWND hWnd,LPARAM lParam,WPARAM wParam,int iInterval,int iTotalTimer = NULL,bool bAutoRun = false,bool bLoop = false,bool bRevers = false);
		void SetTimerParam(HWND hWnd,LPARAM lParam,WPARAM wParam,int iInterval,int iTotalTimer = NULL,bool bAutoRun = true,bool bLoop = false,bool bRevers = false);

		UINT  CalInterval();
		bool  FinshTimered();

		void Resert(){m_iCurTimer = 0;};
		bool IsLoop() const {return m_bLoop;};
		bool IsRevers() const {return m_bReverse;};
		bool IsInverted() const {return m_bInverted;};
		void SetLoop(bool bLoop) {m_bLoop = bLoop;}
		void SetRevers(bool bReverse) {m_bReverse = bReverse;}
		void SetInverted(bool bInverted) {m_bInverted = bInverted;}
		void SetCurTimer(LONG iCurTimer) {m_iCurTimer = iCurTimer;}
		void* GetObj() const {return m_pObject;};
		void* GetFun() const {return m_pFun;};
		void* GetParam() const {return m_pParam;};
		LONG  GetInterval() const {return m_iInterval;};
		LONG  GetTotalTimer() const {return m_iTotalTimer;};
		LONG  GetCurTimer() const {return m_iCurTimer;};
		UINT  GetTimerID() const {return m_uTimerID;};
		UINT  GetTimerAccuracy() const {return m_uTimerAccuracy;};
		HWND  GetHwnd() const {return m_hWnd;};
		LPARAM GetLParam() const {return m_lParam;};
		WPARAM GetWParam() const {return m_wParam;};
		bool  GetAutoStart() const {return m_bAutoStart;};

		void*	m_pFun;
	protected:
		bool InnerSetTimer();
		void InnerKillTimer();
	private:
		void*	m_pObject;
		void*	m_pParam;
		bool	m_bAutoStart;
		bool	m_bLoop;
		bool	m_bReverse;
		bool	m_bInverted;
		LONG	m_iInterval;
		LONG	m_iTotalTimer;
		LONG	m_iCurTimer;
		UINT	m_uTimerID;
		UINT	m_uTimerAccuracy;
		HWND	m_hWnd;
		LPARAM	m_lParam;
		WPARAM	m_wParam;
	};

	class UILIB_API CDuiTimer : public CDuiTimerBase
	{
	public:
		CDuiTimer();
		CDuiTimer(HWND hWnd,LPARAM lParam,WPARAM wParam,int iInterval,int iTotalTimer = NULL,bool bAutoRun = true,bool bLoop = false,bool bRevers = false);
		CDuiTimer(const CDuiTimer& rhs);
		~CDuiTimer();

		virtual bool SetDuiTimer(HWND hWnd,LPARAM lParam,WPARAM wParam,int iInterval,int iTotalTimer = NULL,bool bAutoRun = true,bool bLoop = false,bool bRevers = false);
		virtual bool SetDuiTimer();
		virtual void KillDuiTimer();
		virtual CDuiTimer* Copy() const;
		
		virtual void OnTimer(IDuiTimer* pTimer) {};
		virtual void OnTimer(IDuiTimer* pTimer,void* param) {};
		virtual void OnTimer(IDuiTimer* pTimer,HWND hWnd,LPARAM lParam,WPARAM wParam){::PostMessage(hWnd,WM_TIMER,lParam,wParam);};
	};

	template<class TObj,class TFun,class TParam = void*>
	class UILIB_API TDuiTimer : public CDuiTimerBase
	{
		typedef void (TFun::* TimerOnFnA)(IDuiTimer* pTimer);
		typedef void (TFun::* TimerOnFnB)(IDuiTimer* pTimer,TParam param);
		typedef void (TFun::* TimerOnFnC)(IDuiTimer* pTimer,HWND hWnd,TParam lParam,WPARAM wParam);
	public:
		TDuiTimer(TObj* pObj,TimerOnFnA pFun,int iInterval,int iTotalTimer = NULL,bool bAutoRun = true,bool bLoop = false,bool bRevers = false) : CDuiTimerBase(pObj,*(void**)&pFun,iInterval,iTotalTimer,bAutoRun,bLoop,bRevers)
		{
			if(GetTimerID())
				KillDuiTimer();
			
			m_pTimerOnFnA = pFun;
			m_pTimerOnFnB = NULL;
			m_pTimerOnFnC = NULL;
		};
		TDuiTimer(TObj* pObj,TimerOnFnB pFun,TParam pParam,int iInterval,int iTotalTimer = NULL,bool bAutoRun = true,bool bLoop = false,bool bRevers = false) : CDuiTimerBase(pObj,*(void**)&pFun,pParam,iInterval,iTotalTimer,bAutoRun,bLoop,bRevers)
		{
			if(GetTimerID())
				KillDuiTimer();

			m_pTimerOnFnA = NULL;
			m_pTimerOnFnB = pFun;
			m_pTimerOnFnC = NULL;
		};
		TDuiTimer(TObj* pObj,TimerOnFnC pFun,HWND hWnd,TParam lParam,WPARAM wParam,int iInterval,int iTotalTimer = NULL,bool bAutoRun = true,bool bLoop = false,bool bRevers = false) : CDuiTimerBase(pObj,*(void**)&pFun,hWnd,(LPARAM)lParam,wParam,iInterval,iTotalTimer,bAutoRun,bLoop,bRevers)
		{
			if(GetTimerID())
				KillDuiTimer();

			m_pTimerOnFnA = NULL;
			m_pTimerOnFnB = NULL;
			m_pTimerOnFnC = pFun;
		};
		TDuiTimer(HWND hWnd,LPARAM lParam,WPARAM wParam,int iInterval,int iTotalTimer = NULL,bool bAutoRun = true,bool bLoop = false,bool bRevers = false) : CDuiTimerBase(hWnd,lParam,wParam,iInterval,iTotalTimer,bAutoRun,bLoop,bRevers)
		{
			if(GetTimerID())
				KillDuiTimer();
		};
		TDuiTimer(const TDuiTimer& rhs) : CDuiTimerBase(rhs)
		{
			m_pTimerOnFnA = rhs.m_pTimerOnFnA;
			m_pTimerOnFnB = rhs.m_pTimerOnFnB;
			m_pTimerOnFnC = rhs.m_pTimerOnFnC;
		}

		~TDuiTimer(){
			KillDuiTimer();
		}

		virtual bool SetDuiTimer(HWND hWnd,LPARAM lParam,WPARAM wParam,int iInterval,int iTotalTimer = NULL,bool bAutoRun = true,bool bLoop = false,bool bRevers = false)
		{
			if(GetTimerID())
				KillDuiTimer();

			SetTimerParam(hWnd,lParam,wParam,iInterval,iTotalTimer,bAutoRun,bLoop,bRevers);

			return InnerSetTimer();
		}
		virtual bool SetDuiTimer(){ return InnerSetTimer();};
		virtual void KillDuiTimer(){ return InnerKillTimer();};
		virtual TDuiTimer* Copy() const { return new TDuiTimer(*this); }

		void OnTimer(IDuiTimer* pTimer)
		{
			TObj* pObject = (TObj*) GetObj();
			if(pObject && m_pTimerOnFnA)
				(pObject->*m_pTimerOnFnA)(pTimer);
		};

		void OnTimer(IDuiTimer* pTimer,void* param)
		{
			TObj* pObject = (TObj*) GetObj();
			if(pObject && m_pTimerOnFnB)
				(pObject->*m_pTimerOnFnB)(pTimer,(TParam)param);
		};

		void OnTimer(IDuiTimer* pTimer,HWND hWnd,LPARAM lParam,WPARAM wParam)
		{
			TObj* pObject = (TObj*) GetObj();
			if(pObject && m_pTimerOnFnC)
				(pObject->*m_pTimerOnFnC)(pTimer,hWnd,(TParam)lParam,wParam);
			else if(hWnd)
				::PostMessage(hWnd,WM_TIMER,lParam,wParam);
		};

	private:
		TimerOnFnA		m_pTimerOnFnA;
		TimerOnFnB		m_pTimerOnFnB;
		TimerOnFnC		m_pTimerOnFnC;
	};

	template <class TObj,class TFun>
	IDuiTimer* MakeDuiTimer(TObj* pObject, void (TFun::* pFn)(IDuiTimer*),int iInterval,int iTotalTimer = NULL,bool bAutoRun = true,bool bLoop = false,bool bRevers = false)
	{
		return (IDuiTimer*)new TDuiTimer<TObj, TFun,void*>(pObject,pFn,iInterval,iTotalTimer,bAutoRun,bLoop,bRevers);
	}

	template <class TObj,class TFun,class TParam>
	IDuiTimer* MakeDuiTimer(TObj* pObject, void (TFun::* pFn)(IDuiTimer*,TParam),TParam pParam,int iInterval,int iTotalTimer = NULL,bool bAutoRun = true,bool bLoop = false,bool bRevers = false)
	{
		return (IDuiTimer*)new TDuiTimer<TObj, TFun,TParam>(pObject,pFn,pParam,iInterval,iTotalTimer,bAutoRun,bLoop,bRevers);
	}

	template <class TObj,class TFun,class TParam>
	IDuiTimer* MakeDuiTimer(TObj* pObject, void (TFun::* pFn)(IDuiTimer* pTimer,HWND hWnd,TParam lParam,WPARAM wParam),HWND hWnd,TParam lParam,WPARAM wParam,int iInterval,int iTotalTimer = NULL,bool bAutoRun = true)
	{
		return (IDuiTimer*)new TDuiTimer<TObj, TFun,TParam>(pObject,pFn,hWnd,lParam,wParam,iInterval,iTotalTimer,bAutoRun,bLoop,bRevers);
	}
	
	typedef void (*TimerOnFnA)(IDuiTimer* pTimer);
	typedef void (*TimerOnFnB)(IDuiTimer* pTimer,void* param);
	typedef void (*TimerOnFnC)(IDuiTimer* pTimer,HWND hWnd,LPARAM lParam,WPARAM wParam);

	class UILIB_API CTimerSource
	{
	public:
		~CTimerSource();
		operator bool();
		void operator+= (const CDuiTimerBase& _DuiTimer);
		void operator+= (IDuiTimer* _DuiTimer){(*this) += (CDuiTimerBase*)_DuiTimer;};
		void operator+= (CDuiTimerBase* _DuiTimer);
		void operator-= (const CDuiTimerBase& _DuiTimer);
		void operator-= (IDuiTimer* _DuiTimer){(*this) -= (CDuiTimerBase*)_DuiTimer;};
		void operator-= (const CDuiTimerBase* _DuiTimer);
	public:
		void KillTimers();
		void SetTimers();
		void ResetTimers();
		void RemoveTimerAll();
		bool RunTimers();
	protected:
		CStdPtrArray m_aDuiTimers;
	};
}
#endif // UITimer_h__
