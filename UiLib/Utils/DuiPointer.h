#ifndef DuiPointer_h__
#define DuiPointer_h__

namespace UiLib
{
	template<typename T>
	class DuiPtr
	{
		template<typename X>
		friend class DuiPtr;
	protected:
		int*	ptrCounter;
		T*		duiReference;

		void Push()
		{
			if(ptrCounter)
				(*ptrCounter)++;
		}

		void Pop()
		{
			if(ptrCounter)
			{
				if(--(*ptrCounter)==0)
				{
					delete ptrCounter;
					delete duiReference;
					ptrCounter	=0;
					duiReference=0;
				}
			}
		}

		int* PtrCounter()const{return ptrCounter;}
		DuiPtr(int* _ptrCounter, T* _duiReference) :ptrCounter(_ptrCounter),duiReference(_duiReference){Push();}
	public:

		DuiPtr()
		{
			ptrCounter	=0;
			duiReference=0;
		}

		DuiPtr(T* _duiPointer)
		{
			if(_duiPointer)
			{
				ptrCounter	=new int(1);
				duiReference=_duiPointer;
			}
			else
			{
				ptrCounter	=0;
				duiReference=0;
			}
		}

		DuiPtr(const DuiPtr<T>& _duiPointer)
		{
			ptrCounter	=_duiPointer.ptrCounter;
			duiReference=_duiPointer.duiReference;
			Push();
		}

		template<typename C>
		DuiPtr(const DuiPtr<C>& _duiPointer)
		{
			T* duiConverted=_duiPointer.Obj();
			if(duiConverted)
			{
				ptrCounter	=_duiPointer.PtrCounter();
				duiReference=duiConverted;
				Push();
			}
			else
			{
				ptrCounter	=0;
				duiReference=0;
			}
		}

		~DuiPtr()
		{
			Pop();
		}

		template<typename C>
		DuiPtr<C> Cast()const
		{
			C* duiConverted=dynamic_cast<C*>(duiReference);
			return DuiPtr<C>((duiConverted?ptrCounter:0), duiConverted);
		}

		DuiPtr<T>& operator=(T* _duiPointer)
		{
			Pop();
			if(_duiPointer)
			{
				ptrCounter	=new int(1);
				duiReference=_duiPointer;
			}
			else
			{
				ptrCounter	=0;
				duiReference=0;
			}
			return *this;
		}

		DuiPtr<T>& operator=(const DuiPtr<T>& _duiPointer)
		{
			if(this!=&_duiPointer)
			{
				Pop();
				ptrCounter	=_duiPointer.ptrCounter;
				duiReference=_duiPointer.duiReference;
				Push();
			}
			return *this;
		}

		template<typename C>
		DuiPtr<T>& operator=(const DuiPtr<C>& _duiPointer)
		{
			T* duiConverted=_duiPointer.Obj();
			Pop();
			if(duiConverted)
			{
				ptrCounter	=_duiPointer.PtrCounter();
				duiReference=duiConverted;
				Push();
			}
			else
			{
				ptrCounter	=0;
				duiReference=0;
			}
			return *this;
		}

		bool operator==(const T* _duiPointer)const{return duiReference==_duiPointer;}
		bool operator!=(const T* _duiPointer)const{return duiReference!=_duiPointer;}
		bool operator>(const T* _duiPointer)const{return duiReference>_duiPointer;}
		bool operator>=(const T* _duiPointer)const{return duiReference>=_duiPointer;}
		bool operator<(const T* _duiPointer)const{return duiReference<_duiPointer;}
		bool operator<=(const T* _duiPointer)const{return duiReference<=_duiPointer;}
		bool operator==(const DuiPtr<T>& _duiPointer)const{return duiReference==_duiPointer.duiReference;}
		bool operator!=(const DuiPtr<T>& _duiPointer)const{return duiReference!=_duiPointer.duiReference;}
		bool operator>(const DuiPtr<T>& _duiPointer)const{return duiReference>_duiPointer.duiReference;}
		bool operator>=(const DuiPtr<T>& _duiPointer)const{return duiReference>=_duiPointer.duiReference;}
		bool operator<(const DuiPtr<T>& _duiPointer)const{return duiReference<_duiPointer.duiReference;}
		bool operator<=(const DuiPtr<T>& _duiPointer)const{return duiReference<=_duiPointer.duiReference;}
		operator bool()const{return duiReference!=0;}
		T* Obj()const{return duiReference;}
		T* operator->()const{return duiReference;}
	};

	template<typename T>
	class DuiComPtr
	{
	protected:
		int*	ptrCounter;
		T*		duiReference;

		void Push()
		{
			if(ptrCounter)
				(*ptrCounter)++;
		}

		void Pop()
		{
			if(ptrCounter)
			{
				if(--(*ptrCounter)==0)
				{
					delete ptrCounter;
					duiReference->Release();
					ptrCounter=0;
					duiReference=0;
				}
			}
		}

		int* PtrCounter()const{return ptrCounter;}
		DuiComPtr(int* _ptrCounter, T* _duiReference) :ptrCounter(_ptrCounter),duiReference(_duiReference){Push();}
	public:

		DuiComPtr()
		{
			ptrCounter	=0;
			duiReference=0;
		}

		DuiComPtr(T* _duiPointer)
		{
			if(_duiPointer)
			{
				ptrCounter	=new int(1);
				duiReference=_duiPointer;
			}
			else
			{
				ptrCounter	=0;
				duiReference=0;
			}
		}

		DuiComPtr(const DuiComPtr<T>& _duiPointer)
		{
			ptrCounter	=_duiPointer.ptrCounter;
			duiReference=_duiPointer.duiReference;
			Push();
		}

		~DuiComPtr(){Pop();}

		DuiComPtr<T>& operator=(T* _duiPointer)
		{
			Pop();
			if(_duiPointer)
			{
				ptrCounter	=new int(1);
				duiReference=_duiPointer;
			}
			else
			{
				ptrCounter	=0;
				duiReference=0;
			}
			return *this;
		}

		DuiComPtr<T>& operator=(const DuiComPtr<T>& _duiPointer)
		{
			if(this!=&_duiPointer)
			{
				Pop();
				ptrCounter	=_duiPointer.ptrCounter;
				duiReference=_duiPointer.duiReference;
				Push();
			}
			return *this;
		}

		bool operator==(const T* _duiPointer)const{return duiReference==_duiPointer;}
		bool operator!=(const T* _duiPointer)const{return duiReference!=_duiPointer;}
		bool operator>(const T* _duiPointer)const{return duiReference>_duiPointer;}
		bool operator>=(const T* _duiPointer)const{return duiReference>=_duiPointer;}
		bool operator<(const T* _duiPointer)const{return duiReference<_duiPointer;}
		bool operator<=(const T* _duiPointer)const{return duiReference<=_duiPointer;}
		bool operator==(const DuiComPtr<T>& _duiPointer)const{return duiReference==_duiPointer.duiReference;}
		bool operator!=(const DuiComPtr<T>& _duiPointer)const{return duiReference!=_duiPointer.duiReference;}
		bool operator>(const DuiComPtr<T>& _duiPointer)const{return duiReference>_duiPointer.duiReference;}
		bool operator>=(const DuiComPtr<T>& _duiPointer)const{return duiReference>=_duiPointer.duiReference;}

		bool operator<(const DuiComPtr<T>& _duiPointer)const{return duiReference<_duiPointer.duiReference;}
		bool operator<=(const DuiComPtr<T>& _duiPointer)const{return duiReference<=_duiPointer.duiReference;}
		operator bool()const{return duiReference!=0;}

		T* Obj()const{return duiReference;}
		T* operator->()const{return duiReference;}
	};

	template<typename T>struct DuiType{static const bool Result=false;};
	template<>struct DuiType<bool>{static const bool Result=true;};
	template<>struct DuiType<unsigned __int8>{static const bool Result=true;};
	template<>struct DuiType<signed __int8>{static const bool Result=true;};
	template<>struct DuiType<unsigned __int16>{static const bool Result=true;};
	template<>struct DuiType<signed __int16>{static const bool Result=true;};
	template<>struct DuiType<unsigned __int32>{static const bool Result=true;};
	template<>struct DuiType<signed __int32>{static const bool Result=true;};
	template<>struct DuiType<unsigned __int64>{static const bool Result=true;};
	template<>struct DuiType<signed __int64>{static const bool Result=true;};
	template<>struct DuiType<char>{static const bool Result=true;};
	template<>struct DuiType<wchar_t>{static const bool Result=true;};
	template<typename T>struct DuiType<T*>{static const bool Result=true;};
	template<typename T>struct DuiType<T&>{static const bool Result=true;};
	template<typename T, typename C>struct DuiType<T C::*>{static const bool Result=true;};
	template<typename T>struct DuiType<const T>{static const bool Result=DuiType<T>::Result;};
	template<typename T>struct DuiType<volatile T>{static const bool Result=DuiType<T>::Result;};
	template<typename T>struct DuiType<const volatile T>{static const bool Result=DuiType<T>::Result;};
	template<typename T>struct DuiType<DuiComPtr<T>>{static const bool Result=false;};
	template<typename T>struct DuiType<DuiPtr<T>>{static const bool Result=false;};
}
#endif // Dui_duiPointer_h__
