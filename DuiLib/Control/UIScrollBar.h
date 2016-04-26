#ifndef __UISCROLLBAR_H__
#define __UISCROLLBAR_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CScrollBarUI : public CControlUI
	{
	public:
		CScrollBarUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		CContainerUI* GetOwner() const;
		void SetOwner(CContainerUI* pOwner);

		void SetVisible(bool bVisible = true);
		void SetEnabled(bool bEnable = true);
		void SetFocus();

		bool IsHorizontal();
		void SetHorizontal(bool bHorizontal = true);
		int GetScrollRange() const;
		void SetScrollRange(int nRange);
		int GetScrollPos() const;
		void SetScrollPos(int nPos);
		int GetLineSize() const;
		void SetLineSize(int nSize);

		bool GetShowButton1();
		void SetShowButton1(bool bShow);
		LPCTSTR GetButton1NormalImage();
		void SetButton1NormalImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1HotImage();
		void SetButton1HotImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1PushedImage();
		void SetButton1PushedImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1DisabledImage();
		void SetButton1DisabledImage(LPCTSTR pStrImage);

		bool GetShowButton2();
		void SetShowButton2(bool bShow);
		LPCTSTR GetButton2NormalImage();
		void SetButton2NormalImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2HotImage();
		void SetButton2HotImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2PushedImage();
		void SetButton2PushedImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2DisabledImage();
		void SetButton2DisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetThumbNormalImage();
		void SetThumbNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbHotImage();
		void SetThumbHotImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbPushedImage();
		void SetThumbPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbDisabledImage();
		void SetThumbDisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetRailNormalImage();
		void SetRailNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetRailHotImage();
		void SetRailHotImage(LPCTSTR pStrImage);
		LPCTSTR GetRailPushedImage();
		void SetRailPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetRailDisabledImage();
		void SetRailDisabledImage(LPCTSTR pStrImage);

		void SetPos(RECT rc);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void DoPaint(HDC hDC, const RECT& rcPaint);

		void PaintBk(HDC hDC);
		void PaintButton1(HDC hDC);
		void PaintButton2(HDC hDC);
		void PaintThumb(HDC hDC);
		void PaintRail(HDC hDC);

	protected:

		enum
		{ 
			DEFAULT_SCROLLBAR_SIZE = 16,
			DEFAULT_TIMERID = 10,
		};

		bool m_bHorizontal;
		int m_nRange;
		int m_nScrollPos;
		int m_nLineSize;
		CContainerUI* m_pOwner;
		POINT ptLastMouse;
		int m_nLastScrollPos;
		int m_nLastScrollOffset;
		int m_nScrollRepeatDelay;

		bool m_bShowButton1;
		RECT m_rcButton1;
		UINT m_uButton1State;
		CImageAttribute m_button1NormalImage;
		CImageAttribute m_button1HotImage;
		CImageAttribute m_button1PushedImage;
		CImageAttribute m_button1DisabledImage;

		bool m_bShowButton2;
		RECT m_rcButton2;
		UINT m_uButton2State;
		CImageAttribute m_button2NormalImage;
		CImageAttribute m_button2HotImage;
		CImageAttribute m_button2PushedImage;
		CImageAttribute m_button2DisabledImage;

		RECT m_rcThumb;
		UINT m_uThumbState;
		CImageAttribute m_thumbNormalImage;
		CImageAttribute m_thumbHotImage;
		CImageAttribute m_thumbPushedImage;
		CImageAttribute m_thumbDisabledImage;

		CImageAttribute m_railNormalImage;
		CImageAttribute m_railHotImage;
		CImageAttribute m_railPushedImage;
		CImageAttribute m_railDisabledImage;
	};
}

#endif // __UISCROLLBAR_H__