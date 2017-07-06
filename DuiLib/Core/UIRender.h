#ifndef __UIRENDER_H__
#define __UIRENDER_H__

#pragma once

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CRenderClip
{
public:
    ~CRenderClip();
    RECT rcItem;
    HDC hDC;
    HRGN hRgn;
    HRGN hOldRgn;

    static void GenerateClip(HDC hDC, RECT rc, CRenderClip& clip);
	//width：一个角的宽度，height：一个角的高度
    static void GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CRenderClip& clip);
    static void UseOldClipBegin(HDC hDC, CRenderClip& clip);
    static void UseOldClipEnd(HDC hDC, CRenderClip& clip);
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CRenderEngine
{
public:
    static DWORD AdjustColor(DWORD dwColor, short H, short S, short L);
	static HBITMAP CreateBitmap(HDC hDC, int cx, int cy, BYTE** pBits);

	static void DrawImage(HDC hDC, HBITMAP hBitmap, const RECT& rcDest, const RECT& rcPaint,
        const RECT& rcSource, const RECT& rcCorner, bool isAlpha, BYTE uAlpha = 255, 
        bool hole = false, bool xtiled = false, bool ytiled = false);
	static bool DrawImage(HDC hDC, CPaintManagerUI* pManager, const RECT& rcControl, const RECT& rcPaint, CImageAttribute& image);
	static void DrawText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, \
		DWORD dwTextColor, int iFont, UINT uStyle);
	static void DrawColor(HDC hDC, const RECT& rc, DWORD color);
    static void DrawGradient(HDC hDC, const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps);

	// 以下函数默认使用Gdi+渲染，支持alpha通道
	// 使用USE_GDI_RENDER宏后使用Gdi渲染，不支持alpha通道
	static void DrawLine(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor, int nStyle = PS_SOLID);
	static void DrawRect(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor);
	static void DrawRoundRect(HDC hDC, const RECT& rc, int width, int height, int nSize, DWORD dwPenColor);

	//
	static void DrawHtmlText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, 
        DWORD dwTextColor, RECT* pLinks, CDuiString* sLinks, int& nLinkRects, UINT uStyle);
    static HBITMAP GenerateBitmap(CPaintManagerUI* pManager, CControlUI* pControl, RECT rc);
	static SIZE GetTextSize(HDC hDC, CPaintManagerUI* pManager , LPCTSTR pstrText, int iFont, UINT uStyle);

	static void RestoreAlphaColor(LPBYTE pBits, int bitsWidth, PRECT rc);
};

} // namespace DuiLib

#endif // __UIRENDER_H__
