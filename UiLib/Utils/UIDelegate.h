#ifndef __UIDELEGATE_H__
#define __UIDELEGATE_H__

#pragma once

namespace UiLib {
	typedef struct tagTEventUI TEventUI;
	typedef struct tagTNotifyUI TNotifyUI;

	typedef bool (*FunVoid)(void* pParam,LPARAM lParam,WPARAM wParam);
	typedef bool (*FunTEvent)(TEventUI* pTEventUI,LPARAM lParam,WPARAM wParam);
	typedef bool (*FunTNotify)(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);

	class CDelegateBase;
	
	class UILIB_API IDelegate
	{
	public:
		typedef union {
			FunVoid pFunVoid;
			FunTEvent pFunTEvent;
			FunTNotify pFunTNotify;
		}FnType;

	public:
		virtual FunVoid GetFunVoid() const = 0;
		virtual FunTEvent GetFunTEvent() const = 0;
		virtual FunTNotify GetFunTNotify() const = 0;
		virtual LPARAM GetLParam() const = 0;
		virtual WPARAM GetWParam() const = 0;
	public:
		virtual CDelegateBase* Copy() const = 0;
		virtual bool Invoke(void* pParam,LPARAM lParam = NULL,WPARAM wParam = NULL) = 0;
		virtual bool Invoke(TEventUI* pTEventUI,LPARAM lParam = NULL,WPARAM wParam = NULL) = 0;
		virtual bool Invoke(TNotifyUI* pTNotifyUI,LPARAM lParam = NULL,WPARAM wParam = NULL) = 0;
	};

	class UILIB_API CDelegateBase : public IDelegate
	{
	public:
		CDelegateBase(void* pObject, FunVoid pFn,LPARAM lParam = NULL,WPARAM wParam = NULL);
		CDelegateBase(void* pObject, FunTEvent pFn,UINT _iEventType,LPARAM lParam = NULL,WPARAM wParam = NULL);
		CDelegateBase(void* pObject, FunTNotify pFn,LPCTSTR _sNotifyTypeName,LPARAM lParam = NULL,WPARAM wParam = NULL);
		CDelegateBase(const CDelegateBase& rhs);

		virtual ~CDelegateBase();
		FunVoid GetFunVoid() const{return m_unionFnType.pFunVoid;};
		FunTEvent GetFunTEvent() const{return m_unionFnType.pFunTEvent;};
		FunTNotify GetFunTNotify() const{return m_unionFnType.pFunTNotify;};
		LPARAM GetLParam() const { return m_lParam;};
		WPARAM GetWParam() const { return m_wParam;};
		bool Equals(const CDelegateBase& rhs) const;
		bool operator() (void* param,LPARAM lParam = NULL,WPARAM wParam = NULL){return Invoke(param,lParam,wParam);};
		bool operator() (TEventUI* pTEventUI,UINT _iEventType,LPARAM lParam = NULL,WPARAM wParam = NULL){return Invoke(pTEventUI,lParam,wParam);};
		bool operator() (TNotifyUI* pTNotifyUI,LPCTSTR _sNotifyTypeName,LPARAM lParam = NULL,WPARAM wParam = NULL){return Invoke(pTNotifyUI,lParam,wParam);};

		void* GetObj() const {return m_pObject;};
	protected:
		void SetEventType(UINT _iEventType){m_iEventType = _iEventType;};
		void SetNotifyTypeName(CDuiString& _sNotifyTypeName){m_sNotifyTypeName = _sNotifyTypeName.GetData();};
		UINT GetEventType(){return m_iEventType;};
		CDuiString GetNotifyTypeName(){return m_sNotifyTypeName.GetData();};

	private:
		void* m_pObject;
		FnType m_unionFnType;
		LPARAM m_lParam;
		WPARAM m_wParam;
	protected:
		UINT m_iEventType;
		CDuiString m_sNotifyTypeName;
	};

	class UILIB_API CDelegateStatic: public CDelegateBase
	{
	public:
		CDelegateStatic(FunVoid pFunVoid,LPARAM lParam = NULL,WPARAM wParam = NULL) : CDelegateBase(NULL, pFunVoid,lParam,wParam) { } 
		CDelegateStatic(FunTEvent pFunTEvent,UINT _iEventType = 0,LPARAM lParam = NULL,WPARAM wParam = NULL) : CDelegateBase(NULL, pFunTEvent,_iEventType,lParam,wParam) { } 
		CDelegateStatic(FunTNotify pFunTNotify,LPCTSTR _sNotifyTypeName = NULL,LPARAM lParam = NULL,WPARAM wParam = NULL) : CDelegateBase(NULL, pFunTNotify,_sNotifyTypeName,lParam,wParam) { } 
		CDelegateStatic(const CDelegateStatic& rhs) : CDelegateBase(rhs) { } 
		virtual CDelegateBase* Copy() const { return new CDelegateStatic(*this); }

	protected:
		virtual bool Invoke(void* param,LPARAM lParam = NULL,WPARAM wParam = NULL)
		{
			FunVoid pFunVoid = GetFunVoid();
			return pFunVoid(param,lParam,wParam); 
		}

		virtual bool Invoke(TEventUI* pTEventUI,LPARAM lParam = NULL,WPARAM wParam = NULL)
		{
			FunTEvent pFunTEvent = GetFunTEvent();
			return !pFunTEvent(pTEventUI,lParam,wParam); 
		};

		virtual bool Invoke(TNotifyUI* pTNotifyUI,LPARAM lParam = NULL,WPARAM wParam = NULL)
		{
			FunTNotify pFunTNotify = GetFunTNotify();
			return pFunTNotify(pTNotifyUI,lParam,wParam);
		};
	};

	template <class O, class T,class P = LPARAM>
	class UILIB_API CDelegate : public CDelegateBase
	{
		typedef bool (T::*CMFunVoid)(void* pParam,P lParam,WPARAM wParam);
		typedef bool (T::*CMFunTEvent)(TEventUI* pTEventUI,P lParam,WPARAM wParam);
		typedef bool (T::*CMFunTNotify)(TNotifyUI* pTNotifyUI,P lParam,WPARAM wParam);
	public:
		CDelegate(O* pObj, CMFunVoid pCMFunVoid,P lParam = NULL,WPARAM wParam = NULL) : CDelegateBase(pObj, *(FunVoid*)&pCMFunVoid,(LPARAM)lParam,wParam), m_pCMFunVoid(pCMFunVoid),m_pCMFunTEvent(NULL),m_pCMFunTNotify(NULL){}
		CDelegate(O* pObj, CMFunTEvent pCMFunTEvent,UINT _iEventType = 0,P lParam = NULL,WPARAM wParam = NULL) : CDelegateBase(pObj, *(FunTEvent*)&pCMFunTEvent,_iEventType,(LPARAM)lParam,wParam), m_pCMFunVoid(NULL),m_pCMFunTEvent(pCMFunTEvent),m_pCMFunTNotify(NULL) { }
		CDelegate(O* pObj, CMFunTNotify pCMFunTNotify,LPCTSTR _sNotifyTypeName = NULL,P lParam = NULL,WPARAM wParam = NULL) : CDelegateBase(pObj, *(FunTNotify*)&pCMFunTNotify,_sNotifyTypeName,(LPARAM)lParam,wParam), m_pCMFunVoid(NULL),m_pCMFunTEvent(NULL),m_pCMFunTNotify(pCMFunTNotify) { }
		CDelegate(const CDelegate& rhs) : CDelegateBase(rhs) { m_pCMFunVoid = rhs.m_pCMFunVoid;m_pCMFunTEvent = rhs.m_pCMFunTEvent;m_pCMFunTNotify = rhs.m_pCMFunTNotify;} 
		virtual CDelegateBase* Copy() const { return new CDelegate(*this); }

	protected:
		virtual bool Invoke(void* param,LPARAM lParam = NULL,WPARAM wParam = NULL)
		{
			O* pObject = (O*) GetObj();
			if(pObject && m_pCMFunVoid)
				return (pObject->*m_pCMFunVoid)(param,(P)GetLParam(),GetWParam());
			else if(pObject && m_pCMFunTEvent)
				return Invoke((TEventUI*)param,GetLParam(),GetWParam());
			else if(pObject && m_pCMFunTNotify)
				return Invoke((TNotifyUI*)param,GetLParam(),GetWParam());

			return true;
		} 

		virtual bool Invoke(TEventUI* pTEventUI,LPARAM lParam = NULL,WPARAM wParam = NULL)
		{
			O* pObject = (O*) GetObj();
			if(pObject && pTEventUI && GetEventType() == 0)
				return (pObject->*m_pCMFunTEvent)(pTEventUI,(P)GetLParam(),GetWParam()); 
			else if(pObject && pTEventUI && pTEventUI->Type == GetEventType())
				return (pObject->*m_pCMFunTEvent)(pTEventUI,(P)GetLParam(),GetWParam());

			return true;
		};

		virtual bool Invoke(TNotifyUI* pTNotifyUI,LPARAM lParam = NULL,WPARAM wParam = NULL)
		{
			O* pObject = (O*) GetObj();
			if(pObject && GetNotifyTypeName().IsEmpty())
				return (pObject->*m_pCMFunTNotify)(pTNotifyUI,(P)GetLParam(),GetWParam());
			else if(pObject && pTNotifyUI && pTNotifyUI->sType == GetNotifyTypeName())
				return (pObject->*m_pCMFunTNotify)(pTNotifyUI,(P)GetLParam(),GetWParam());

			return true;
		};

	private:
		CMFunVoid m_pCMFunVoid;
		CMFunTEvent m_pCMFunTEvent;
		CMFunTNotify m_pCMFunTNotify;
	};

	template <class O, class T,class P>
	CDelegate<O, T, P> MakeDelegate(O* pObject, bool (T::* pFn)(void* pParam,LPARAM lParam,WPARAM wParam),P lParam = NULL,WPARAM wParam = NULL)
	{
		return CDelegate<O, T, P>(pObject, pFn,lParam,wParam);
	}

	template <class O, class T,class P>
	CDelegate<O, T, P> MakeDelegate(O* pObject, bool (T::* pFn)(TEventUI*,P lParam,WPARAM wParam),UINT _iEventType,P lParam = NULL,WPARAM wParam = NULL)
	{
		return CDelegate<O, T, P>(pObject, pFn,_iEventType,lParam,wParam);
	}

	template <class O, class T,class P>
	CDelegate<O, T, P> MakeDelegate(O* pObject, bool (T::* pFn)(TNotifyUI*,P lParam,WPARAM wParam),LPCTSTR _sNotifyTypeName,P lParam = NULL,WPARAM wParam = NULL)
	{
		return CDelegate<O, T, P>(pObject, pFn,(LPCTSTR)_sNotifyTypeName,lParam,wParam);
	}

	inline CDelegateStatic MakeDelegate(FunVoid pFunVoid,LPARAM lParam = NULL,WPARAM wParam = NULL)
	{
		return CDelegateStatic(pFunVoid,lParam,wParam);
	}

	inline CDelegateStatic MakeDelegate(FunTEvent pFunTEvent,UINT _iEventType,LPARAM lParam = NULL,WPARAM wParam = NULL)
	{
		return CDelegateStatic(pFunTEvent,_iEventType,lParam,wParam); 
	}

	inline CDelegateStatic MakeDelegate(FunTNotify pFunTNotify,LPCTSTR _sNotifyTypeName,LPARAM lParam = NULL,WPARAM wParam = NULL)
	{
		return CDelegateStatic(pFunTNotify,_sNotifyTypeName,lParam,wParam); 
	}

	class UILIB_API CEventSource
	{
	public:
		CEventSource();
		~CEventSource();
		operator bool();
		void operator+= (const CDelegateBase& d); // add const for gcc
		void operator+= (FunVoid pFunVoid){(*this) += MakeDelegate(pFunVoid);};
		void operator-= (const CDelegateBase& d);
		void operator-= (FunVoid pFunVoid){(*this) -= MakeDelegate(pFunVoid);};
		bool operator() (void* param);
		bool operator() (TEventUI* pTEventUI);
		bool operator() (TNotifyUI* pTNotifyUI);

	protected:
		TStdPtrArray<CDelegateBase*> m_aDelegates;
	};

} // namespace UiLib

#endif // __UIDELEGATE_H__