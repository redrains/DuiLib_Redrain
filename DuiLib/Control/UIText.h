#ifndef __UITEXT_H__
#define __UITEXT_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CTextUI : public CLabelUI
	{
	public:
		CTextUI();
		~CTextUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void SetText(LPCTSTR pstrText) override;

		CDuiString* GetLinkContent(int iIndex);

		void DoEvent(TEventUI& event);
		SIZE EstimateSize(SIZE szAvailable);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetShadowColor(DWORD dwTextColor);
		DWORD GetShadowColor() const;
		void PaintText(HDC hDC);

	protected:
		enum { MAX_LINK = 8 };
		int		m_nLinks;
		RECT	m_rcLinks[MAX_LINK];
		CDuiString m_sLinks[MAX_LINK];
		int		m_nHoverLink;

		DWORD	m_dwShadowColor;
	};

} // namespace DuiLib

#endif //__UITEXT_H__