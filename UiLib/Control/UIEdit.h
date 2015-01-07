#ifndef __UIEDIT_H__
#define __UIEDIT_H__

#pragma once

namespace UiLib
{
	class CEditWnd;

	class UILIB_API CEditUI : public CLabelUI
	{
		friend class CEditWnd;
	public:
		CEditUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void SetEnabled(bool bEnable = true);
		void SetText(LPCTSTR pstrText);
		void SetMaxChar(int uMax);
		int GetMaxChar();
		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;
		void SetPasswordMode(bool bPasswordMode);
		bool IsPasswordMode() const;
		void SetPasswordChar(TCHAR cPasswordChar);
		TCHAR GetPasswordChar() const;
		void SetNumberOnly(bool bNumberOnly);
		bool IsNumberOnly() const;
		int GetWindowStyls() const;

		LPCTSTR GetNormalImage();
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage();
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage();
		void SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetDisabledImage();
		void SetDisabledImage(LPCTSTR pStrImage);
		void SetNativeEditBkColor(DWORD dwBkColor);
		DWORD GetNativeEditBkColor() const;
		void SetNativeEditTextColor( LPCTSTR pStrColor );
		DWORD GetNativeEditTextColor() const;

		void SetSel(long nStartChar, long nEndChar);
		void SetSelAll();
		void SetReplaceSel(LPCTSTR lpszReplace);

		bool MatchRegular(bool isShowMsg = true);
		void SetRegularCheck(LPCTSTR pRegularCheckStr);
		LPCTSTR GetRegularCheck();
		void SetRegularTip(LPCTSTR pRegularTipStr);
		LPCTSTR GetRegularTip();
		void SetMatchCase(bool bMatchCase = false);
		bool GetMatchCase();

		void SetTipValue(LPCTSTR pStrTipValue);
		LPCTSTR GetTipValue();
		void SetTipValueColor(LPCTSTR pStrColor);
		DWORD GetTipValueColor();

		void SetPos(RECT rc);
		void SetVisible(bool bVisible = true);
		void SetInternVisible(bool bVisible = true);
		SIZE EstimateSize(SIZE szAvailable);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintBkColor(HDC hDC);
		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);

		void SetEnableTimer(bool bEnableTime);
		bool GetEnableTimer();
		void SetTimerDelay(UINT nDelay);
		UINT GetTimerDelay();

	public:
		void OnTimer(UINT iTimerID);
	protected:
		UINT m_uDelay;
		CEditWnd*	m_pWindow;
		bool		m_bEnableTime;
		CDuiString	m_sCheckVal;
		
		int m_uMaxChar;
		bool m_bReadOnly;
		bool m_bPasswordMode;
		bool m_bMatchCase;
		TCHAR m_cPasswordChar;
		UINT m_uButtonState;
		CDuiString m_RegularCheckStr;
		CDuiString m_RegularTipStr;
		CDuiString m_RegluarSrcText;
		CDuiString m_sNormalImage;
		CDuiString m_sHotImage;
		CDuiString m_sFocusedImage;
		CDuiString m_sDisabledImage;
		CDuiString m_sTipValue;
		DWORD m_dwTipValueColor;
		DWORD m_dwEditbkColor;
		DWORD m_dwEditTextColor;
		int m_iWindowStyls;
	};
}
#endif // __UIEDIT_H__