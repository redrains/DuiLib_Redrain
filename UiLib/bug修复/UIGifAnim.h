#ifndef GifAnimUI_h__
#define GifAnimUI_h__

#include <gdiplus.h>
using namespace Gdiplus;
#pragma once

namespace DuiLib
{
	class CLabelUI;

#define EVENT_TIEM_ID	100

	class CGifAnimUI : public CLabelUI
	{
	public:
		CGifAnimUI(void);
		~CGifAnimUI(void);
	public:
		virtual LPCTSTR	GetClass() const;
		virtual LPVOID	GetInterface(LPCTSTR pstrName);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void SetVisible(bool bVisible = true);
		virtual void SetInternVisible(bool bVisible = true);
		virtual void DoPaint(HDC hDC, const RECT& rcPaint);
		virtual void DoEvent(TEventUI& event);
	public:
		void StartAnim();
		void StopAnim();
	protected:
		struct Imp;
		Imp* m_pImp;
	};
}

#endif // GifAnimUI_h__
