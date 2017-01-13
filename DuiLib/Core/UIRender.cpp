#include "StdAfx.h"


namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

CRenderClip::~CRenderClip()
{
    ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
    ASSERT(::GetObjectType(hRgn)==OBJ_REGION);
    ASSERT(::GetObjectType(hOldRgn)==OBJ_REGION);
    ::SelectClipRgn(hDC, hOldRgn);
    ::DeleteObject(hOldRgn);
    ::DeleteObject(hRgn);
}

void CRenderClip::GenerateClip(HDC hDC, RECT rc, CRenderClip& clip)
{
    RECT rcClip = { 0 };
    ::GetClipBox(hDC, &rcClip);
    clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
    clip.hRgn = ::CreateRectRgnIndirect(&rc);
    ::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
    clip.hDC = hDC;
    clip.rcItem = rc;
}

void CRenderClip::GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CRenderClip& clip)
{
    RECT rcClip = { 0 };
    ::GetClipBox(hDC, &rcClip);
    clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
    clip.hRgn = ::CreateRectRgnIndirect(&rc);
    HRGN hRgnItem = ::CreateRoundRectRgn(rcItem.left, rcItem.top, rcItem.right + 1, rcItem.bottom + 1, width * 2, height * 2);
    ::CombineRgn(clip.hRgn, clip.hRgn, hRgnItem, RGN_AND);
    ::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
    clip.hDC = hDC;
	clip.rcItem = rc;
    ::DeleteObject(hRgnItem);
}

void CRenderClip::UseOldClipBegin(HDC hDC, CRenderClip& clip)
{
    ::SelectClipRgn(hDC, clip.hOldRgn);
}

void CRenderClip::UseOldClipEnd(HDC hDC, CRenderClip& clip)
{
    ::SelectClipRgn(hDC, clip.hRgn);
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

static const float OneThird = 1.0f / 3;

static void RGBtoHSL(DWORD ARGB, float* H, float* S, float* L) {
    const float
        R = (float)GetRValue(ARGB),
        G = (float)GetGValue(ARGB),
        B = (float)GetBValue(ARGB),
        nR = (R<0?0:(R>255?255:R))/255,
        nG = (G<0?0:(G>255?255:G))/255,
        nB = (B<0?0:(B>255?255:B))/255,
        m = min(min(nR,nG),nB),
        M = max(max(nR,nG),nB);
    *L = (m + M)/2;
    if (M==m) *H = *S = 0;
    else {
        const float
            f = (nR==m)?(nG-nB):((nG==m)?(nB-nR):(nR-nG)),
            i = (nR==m)?3.0f:((nG==m)?5.0f:1.0f);
        *H = (i-f/(M-m));
        if (*H>=6) *H-=6;
        *H*=60;
        *S = (2*(*L)<=1)?((M-m)/(M+m)):((M-m)/(2-M-m));
    }
}

static void HSLtoRGB(DWORD* ARGB, float H, float S, float L) {
    const float
        q = 2*L<1?L*(1+S):(L+S-L*S),
        p = 2*L-q,
        h = H/360,
        tr = h + OneThird,
        tg = h,
        tb = h - OneThird,
        ntr = tr<0?tr+1:(tr>1?tr-1:tr),
        ntg = tg<0?tg+1:(tg>1?tg-1:tg),
        ntb = tb<0?tb+1:(tb>1?tb-1:tb),
        R = 255*(6*ntr<1?p+(q-p)*6*ntr:(2*ntr<1?q:(3*ntr<2?p+(q-p)*6*(2.0f*OneThird-ntr):p))),
        G = 255*(6*ntg<1?p+(q-p)*6*ntg:(2*ntg<1?q:(3*ntg<2?p+(q-p)*6*(2.0f*OneThird-ntg):p))),
        B = 255*(6*ntb<1?p+(q-p)*6*ntb:(2*ntb<1?q:(3*ntb<2?p+(q-p)*6*(2.0f*OneThird-ntb):p)));
    *ARGB &= 0xFF000000;
    *ARGB |= RGB( (BYTE)(R<0?0:(R>255?255:R)), (BYTE)(G<0?0:(G>255?255:G)), (BYTE)(B<0?0:(B>255?255:B)) );
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

DWORD CRenderEngine::AdjustColor(DWORD dwColor, short H, short S, short L)
{
    if( H == 180 && S == 100 && L == 100 ) return dwColor;
    float fH, fS, fL;
    float S1 = S / 100.0f;
    float L1 = L / 100.0f;
    RGBtoHSL(dwColor, &fH, &fS, &fL);
    fH += (H - 180);
    fH = fH > 0 ? fH : fH + 360; 
    fS *= S1;
    fL *= L1;
    HSLtoRGB(&dwColor, fH, fS, fL);
    return dwColor;
}

HBITMAP CRenderEngine::CreateBitmap(HDC hDC, int cx, int cy, BYTE** pBits)
{
	//建立up-left Bitmap
	BITMAPINFO bmi;
	::ZeroMemory(&bmi, sizeof(BITMAPINFO));
	LPBITMAPINFO lpbiSrc = NULL;

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = cx;
	bmi.bmiHeader.biHeight = -cy;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = cx * cy * 4;
	bmi.bmiHeader.biClrUsed = 0;
	HBITMAP hBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void **)pBits, NULL, NULL);

	return hBitmap;
}

void CRenderEngine::DrawImage(HDC hDC, HBITMAP hBitmap, const RECT& rcDest, const RECT& rcPaint,
								const RECT&rcSource, const RECT& rcCorner, bool isAlpha,
                                    BYTE uAlpha, bool hole, bool xtiled, bool ytiled)
{
    ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

    typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
    static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");

	ASSERT(lpAlphaBlend != NULL);
    if( hBitmap == NULL ) return;

    HDC hCloneDC = ::CreateCompatibleDC(hDC);
    HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
    ::SetStretchBltMode(hDC, HALFTONE);

    RECT rcTemp = {0};
    RECT rcRealDest = {0};//实际的绘制位置
    if( isAlpha || uAlpha < 255 ) 
	{
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, uAlpha, AC_SRC_ALPHA };
        // middle
        if( !hole ) {
            rcRealDest.left = rcDest.left + rcCorner.left;
            rcRealDest.top = rcDest.top + rcCorner.top;
            rcRealDest.right = rcDest.right - rcDest.left - rcCorner.left - rcCorner.right;
            rcRealDest.bottom = rcDest.bottom - rcDest.top - rcCorner.top - rcCorner.bottom;
            rcRealDest.right += rcRealDest.left;
            rcRealDest.bottom += rcRealDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) ) 
			{
                if( !xtiled && !ytiled ) 
				{
                    rcRealDest.right -= rcRealDest.left;
                    rcRealDest.bottom -= rcRealDest.top;
                    lpAlphaBlend(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                        rcSource.left + rcCorner.left, rcSource.top + rcCorner.top, \
                        rcSource.right - rcSource.left - rcCorner.left - rcCorner.right, \
                        rcSource.bottom - rcSource.top - rcCorner.top - rcCorner.bottom, bf);
                }
                else if( xtiled && ytiled ) 
				{
                    LONG lWidth = rcSource.right - rcSource.left - rcCorner.left - rcCorner.right;
                    LONG lHeight = rcSource.bottom - rcSource.top - rcCorner.top - rcCorner.bottom;
                    int iTimesX = (rcRealDest.right - rcRealDest.left + lWidth - 1) / lWidth;
                    int iTimesY = (rcRealDest.bottom - rcRealDest.top + lHeight - 1) / lHeight;
                    for( int j = 0; j < iTimesY; ++j )
					{
                        LONG lDestTop = rcRealDest.top + lHeight * j;
                        LONG lDestBottom = rcRealDest.top + lHeight * (j + 1);
                        LONG lDrawHeight = lHeight;
                        if( lDestBottom > rcRealDest.bottom ) 
						{
                            lDrawHeight -= lDestBottom - rcRealDest.bottom;
                            lDestBottom = rcRealDest.bottom;
                        }
                        for( int i = 0; i < iTimesX; ++i ) 
						{
                            LONG lDestLeft = rcRealDest.left + lWidth * i;
                            LONG lDestRight = rcRealDest.left + lWidth * (i + 1);
                            LONG lDrawWidth = lWidth;
                            if( lDestRight > rcRealDest.right )
							{
                                lDrawWidth -= lDestRight - rcRealDest.right;
                                lDestRight = rcRealDest.right;
                            }
                            lpAlphaBlend(hDC, rcRealDest.left + lWidth * i, rcRealDest.top + lHeight * j, 
                                lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, 
                                rcSource.left + rcCorner.left, rcSource.top + rcCorner.top, lDrawWidth, lDrawHeight, bf);
                        }
                    }
                }
                else if( xtiled )
				{
                    LONG lWidth = rcSource.right - rcSource.left - rcCorner.left - rcCorner.right;
                    int iTimes = (rcRealDest.right - rcRealDest.left + lWidth - 1) / lWidth;
                    for( int i = 0; i < iTimes; ++i ) {
                        LONG lDestLeft = rcRealDest.left + lWidth * i;
                        LONG lDestRight = rcRealDest.left + lWidth * (i + 1);
                        LONG lDrawWidth = lWidth;
                        if( lDestRight > rcRealDest.right ) {
                            lDrawWidth -= lDestRight - rcRealDest.right;
                            lDestRight = rcRealDest.right;
                        }
                        lpAlphaBlend(hDC, lDestLeft, rcRealDest.top, lDestRight - lDestLeft, rcRealDest.bottom, 
                            hCloneDC, rcSource.left + rcCorner.left, rcSource.top + rcCorner.top, \
                            lDrawWidth, rcSource.bottom - rcSource.top - rcCorner.top - rcCorner.bottom, bf);
                    }
                }
                else 
				{ // ytiled
                    LONG lHeight = rcSource.bottom - rcSource.top - rcCorner.top - rcCorner.bottom;
                    int iTimes = (rcRealDest.bottom - rcRealDest.top + lHeight - 1) / lHeight;
                    for( int i = 0; i < iTimes; ++i ) 
					{
                        LONG lDestTop = rcRealDest.top + lHeight * i;
                        LONG lDestBottom = rcRealDest.top + lHeight * (i + 1);
                        LONG lDrawHeight = lHeight;
                        if( lDestBottom > rcRealDest.bottom ) 
						{
                            lDrawHeight -= lDestBottom - rcRealDest.bottom;
                            lDestBottom = rcRealDest.bottom;
                        }
                        lpAlphaBlend(hDC, rcRealDest.left, rcRealDest.top + lHeight * i, rcRealDest.right, lDestBottom - lDestTop, 
                            hCloneDC, rcSource.left + rcCorner.left, rcSource.top + rcCorner.top, \
                            rcSource.right - rcSource.left - rcCorner.left - rcCorner.right, lDrawHeight, bf);                    
                    }
                }
            }
        }

        // left-top
        if( rcCorner.left > 0 && rcCorner.top > 0 ) 
		{
            rcRealDest.left = rcDest.left;
            rcRealDest.top = rcDest.top;
            rcRealDest.right = rcCorner.left;
            rcRealDest.bottom = rcCorner.top;
            rcRealDest.right += rcRealDest.left;
            rcRealDest.bottom += rcRealDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) )
			{
                rcRealDest.right -= rcRealDest.left;
                rcRealDest.bottom -= rcRealDest.top;
                lpAlphaBlend(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                    rcSource.left, rcSource.top, rcCorner.left, rcCorner.top, bf);
            }
        }
        // top
        if( rcCorner.top > 0 )
		{
            rcRealDest.left = rcDest.left + rcCorner.left;
            rcRealDest.top = rcDest.top;
            rcRealDest.right = rcDest.right - rcDest.left - rcCorner.left - rcCorner.right;
            rcRealDest.bottom = rcCorner.top;
            rcRealDest.right += rcRealDest.left;
            rcRealDest.bottom += rcRealDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) ) 
			{
                rcRealDest.right -= rcRealDest.left;
                rcRealDest.bottom -= rcRealDest.top;
                lpAlphaBlend(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                    rcSource.left + rcCorner.left, rcSource.top, rcSource.right - rcSource.left - \
                    rcCorner.left - rcCorner.right, rcCorner.top, bf);
            }
        }
        // right-top
        if( rcCorner.right > 0 && rcCorner.top > 0 ) 
		{
            rcRealDest.left = rcDest.right - rcCorner.right;
            rcRealDest.top = rcDest.top;
            rcRealDest.right = rcCorner.right;
            rcRealDest.bottom = rcCorner.top;
            rcRealDest.right += rcRealDest.left;
            rcRealDest.bottom += rcRealDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) )
			{
                rcRealDest.right -= rcRealDest.left;
                rcRealDest.bottom -= rcRealDest.top;
                lpAlphaBlend(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                    rcSource.right - rcCorner.right, rcSource.top, rcCorner.right, rcCorner.top, bf);
            }
        }
        // left
        if( rcCorner.left > 0 ) 
		{
            rcRealDest.left = rcDest.left;
            rcRealDest.top = rcDest.top + rcCorner.top;
            rcRealDest.right = rcCorner.left;
            rcRealDest.bottom = rcDest.bottom - rcDest.top - rcCorner.top - rcCorner.bottom;
            rcRealDest.right += rcRealDest.left;
            rcRealDest.bottom += rcRealDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) ) 
			{
                rcRealDest.right -= rcRealDest.left;
                rcRealDest.bottom -= rcRealDest.top;
                lpAlphaBlend(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                    rcSource.left, rcSource.top + rcCorner.top, rcCorner.left, rcSource.bottom - \
                    rcSource.top - rcCorner.top - rcCorner.bottom, bf);
            }
        }
        // right
        if( rcCorner.right > 0 ) 
		{
            rcRealDest.left = rcDest.right - rcCorner.right;
            rcRealDest.top = rcDest.top + rcCorner.top;
            rcRealDest.right = rcCorner.right;
            rcRealDest.bottom = rcDest.bottom - rcDest.top - rcCorner.top - rcCorner.bottom;
            rcRealDest.right += rcRealDest.left;
            rcRealDest.bottom += rcRealDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) )
			{
                rcRealDest.right -= rcRealDest.left;
                rcRealDest.bottom -= rcRealDest.top;
                lpAlphaBlend(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                    rcSource.right - rcCorner.right, rcSource.top + rcCorner.top, rcCorner.right, \
                    rcSource.bottom - rcSource.top - rcCorner.top - rcCorner.bottom, bf);
            }
        }
        // left-bottom
        if( rcCorner.left > 0 && rcCorner.bottom > 0 ) 
		{
            rcRealDest.left = rcDest.left;
            rcRealDest.top = rcDest.bottom - rcCorner.bottom;
            rcRealDest.right = rcCorner.left;
            rcRealDest.bottom = rcCorner.bottom;
            rcRealDest.right += rcRealDest.left;
            rcRealDest.bottom += rcRealDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) )
			{
                rcRealDest.right -= rcRealDest.left;
                rcRealDest.bottom -= rcRealDest.top;
                lpAlphaBlend(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                    rcSource.left, rcSource.bottom - rcCorner.bottom, rcCorner.left, rcCorner.bottom, bf);
            }
        }
        // bottom
        if( rcCorner.bottom > 0 )
		{
            rcRealDest.left = rcDest.left + rcCorner.left;
            rcRealDest.top = rcDest.bottom - rcCorner.bottom;
            rcRealDest.right = rcDest.right - rcDest.left - rcCorner.left - rcCorner.right;
            rcRealDest.bottom = rcCorner.bottom;
            rcRealDest.right += rcRealDest.left;
            rcRealDest.bottom += rcRealDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) )
			{
                rcRealDest.right -= rcRealDest.left;
                rcRealDest.bottom -= rcRealDest.top;
                lpAlphaBlend(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                    rcSource.left + rcCorner.left, rcSource.bottom - rcCorner.bottom, \
                    rcSource.right - rcSource.left - rcCorner.left - rcCorner.right, rcCorner.bottom, bf);
            }
        }
        // right-bottom
        if( rcCorner.right > 0 && rcCorner.bottom > 0 ) 
		{
            rcRealDest.left = rcDest.right - rcCorner.right;
            rcRealDest.top = rcDest.bottom - rcCorner.bottom;
            rcRealDest.right = rcCorner.right;
            rcRealDest.bottom = rcCorner.bottom;
            rcRealDest.right += rcRealDest.left;
            rcRealDest.bottom += rcRealDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) ) 
			{
                rcRealDest.right -= rcRealDest.left;
                rcRealDest.bottom -= rcRealDest.top;
                lpAlphaBlend(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                    rcSource.right - rcCorner.right, rcSource.bottom - rcCorner.bottom, rcCorner.right, \
                    rcCorner.bottom, bf);
            }
        }
    }
    else 
    {
        if (rcDest.right - rcDest.left == rcSource.right - rcSource.left \
            && rcDest.bottom - rcDest.top == rcSource.bottom - rcSource.top \
            && rcCorner.left == 0 && rcCorner.right == 0 && rcCorner.top == 0 && rcCorner.bottom == 0)
        {
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) 
			{
                ::BitBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top, \
                    hCloneDC, rcSource.left + rcTemp.left - rcDest.left, rcSource.top + rcTemp.top - rcDest.top, SRCCOPY);
            }
        }
        else
        {
            // middle
            if( !hole ) 
			{
                rcRealDest.left = rcDest.left + rcCorner.left;
                rcRealDest.top = rcDest.top + rcCorner.top;
                rcRealDest.right = rcDest.right - rcDest.left - rcCorner.left - rcCorner.right;
                rcRealDest.bottom = rcDest.bottom - rcDest.top - rcCorner.top - rcCorner.bottom;
                rcRealDest.right += rcRealDest.left;
                rcRealDest.bottom += rcRealDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) )
				{
                    if( !xtiled && !ytiled ) {
                        rcRealDest.right -= rcRealDest.left;
                        rcRealDest.bottom -= rcRealDest.top;
                        ::StretchBlt(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                            rcSource.left + rcCorner.left, rcSource.top + rcCorner.top, \
                            rcSource.right - rcSource.left - rcCorner.left - rcCorner.right, \
                            rcSource.bottom - rcSource.top - rcCorner.top - rcCorner.bottom, SRCCOPY);
                    }
                    else if( xtiled && ytiled )
					{
                        LONG lWidth = rcSource.right - rcSource.left - rcCorner.left - rcCorner.right;
                        LONG lHeight = rcSource.bottom - rcSource.top - rcCorner.top - rcCorner.bottom;
                        int iTimesX = (rcRealDest.right - rcRealDest.left + lWidth - 1) / lWidth;
                        int iTimesY = (rcRealDest.bottom - rcRealDest.top + lHeight - 1) / lHeight;
                        for( int j = 0; j < iTimesY; ++j ) 
						{
                            LONG lDestTop = rcRealDest.top + lHeight * j;
                            LONG lDestBottom = rcRealDest.top + lHeight * (j + 1);
                            LONG lDrawHeight = lHeight;
                            if( lDestBottom > rcRealDest.bottom ) 
							{
                                lDrawHeight -= lDestBottom - rcRealDest.bottom;
                                lDestBottom = rcRealDest.bottom;
                            }
                            for( int i = 0; i < iTimesX; ++i ) 
							{
                                LONG lDestLeft = rcRealDest.left + lWidth * i;
                                LONG lDestRight = rcRealDest.left + lWidth * (i + 1);
                                LONG lDrawWidth = lWidth;
                                if( lDestRight > rcRealDest.right )
								{
                                    lDrawWidth -= lDestRight - rcRealDest.right;
                                    lDestRight = rcRealDest.right;
                                }
                                ::BitBlt(hDC, rcRealDest.left + lWidth * i, rcRealDest.top + lHeight * j, \
                                    lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, \
                                    rcSource.left + rcCorner.left, rcSource.top + rcCorner.top, SRCCOPY);
                            }
                        }
                    }
                    else if( xtiled ) 
					{
                        LONG lWidth = rcSource.right - rcSource.left - rcCorner.left - rcCorner.right;
                        int iTimes = (rcRealDest.right - rcRealDest.left + lWidth - 1) / lWidth;
                        for( int i = 0; i < iTimes; ++i )
						{
                            LONG lDestLeft = rcRealDest.left + lWidth * i;
                            LONG lDestRight = rcRealDest.left + lWidth * (i + 1);
                            LONG lDrawWidth = lWidth;
                            if( lDestRight > rcRealDest.right ) 
							{
                                lDrawWidth -= lDestRight - rcRealDest.right;
                                lDestRight = rcRealDest.right;
                            }
                            ::StretchBlt(hDC, lDestLeft, rcRealDest.top, lDestRight - lDestLeft, rcRealDest.bottom, 
                                hCloneDC, rcSource.left + rcCorner.left, rcSource.top + rcCorner.top, \
                                lDrawWidth, rcSource.bottom - rcSource.top - rcCorner.top - rcCorner.bottom, SRCCOPY);
                        }
                    }
                    else
					{ // ytiled
                        LONG lHeight = rcSource.bottom - rcSource.top - rcCorner.top - rcCorner.bottom;
                        int iTimes = (rcRealDest.bottom - rcRealDest.top + lHeight - 1) / lHeight;
                        for( int i = 0; i < iTimes; ++i ) 
						{
                            LONG lDestTop = rcRealDest.top + lHeight * i;
                            LONG lDestBottom = rcRealDest.top + lHeight * (i + 1);
                            LONG lDrawHeight = lHeight;
                            if( lDestBottom > rcRealDest.bottom ) 
							{
                                lDrawHeight -= lDestBottom - rcRealDest.bottom;
                                lDestBottom = rcRealDest.bottom;
                            }
                            ::StretchBlt(hDC, rcRealDest.left, rcRealDest.top + lHeight * i, rcRealDest.right, lDestBottom - lDestTop, 
                                hCloneDC, rcSource.left + rcCorner.left, rcSource.top + rcCorner.top, \
                                rcSource.right - rcSource.left - rcCorner.left - rcCorner.right, lDrawHeight, SRCCOPY);                    
                        }
                    }
                }
            }
            
            // left-top
            if( rcCorner.left > 0 && rcCorner.top > 0 ) 
			{
                rcRealDest.left = rcDest.left;
                rcRealDest.top = rcDest.top;
                rcRealDest.right = rcCorner.left;
                rcRealDest.bottom = rcCorner.top;
                rcRealDest.right += rcRealDest.left;
                rcRealDest.bottom += rcRealDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) ) 
				{
                    rcRealDest.right -= rcRealDest.left;
                    rcRealDest.bottom -= rcRealDest.top;
                    ::StretchBlt(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                        rcSource.left, rcSource.top, rcCorner.left, rcCorner.top, SRCCOPY);
                }
            }
            // top
            if( rcCorner.top > 0 ) 
			{
                rcRealDest.left = rcDest.left + rcCorner.left;
                rcRealDest.top = rcDest.top;
                rcRealDest.right = rcDest.right - rcDest.left - rcCorner.left - rcCorner.right;
                rcRealDest.bottom = rcCorner.top;
                rcRealDest.right += rcRealDest.left;
                rcRealDest.bottom += rcRealDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) ) 
				{
                    rcRealDest.right -= rcRealDest.left;
                    rcRealDest.bottom -= rcRealDest.top;
                    ::StretchBlt(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                        rcSource.left + rcCorner.left, rcSource.top, rcSource.right - rcSource.left - \
                        rcCorner.left - rcCorner.right, rcCorner.top, SRCCOPY);
                }
            }
            // right-top
            if( rcCorner.right > 0 && rcCorner.top > 0 ) 
			{
                rcRealDest.left = rcDest.right - rcCorner.right;
                rcRealDest.top = rcDest.top;
                rcRealDest.right = rcCorner.right;
                rcRealDest.bottom = rcCorner.top;
                rcRealDest.right += rcRealDest.left;
                rcRealDest.bottom += rcRealDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) ) 
				{
                    rcRealDest.right -= rcRealDest.left;
                    rcRealDest.bottom -= rcRealDest.top;
                    ::StretchBlt(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                        rcSource.right - rcCorner.right, rcSource.top, rcCorner.right, rcCorner.top, SRCCOPY);
                }
            }
            // left
            if( rcCorner.left > 0 )
			{
                rcRealDest.left = rcDest.left;
                rcRealDest.top = rcDest.top + rcCorner.top;
                rcRealDest.right = rcCorner.left;
                rcRealDest.bottom = rcDest.bottom - rcDest.top - rcCorner.top - rcCorner.bottom;
                rcRealDest.right += rcRealDest.left;
                rcRealDest.bottom += rcRealDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) ) 
				{
                    rcRealDest.right -= rcRealDest.left;
                    rcRealDest.bottom -= rcRealDest.top;
                    ::StretchBlt(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                        rcSource.left, rcSource.top + rcCorner.top, rcCorner.left, rcSource.bottom - \
                        rcSource.top - rcCorner.top - rcCorner.bottom, SRCCOPY);
                }
            }
            // right
            if( rcCorner.right > 0 )
			{
                rcRealDest.left = rcDest.right - rcCorner.right;
                rcRealDest.top = rcDest.top + rcCorner.top;
                rcRealDest.right = rcCorner.right;
                rcRealDest.bottom = rcDest.bottom - rcDest.top - rcCorner.top - rcCorner.bottom;
                rcRealDest.right += rcRealDest.left;
                rcRealDest.bottom += rcRealDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) ) {
                    rcRealDest.right -= rcRealDest.left;
                    rcRealDest.bottom -= rcRealDest.top;
                    ::StretchBlt(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                        rcSource.right - rcCorner.right, rcSource.top + rcCorner.top, rcCorner.right, \
                        rcSource.bottom - rcSource.top - rcCorner.top - rcCorner.bottom, SRCCOPY);
                }
            }
            // left-bottom
            if( rcCorner.left > 0 && rcCorner.bottom > 0 ) 
			{
                rcRealDest.left = rcDest.left;
                rcRealDest.top = rcDest.bottom - rcCorner.bottom;
                rcRealDest.right = rcCorner.left;
                rcRealDest.bottom = rcCorner.bottom;
                rcRealDest.right += rcRealDest.left;
                rcRealDest.bottom += rcRealDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) )
				{
                    rcRealDest.right -= rcRealDest.left;
                    rcRealDest.bottom -= rcRealDest.top;
                    ::StretchBlt(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                        rcSource.left, rcSource.bottom - rcCorner.bottom, rcCorner.left, rcCorner.bottom, SRCCOPY);
                }
            }
            // bottom
            if( rcCorner.bottom > 0 )
			{
                rcRealDest.left = rcDest.left + rcCorner.left;
                rcRealDest.top = rcDest.bottom - rcCorner.bottom;
                rcRealDest.right = rcDest.right - rcDest.left - rcCorner.left - rcCorner.right;
                rcRealDest.bottom = rcCorner.bottom;
                rcRealDest.right += rcRealDest.left;
                rcRealDest.bottom += rcRealDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) ) 
				{
                    rcRealDest.right -= rcRealDest.left;
                    rcRealDest.bottom -= rcRealDest.top;
                    ::StretchBlt(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                        rcSource.left + rcCorner.left, rcSource.bottom - rcCorner.bottom, \
                        rcSource.right - rcSource.left - rcCorner.left - rcCorner.right, rcCorner.bottom, SRCCOPY);
                }
            }
            // right-bottom
            if( rcCorner.right > 0 && rcCorner.bottom > 0 ) 
			{
                rcRealDest.left = rcDest.right - rcCorner.right;
                rcRealDest.top = rcDest.bottom - rcCorner.bottom;
                rcRealDest.right = rcCorner.right;
                rcRealDest.bottom = rcCorner.bottom;
                rcRealDest.right += rcRealDest.left;
                rcRealDest.bottom += rcRealDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcRealDest) )
				{
                    rcRealDest.right -= rcRealDest.left;
                    rcRealDest.bottom -= rcRealDest.top;
                    ::StretchBlt(hDC, rcRealDest.left, rcRealDest.top, rcRealDest.right, rcRealDest.bottom, hCloneDC, \
                        rcSource.right - rcCorner.right, rcSource.bottom - rcCorner.bottom, rcCorner.right, \
                        rcCorner.bottom, SRCCOPY);
                }
            }
        }
    }

    ::SelectObject(hCloneDC, hOldBitmap);
    ::DeleteDC(hCloneDC);
}

bool CRenderEngine::DrawImage(HDC hDC, CPaintManagerUI* pManager, const RECT& rcControl, const RECT& rcPaint, CImageAttribute& image)
{
	if ((pManager == NULL) || (hDC == NULL))
		return false;

	if (image.m_sImage.IsEmpty())
		return false;

	TImageInfo* data = image.m_imageInfo;
	if (!data) return false;

	RECT rcDest = image.m_rcDest;
	rcDest.left += rcControl.left;
	rcDest.top += rcControl.top;
	if (image.m_rcDest.top == 0 && image.m_rcDest.left == 0 && image.m_rcDest.right == 0 && image.m_rcDest.bottom == 0)
	{
		//如果为空则赋值为控件值
		rcDest.right += rcControl.right;
		rcDest.bottom += rcControl.bottom;
	}
	else
	{
		//不为空则计算右下角坐标值
		//计算图片的宽高
		int nWidth = image.m_rcDest.right - image.m_rcDest.left;
		int nHeight = image.m_rcDest.bottom - image.m_rcDest.top;
		rcDest.right = rcDest.left + nWidth;
		rcDest.bottom = rcDest.top + nHeight;
	}

	if (rcDest.right > rcControl.right)
		rcDest.right = rcControl.right;	
	if (rcDest.bottom > rcControl.bottom)
		rcDest.bottom = rcControl.bottom;

	RECT rcTemp;
	if (!::IntersectRect(&rcTemp, &rcDest, &rcControl)) return true;
	if (!::IntersectRect(&rcTemp, &rcDest, &rcPaint)) return true;

	CRenderEngine::DrawImage(hDC, data->hBitmap, rcDest, rcPaint, image.m_rcSource, image.m_rcCorner,
		pManager->IsLayeredWindow() ? true : data->alphaChannel,
		image.m_bFade, image.m_bHole, image.m_bTiledX, image.m_bTiledY);

	return true;
}

void CRenderEngine::DrawText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, int iFont, UINT uStyle)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	if (pstrText == NULL || pManager == NULL) return;

	if (pManager->IsLayeredWindow() || pManager->IsUseGdiplusText())
	{
		Gdiplus::Graphics graphics(hDC);

		Gdiplus::Font font(hDC, pManager->GetFont(iFont));
		Gdiplus::RectF rectF((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.top, (Gdiplus::REAL)(rc.right - rc.left), (Gdiplus::REAL)(rc.bottom - rc.top));
		Gdiplus::SolidBrush brush(Gdiplus::Color(254, GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));

		Gdiplus::StringFormat stringFormat = Gdiplus::StringFormat::GenericTypographic();

		if ((uStyle & DT_END_ELLIPSIS) != 0) {
			stringFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
		}

		int formatFlags = 0;
		if ((uStyle & DT_NOCLIP) != 0) {
			formatFlags |= Gdiplus::StringFormatFlagsNoClip;
		}
		if ((uStyle & DT_SINGLELINE) != 0) {
			formatFlags |= Gdiplus::StringFormatFlagsNoWrap;
		}

		stringFormat.SetFormatFlags(formatFlags);

		if ((uStyle & DT_LEFT) != 0) {
			stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
		}
		else if ((uStyle & DT_CENTER) != 0) {
			stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		}
		else if ((uStyle & DT_RIGHT) != 0) {
			stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);
		}
		else {
			stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
		}
		stringFormat.GenericTypographic();
		if ((uStyle & DT_TOP) != 0) {
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
		}
		else if ((uStyle & DT_VCENTER) != 0) {
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		}
		else if ((uStyle & DT_BOTTOM) != 0) {
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
		}
		else {
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
		}

		if ((uStyle & DT_CALCRECT) != 0)
		{
			Gdiplus::RectF bounds;
			graphics.MeasureString(pstrText, -1, &font, rectF, &stringFormat, &bounds);

			// MeasureString存在计算误差，这里加一像素
			rc.bottom = rc.top + (long)bounds.Height + 1;
			rc.right = rc.left + (long)bounds.Width + 1;
		}
		else
		{
			graphics.DrawString(pstrText, -1, &font, rectF, &stringFormat, &brush);
		}
	}
	else
	{
		::SetBkMode(hDC, TRANSPARENT);
		::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
		HFONT hOldFont = (HFONT)::SelectObject(hDC, pManager->GetFont(iFont));
		::DrawText(hDC, pstrText, -1, &rc, uStyle | DT_NOPREFIX);
		::SelectObject(hDC, hOldFont);
	}
}

void CRenderEngine::DrawColor(HDC hDC, const RECT& rc, DWORD color)
{
    if( color <= 0x00FFFFFF ) return;

	Gdiplus::Graphics graphics(hDC);
	Gdiplus::SolidBrush brush(Gdiplus::Color((LOBYTE((color) >> 24)), GetBValue(color), GetGValue(color), GetRValue(color)));

	graphics.FillRectangle(&brush, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);

}

void CRenderEngine::DrawGradient(HDC hDC, const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps)
{
    typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
    static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");
    ASSERT( lpAlphaBlend != NULL );
    typedef BOOL (WINAPI *PGradientFill)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
    static PGradientFill lpGradientFill = (PGradientFill) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "GradientFill");

    BYTE bAlpha = (BYTE)(((dwFirst >> 24) + (dwSecond >> 24)) >> 1);
    if( bAlpha == 0 ) return;
    int cx = rc.right - rc.left;
    int cy = rc.bottom - rc.top;
    RECT rcPaint = rc;
    HDC hPaintDC = hDC;
    HBITMAP hPaintBitmap = NULL;
    HBITMAP hOldPaintBitmap = NULL;
    if( bAlpha < 255 ) {
        rcPaint.left = rcPaint.top = 0;
        rcPaint.right = cx;
        rcPaint.bottom = cy;
        hPaintDC = ::CreateCompatibleDC(hDC);
        hPaintBitmap = ::CreateCompatibleBitmap(hDC, cx, cy);
        ASSERT(hPaintDC);
        ASSERT(hPaintBitmap);
        hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
    }
    if( lpGradientFill != NULL ) 
    {
        TRIVERTEX triv[2] = 
        {
            { rcPaint.left, rcPaint.top, GetBValue(dwFirst) << 8, GetGValue(dwFirst) << 8, GetRValue(dwFirst) << 8, 0xFF00 },
            { rcPaint.right, rcPaint.bottom, GetBValue(dwSecond) << 8, GetGValue(dwSecond) << 8, GetRValue(dwSecond) << 8, 0xFF00 }
        };
        GRADIENT_RECT grc = { 0, 1 };
        lpGradientFill(hPaintDC, triv, 2, &grc, 1, bVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
    }
    else 
    {
        // Determine how many shades
        int nShift = 1;
        if( nSteps >= 64 ) nShift = 6;
        else if( nSteps >= 32 ) nShift = 5;
        else if( nSteps >= 16 ) nShift = 4;
        else if( nSteps >= 8 ) nShift = 3;
        else if( nSteps >= 4 ) nShift = 2;
        int nLines = 1 << nShift;
        for( int i = 0; i < nLines; i++ ) {
            // Do a little alpha blending
            BYTE bR = (BYTE) ((GetBValue(dwSecond) * (nLines - i) + GetBValue(dwFirst) * i) >> nShift);
            BYTE bG = (BYTE) ((GetGValue(dwSecond) * (nLines - i) + GetGValue(dwFirst) * i) >> nShift);
            BYTE bB = (BYTE) ((GetRValue(dwSecond) * (nLines - i) + GetRValue(dwFirst) * i) >> nShift);
            // ... then paint with the resulting color
            HBRUSH hBrush = ::CreateSolidBrush(RGB(bR,bG,bB));
            RECT r2 = rcPaint;
            if( bVertical ) {
                r2.bottom = rc.bottom - ((i * (rc.bottom - rc.top)) >> nShift);
                r2.top = rc.bottom - (((i + 1) * (rc.bottom - rc.top)) >> nShift);
                if( (r2.bottom - r2.top) > 0 ) ::FillRect(hDC, &r2, hBrush);
            }
            else {
                r2.left = rc.right - (((i + 1) * (rc.right - rc.left)) >> nShift);
                r2.right = rc.right - ((i * (rc.right - rc.left)) >> nShift);
                if( (r2.right - r2.left) > 0 ) ::FillRect(hPaintDC, &r2, hBrush);
            }
            ::DeleteObject(hBrush);
        }
    }
    if( bAlpha < 255 ) {
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, bAlpha, AC_SRC_ALPHA };
        lpAlphaBlend(hDC, rc.left, rc.top, cx, cy, hPaintDC, 0, 0, cx, cy, bf);
        ::SelectObject(hPaintDC, hOldPaintBitmap);
        ::DeleteObject(hPaintBitmap);
        ::DeleteDC(hPaintDC);
    }
}

void CRenderEngine::DrawLine( HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor,int nStyle /*= PS_SOLID*/ )
{
#if USE_GDI_RENDER
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

	LOGPEN lg;
	lg.lopnColor = RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor));
	lg.lopnStyle = nStyle;
	lg.lopnWidth.x = nSize;
	HPEN hPen = CreatePenIndirect(&lg);
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	POINT ptTemp = { 0 };
	::MoveToEx(hDC, rc.left, rc.top, &ptTemp);
	::LineTo(hDC, rc.right, rc.bottom);
	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
#else
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

	Gdiplus::Graphics graphics(hDC);
	Gdiplus::Pen pen(Gdiplus::Color(dwPenColor), (Gdiplus::REAL)nSize);
	pen.SetDashStyle((Gdiplus::DashStyle)nStyle);
	graphics.DrawLine(&pen, Gdiplus::Point(rc.left, rc.top), Gdiplus::Point(rc.right, rc.bottom));

#endif
}

void CRenderEngine::DrawRect(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor)
{
#if USE_GDI_RENDER
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
	::Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
#else
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	Gdiplus::Graphics graphics(hDC);
	Gdiplus::Pen pen(Gdiplus::Color(dwPenColor), (Gdiplus::REAL)nSize);
	pen.SetAlignment(Gdiplus::PenAlignmentInset);

	graphics.DrawRectangle(&pen, rc.left, rc.top, rc.right-rc.left-1, rc.bottom-rc.top-1);
#endif

}

void CRenderEngine::DrawRoundRect(HDC hDC, const RECT& rc, int nSize, int width, int height, DWORD dwPenColor)
{
#ifdef USE_GDI_RENDER
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
	::RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, width * 2, height * 2);
	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
#else
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	Gdiplus::Graphics graphics(hDC);
	Gdiplus::Pen pen(Gdiplus::Color(dwPenColor), (Gdiplus::REAL)nSize);
	pen.SetAlignment(Gdiplus::PenAlignmentInset);

	Gdiplus::GraphicsPath path;
	// 把圆角矩形分成直线、弧的组合，依次加到路径中
	//左
	path.AddLine(Gdiplus::Point(rc.left, rc.bottom - height), Gdiplus::Point(rc.left, rc.top + height));
	//左上
	path.AddArc(rc.left, rc.top, width * 2, height * 2, (Gdiplus::REAL)180, (Gdiplus::REAL)90);

	//上
	path.AddLine(Gdiplus::Point(rc.left + width, rc.top), Gdiplus::Point(rc.right - width, rc.top));
	//右上
	path.AddArc(rc.right - width * 2-1, rc.top, width * 2, height * 2, (Gdiplus::REAL)270, (Gdiplus::REAL)90);

	//右
	path.AddLine(Gdiplus::Point(rc.right-1, rc.top + height), Gdiplus::Point(rc.right-1, rc.bottom - height));
	//右下
	path.AddArc(rc.right - width * 2-1, rc.bottom - height * 2-1, width * 2, height * 2, (Gdiplus::REAL)0, (Gdiplus::REAL)90);

	//下
	path.AddLine(Gdiplus::Point(rc.right - width, rc.bottom-1), Gdiplus::Point(rc.left + width, rc.bottom-1));
	//左下
	path.AddArc(rc.left, rc.bottom - height * 2-1, width * 2, height * 2, (Gdiplus::REAL)90, (Gdiplus::REAL)90);
	
	graphics.DrawPath(&pen, &path);
#endif

}

void CRenderEngine::DrawHtmlText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, RECT* prcLinks, CDuiString* sLinks, int& nLinkRects, UINT uStyle)
{
    // 考虑到在xml编辑器中使用<>符号不方便，可以使用{}符号代替
    // 支持标签嵌套（如<l><b>text</b></l>），但是交叉嵌套是应该避免的（如<l><b>text</l></b>）
    // The string formatter supports a kind of "mini-html" that consists of various short tags:
    //
    //   Bold:             <b>text</b>
    //   Color:            <c #xxxxxx>text</c>  where x = RGB in hex
    //   Font:             <f x>text</f>        where x = font id
    //   Italic:           <i>text</i>
    //   Image:            <i x y z>            where x = image name and y = imagelist num and z(optional) = imagelist id
    //   Link:             <a x>text</a>        where x(optional) = link content, normal like app:notepad or http:www.xxx.com
    //   NewLine           <n>                  
    //   Paragraph:        <p x>text</p>        where x = extra pixels indent in p
    //   Raw Text:         <r>text</r>
    //   Selected:         <s>text</s>
    //   Underline:        <u>text</u>
    //   X Indent:         <x i>                where i = hor indent in pixels
    //   Y Indent:         <y i>                where i = ver indent in pixels 

    ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
    if( pstrText == NULL || pManager == NULL ) return;
    if( ::IsRectEmpty(&rc) ) return;

    bool bDraw = (uStyle & DT_CALCRECT) == 0;

    CStdPtrArray aFontArray(10);
    CStdPtrArray aColorArray(10);
    CStdPtrArray aPIndentArray(10);

    RECT rcClip = { 0 };
    ::GetClipBox(hDC, &rcClip);
    HRGN hOldRgn = ::CreateRectRgnIndirect(&rcClip);
    HRGN hRgn = ::CreateRectRgnIndirect(&rc);
    if( bDraw ) ::ExtSelectClipRgn(hDC, hRgn, RGN_AND);

    TEXTMETRIC* pTm = &pManager->GetDefaultFontInfo()->tm;
    HFONT hOldFont = (HFONT) ::SelectObject(hDC, pManager->GetDefaultFontInfo()->hFont);
    ::SetBkMode(hDC, TRANSPARENT);
    ::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
    DWORD dwBkColor = pManager->GetDefaultSelectedBkColor();
    ::SetBkColor(hDC, RGB(GetBValue(dwBkColor), GetGValue(dwBkColor), GetRValue(dwBkColor)));

    // If the drawstyle include a alignment, we'll need to first determine the text-size so
    // we can draw it at the correct position...
	if( ((uStyle & DT_CENTER) != 0 || (uStyle & DT_RIGHT) != 0 || (uStyle & DT_VCENTER) != 0 || (uStyle & DT_BOTTOM) != 0) && (uStyle & DT_CALCRECT) == 0 ) {
		RECT rcText = { 0, 0, 9999, 100 };
		int nLinks = 0;
		DrawHtmlText(hDC, pManager, rcText, pstrText, dwTextColor, NULL, NULL, nLinks, uStyle | DT_CALCRECT);
		if( (uStyle & DT_SINGLELINE) != 0 ){
			if( (uStyle & DT_CENTER) != 0 ) {
				rc.left = rc.left + ((rc.right - rc.left) / 2) - ((rcText.right - rcText.left) / 2);
				rc.right = rc.left + (rcText.right - rcText.left);
			}
			if( (uStyle & DT_RIGHT) != 0 ) {
				rc.left = rc.right - (rcText.right - rcText.left);
			}
		}
		if( (uStyle & DT_VCENTER) != 0 ) {
			rc.top = rc.top + ((rc.bottom - rc.top) / 2) - ((rcText.bottom - rcText.top) / 2);
			rc.bottom = rc.top + (rcText.bottom - rcText.top);
		}
		if( (uStyle & DT_BOTTOM) != 0 ) {
			rc.top = rc.bottom - (rcText.bottom - rcText.top);
		}
	}

    bool bHoverLink = false;
    CDuiString sHoverLink;
    POINT ptMouse = pManager->GetMousePos();
    for( int i = 0; !bHoverLink && i < nLinkRects; i++ ) {
        if( ::PtInRect(prcLinks + i, ptMouse) ) {
            sHoverLink = *(CDuiString*)(sLinks + i);
            bHoverLink = true;
        }
    }

    POINT pt = { rc.left, rc.top };
    int iLinkIndex = 0;
    int cyLine = pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1);
    int cyMinHeight = 0;
    int cxMaxWidth = 0;
    POINT ptLinkStart = { 0 };
    bool bLineEnd = false;
    bool bInRaw = false;
    bool bInLink = false;
    bool bInSelected = false;
    int iLineLinkIndex = 0;

    // 排版习惯是图文底部对齐，所以每行绘制都要分两步，先计算高度，再绘制
    CStdPtrArray aLineFontArray;
    CStdPtrArray aLineColorArray;
    CStdPtrArray aLinePIndentArray;
    LPCTSTR pstrLineBegin = pstrText;
    bool bLineInRaw = false;
    bool bLineInLink = false;
    bool bLineInSelected = false;
    int cyLineHeight = 0;
    bool bLineDraw = false; // 行的第二阶段：绘制
    while( *pstrText != _T('\0') ) {
        if( pt.x >= rc.right || *pstrText == _T('\n') || bLineEnd ) {
            if( *pstrText == _T('\n') ) pstrText++;
            if( bLineEnd ) bLineEnd = false;
            if( !bLineDraw ) {
                if( bInLink && iLinkIndex < nLinkRects ) {
                    ::SetRect(&prcLinks[iLinkIndex++], ptLinkStart.x, ptLinkStart.y, MIN(pt.x, rc.right), pt.y + cyLine);
                    CDuiString *pStr1 = (CDuiString*)(sLinks + iLinkIndex - 1);
                    CDuiString *pStr2 = (CDuiString*)(sLinks + iLinkIndex);
                    *pStr2 = *pStr1;
                }
                for( int i = iLineLinkIndex; i < iLinkIndex; i++ ) {
                    prcLinks[i].bottom = pt.y + cyLine;
                }
                if( bDraw ) {
                    bInLink = bLineInLink;
                    iLinkIndex = iLineLinkIndex;
                }
            }
            else {
                if( bInLink && iLinkIndex < nLinkRects ) iLinkIndex++;
                bLineInLink = bInLink;
                iLineLinkIndex = iLinkIndex;
            }
            if( (uStyle & DT_SINGLELINE) != 0 && (!bDraw || bLineDraw) ) break;
            if( bDraw ) bLineDraw = !bLineDraw; // !
            pt.x = rc.left;
            if( !bLineDraw ) pt.y += cyLine;
            if( pt.y > rc.bottom && bDraw ) break;
            ptLinkStart = pt;
            cyLine = pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1);
            if( pt.x >= rc.right ) break;
        }
        else if( !bInRaw && ( *pstrText == _T('<') || *pstrText == _T('{') )
            && ( pstrText[1] >= _T('a') && pstrText[1] <= _T('z') )
            && ( pstrText[2] == _T(' ') || pstrText[2] == _T('>') || pstrText[2] == _T('}') ) ) {
                pstrText++;
                LPCTSTR pstrNextStart = NULL;
                switch( *pstrText ) {
            case _T('a'):  // Link
                {
                    pstrText++;
                    while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                    if( iLinkIndex < nLinkRects && !bLineDraw ) {
                        CDuiString *pStr = (CDuiString*)(sLinks + iLinkIndex);
                        pStr->Empty();
                        while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) {
                            LPCTSTR pstrTemp = ::CharNext(pstrText);
                            while( pstrText < pstrTemp) {
                                *pStr += *pstrText++;
                            }
                        }
                    }

                    DWORD clrColor = pManager->GetDefaultLinkFontColor();
                    if( bHoverLink && iLinkIndex < nLinkRects ) {
                        CDuiString *pStr = (CDuiString*)(sLinks + iLinkIndex);
                        if( sHoverLink == *pStr ) clrColor = pManager->GetDefaultLinkHoverFontColor();
                    }
                    //else if( prcLinks == NULL ) {
                    //    if( ::PtInRect(&rc, ptMouse) )
                    //        clrColor = pManager->GetDefaultLinkHoverFontColor();
                    //}
                    aColorArray.Add((LPVOID)clrColor);
                    ::SetTextColor(hDC,  RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                    TFontInfo* pFontInfo = pManager->GetDefaultFontInfo();
                    if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
                    if( pFontInfo->bUnderline == false ) {
                        HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
                        if( hFont == NULL ) hFont = pManager->AddFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
                        pFontInfo = pManager->GetFontInfo(hFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        ::SelectObject(hDC, pFontInfo->hFont);
                        cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                    }
                    ptLinkStart = pt;
                    bInLink = true;
                }
                break;
            case _T('b'):  // Bold
                {
                    pstrText++;
                    TFontInfo* pFontInfo = pManager->GetDefaultFontInfo();
                    if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
                    if( pFontInfo->bBold == false ) {
                        HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, true, pFontInfo->bUnderline, pFontInfo->bItalic);
                        if( hFont == NULL ) hFont = pManager->AddFont(pFontInfo->sFontName, pFontInfo->iSize, true, pFontInfo->bUnderline, pFontInfo->bItalic);
                        pFontInfo = pManager->GetFontInfo(hFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        ::SelectObject(hDC, pFontInfo->hFont);
                        cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                    }
                }
                break;
            case _T('c'):  // Color
                {
                    pstrText++;
                    while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                    if( *pstrText == _T('#')) pstrText++;
                    DWORD clrColor = _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 16);
                    aColorArray.Add((LPVOID)clrColor);
                    ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                }
                break;
            case _T('f'):  // Font
                {
                    pstrText++;
                    while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                    LPCTSTR pstrTemp = pstrText;
                    int iFont = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
                    //if( isdigit(*pstrText) ) { // debug版本会引起异常
                    if( pstrTemp != pstrText ) {
                        TFontInfo* pFontInfo = pManager->GetFontInfo(iFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        ::SelectObject(hDC, pFontInfo->hFont);
                    }
                    else {
                        CDuiString sFontName;
                        int iFontSize = 10;
                        CDuiString sFontAttr;
                        bool bBold = false;
                        bool bUnderline = false;
                        bool bItalic = false;
                        while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') && *pstrText != _T(' ') ) {
                            pstrTemp = ::CharNext(pstrText);
                            while( pstrText < pstrTemp) {
                                sFontName += *pstrText++;
                            }
                        }
                        while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                        if( isdigit(*pstrText) ) {
                            iFontSize = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
                        }
                        while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                        while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) {
                            pstrTemp = ::CharNext(pstrText);
                            while( pstrText < pstrTemp) {
                                sFontAttr += *pstrText++;
                            }
                        }
                        sFontAttr.MakeLower();
                        if( sFontAttr.Find(_T("bold")) >= 0 ) bBold = true;
                        if( sFontAttr.Find(_T("underline")) >= 0 ) bUnderline = true;
                        if( sFontAttr.Find(_T("italic")) >= 0 ) bItalic = true;
                        HFONT hFont = pManager->GetFont(sFontName, iFontSize, bBold, bUnderline, bItalic);
                        if( hFont == NULL ) hFont = pManager->AddFont(sFontName, iFontSize, bBold, bUnderline, bItalic);
                        TFontInfo* pFontInfo = pManager->GetFontInfo(hFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        ::SelectObject(hDC, pFontInfo->hFont);
                    }
                    cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                }
                break;
            case _T('i'):  // Italic or Image
                {    
                    pstrNextStart = pstrText - 1;
                    pstrText++;
					CDuiString sImageString = pstrText;
                    int iWidth = 0;
                    int iHeight = 0;
                    while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                    const TImageInfo* pImageInfo = NULL;
                    CDuiString sName;
                    while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') && *pstrText != _T(' ') ) {
                        LPCTSTR pstrTemp = ::CharNext(pstrText);
                        while( pstrText < pstrTemp) {
                            sName += *pstrText++;
                        }
                    }
                    if( sName.IsEmpty() ) { // Italic
                        pstrNextStart = NULL;
                        TFontInfo* pFontInfo = pManager->GetDefaultFontInfo();
                        if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
                        if( pFontInfo->bItalic == false ) {
                            HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, pFontInfo->bUnderline, true);
                            if( hFont == NULL ) hFont = pManager->AddFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, pFontInfo->bUnderline, true);
                            pFontInfo = pManager->GetFontInfo(hFont);
                            aFontArray.Add(pFontInfo);
                            pTm = &pFontInfo->tm;
                            ::SelectObject(hDC, pFontInfo->hFont);
                            cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                        }
                    }
                    else {
                        while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                        int iImageListNum = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
						if( iImageListNum <= 0 ) iImageListNum = 1;
						while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
						int iImageListIndex = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
						if( iImageListIndex < 0 || iImageListIndex >= iImageListNum ) iImageListIndex = 0;

						if( _tcsstr(sImageString.GetData(), _T("file=\'")) != NULL || _tcsstr(sImageString.GetData(), _T("res=\'")) != NULL ) {
							CDuiString sImageResType;
							CDuiString sImageName;
							LPCTSTR pStrImage = sImageString.GetData();
							CDuiString sItem;
							CDuiString sValue;
							while( *pStrImage != _T('\0') ) {
								sItem.Empty();
								sValue.Empty();
								while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
								while( *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ') ) {
									LPTSTR pstrTemp = ::CharNext(pStrImage);
									while( pStrImage < pstrTemp) {
										sItem += *pStrImage++;
									}
								}
								while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
								if( *pStrImage++ != _T('=') ) break;
								while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
								if( *pStrImage++ != _T('\'') ) break;
								while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) {
									LPTSTR pstrTemp = ::CharNext(pStrImage);
									while( pStrImage < pstrTemp) {
										sValue += *pStrImage++;
									}
								}
								if( *pStrImage++ != _T('\'') ) break;
								if( !sValue.IsEmpty() ) {
									if( sItem == _T("file") || sItem == _T("res") ) {
										sImageName = sValue;
									}
									else if( sItem == _T("restype") ) {
										sImageResType = sValue;
									}
								}
								if( *pStrImage++ != _T(' ') ) break;
							}

							pImageInfo = pManager->GetImageEx((LPCTSTR)sImageName, sImageResType);
						}
						else
							pImageInfo = pManager->GetImageEx((LPCTSTR)sName);

						if( pImageInfo ) {
							iWidth = pImageInfo->nX;
							iHeight = pImageInfo->nY;
							if( iImageListNum > 1 ) iWidth /= iImageListNum;

                            if( pt.x + iWidth > rc.right && pt.x > rc.left && (uStyle & DT_SINGLELINE) == 0 ) {
                                bLineEnd = true;
                            }
                            else {
                                pstrNextStart = NULL;
                                if( bDraw && bLineDraw ) {
                                    CDuiRect rcImage(pt.x, pt.y + cyLineHeight - iHeight, pt.x + iWidth, pt.y + cyLineHeight);
                                    if( iHeight < cyLineHeight ) { 
                                        rcImage.bottom -= (cyLineHeight - iHeight) / 2;
                                        rcImage.top = rcImage.bottom -  iHeight;
                                    }
                                    CDuiRect rcBmpPart(0, 0, iWidth, iHeight);
                                    rcBmpPart.left = iWidth * iImageListIndex;
                                    rcBmpPart.right = iWidth * (iImageListIndex + 1);
                                    CDuiRect rcCorner(0, 0, 0, 0);
                                    DrawImage(hDC, pImageInfo->hBitmap, rcImage, rcImage, rcBmpPart, rcCorner, \
                                        pImageInfo->alphaChannel, 255);
                                }

                                cyLine = MAX(iHeight, cyLine);
                                pt.x += iWidth;
                                cyMinHeight = pt.y + iHeight;
                                cxMaxWidth = MAX(cxMaxWidth, pt.x);
                            }
                        }
                        else pstrNextStart = NULL;
                    }
                }
                break;
            case _T('n'):  // Newline
                {
                    pstrText++;
                    if( (uStyle & DT_SINGLELINE) != 0 ) break;
                    bLineEnd = true;
                }
                break;
            case _T('p'):  // Paragraph
                {
                    pstrText++;
                    if( pt.x > rc.left ) bLineEnd = true;
                    while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                    int cyLineExtra = (int)_tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
                    aPIndentArray.Add((LPVOID)cyLineExtra);
                    cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + cyLineExtra);
                }
                break;
            case _T('r'):  // Raw Text
                {
                    pstrText++;
                    bInRaw = true;
                }
                break;
            case _T('s'):  // Selected text background color
                {
                    pstrText++;
                    bInSelected = !bInSelected;
                    if( bDraw && bLineDraw ) {
                        if( bInSelected ) ::SetBkMode(hDC, OPAQUE);
                        else ::SetBkMode(hDC, TRANSPARENT);
                    }
                }
                break;
            case _T('u'):  // Underline text
                {
                    pstrText++;
                    TFontInfo* pFontInfo = pManager->GetDefaultFontInfo();
                    if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
                    if( pFontInfo->bUnderline == false ) {
                        HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
                        if( hFont == NULL ) hFont = pManager->AddFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
                        pFontInfo = pManager->GetFontInfo(hFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        ::SelectObject(hDC, pFontInfo->hFont);
                        cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                    }
                }
                break;
            case _T('x'):  // X Indent
                {
                    pstrText++;
                    while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                    int iWidth = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
                    pt.x += iWidth;
                    cxMaxWidth = MAX(cxMaxWidth, pt.x);
                }
                break;
            case _T('y'):  // Y Indent
                {
                    pstrText++;
                    while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                    cyLine = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
                }
                break;
                }
                if( pstrNextStart != NULL ) pstrText = pstrNextStart;
                else {
                    while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) pstrText = ::CharNext(pstrText);
                    pstrText = ::CharNext(pstrText);
                }
        }
        else if( !bInRaw && ( *pstrText == _T('<') || *pstrText == _T('{') ) && pstrText[1] == _T('/') )
        {
            pstrText++;
            pstrText++;
            switch( *pstrText )
            {
            case _T('c'):
                {
                    pstrText++;
                    aColorArray.Remove(aColorArray.GetSize() - 1);
                    DWORD clrColor = dwTextColor;
                    if( aColorArray.GetSize() > 0 ) clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
                    ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                }
                break;
            case _T('p'):
                pstrText++;
                if( pt.x > rc.left ) bLineEnd = true;
                aPIndentArray.Remove(aPIndentArray.GetSize() - 1);
                cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                break;
            case _T('s'):
                {
                    pstrText++;
                    bInSelected = !bInSelected;
                    if( bDraw && bLineDraw ) {
                        if( bInSelected ) ::SetBkMode(hDC, OPAQUE);
                        else ::SetBkMode(hDC, TRANSPARENT);
                    }
                }
                break;
            case _T('a'):
                {
                    if( iLinkIndex < nLinkRects ) {
                        if( !bLineDraw ) ::SetRect(&prcLinks[iLinkIndex], ptLinkStart.x, ptLinkStart.y, MIN(pt.x, rc.right), pt.y + pTm->tmHeight + pTm->tmExternalLeading);
                        iLinkIndex++;
                    }
                    aColorArray.Remove(aColorArray.GetSize() - 1);
                    DWORD clrColor = dwTextColor;
                    if( aColorArray.GetSize() > 0 ) clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
                    ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                    bInLink = false;
                }
            case _T('b'):
            case _T('f'):
            case _T('i'):
            case _T('u'):
                {
                    pstrText++;
                    aFontArray.Remove(aFontArray.GetSize() - 1);
                    TFontInfo* pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
                    if( pFontInfo == NULL ) pFontInfo = pManager->GetDefaultFontInfo();
                    if( pTm->tmItalic && pFontInfo->bItalic == false ) {
                        ABC abc;
                        ::GetCharABCWidths(hDC, _T(' '), _T(' '), &abc);
                        pt.x += abc.abcC / 2; // 简单修正一下斜体混排的问题, 正确做法应该是http://support.microsoft.com/kb/244798/en-us
                    }
                    pTm = &pFontInfo->tm;
                    ::SelectObject(hDC, pFontInfo->hFont);
                    cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                }
                break;
            }
            while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) pstrText = ::CharNext(pstrText);
            pstrText = ::CharNext(pstrText);
        }
        else if( !bInRaw &&  *pstrText == _T('<') && pstrText[2] == _T('>') && (pstrText[1] == _T('{')  || pstrText[1] == _T('}')) )
        {
            SIZE szSpace = { 0 };
            ::GetTextExtentPoint32(hDC, &pstrText[1], 1, &szSpace);
            if( bDraw && bLineDraw ) ::TextOut(hDC, pt.x, pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, &pstrText[1], 1);
            pt.x += szSpace.cx;
            cxMaxWidth = MAX(cxMaxWidth, pt.x);
            pstrText++;pstrText++;pstrText++;
        }
        else if( !bInRaw &&  *pstrText == _T('{') && pstrText[2] == _T('}') && (pstrText[1] == _T('<')  || pstrText[1] == _T('>')) )
        {
            SIZE szSpace = { 0 };
            ::GetTextExtentPoint32(hDC, &pstrText[1], 1, &szSpace);
            if( bDraw && bLineDraw ) ::TextOut(hDC, pt.x,  pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, &pstrText[1], 1);
            pt.x += szSpace.cx;
            cxMaxWidth = MAX(cxMaxWidth, pt.x);
            pstrText++;pstrText++;pstrText++;
        }
        else if( !bInRaw &&  *pstrText == _T(' ') )
        {
            SIZE szSpace = { 0 };
            ::GetTextExtentPoint32(hDC, _T(" "), 1, &szSpace);
            // Still need to paint the space because the font might have
            // underline formatting.
            if( bDraw && bLineDraw ) ::TextOut(hDC, pt.x,  pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, _T(" "), 1);
            pt.x += szSpace.cx;
            cxMaxWidth = MAX(cxMaxWidth, pt.x);
            pstrText++;
        }
        else
        {
            POINT ptPos = pt;
            int cchChars = 0;
            int cchSize = 0;
            int cchLastGoodWord = 0;
            int cchLastGoodSize = 0;
            LPCTSTR p = pstrText;
            LPCTSTR pstrNext;
            SIZE szText = { 0 };
            if( !bInRaw && *p == _T('<') || *p == _T('{') ) p++, cchChars++, cchSize++;
            while( *p != _T('\0') && *p != _T('\n') ) {
                // This part makes sure that we're word-wrapping if needed or providing support
                // for DT_END_ELLIPSIS. Unfortunately the GetTextExtentPoint32() call is pretty
                // slow when repeated so often.
                // TODO: Rewrite and use GetTextExtentExPoint() instead!
                if( bInRaw ) {
                    if( ( *p == _T('<') || *p == _T('{') ) && p[1] == _T('/') 
                        && p[2] == _T('r') && ( p[3] == _T('>') || p[3] == _T('}') ) ) {
                            p += 4;
                            bInRaw = false;
                            break;
                    }
                }
                else {
                    if( *p == _T('<') || *p == _T('{') ) break;
                }
                pstrNext = ::CharNext(p);
                cchChars++;
                cchSize += (int)(pstrNext - p);
                szText.cx = cchChars * pTm->tmMaxCharWidth;
                if( pt.x + szText.cx >= rc.right ) {
                    ::GetTextExtentPoint32(hDC, pstrText, cchSize, &szText);
                }
                if( pt.x + szText.cx > rc.right ) {
                    if( pt.x + szText.cx > rc.right && pt.x != rc.left) {
                        cchChars--;
                        cchSize -= (int)(pstrNext - p);
                    }
                    if( (uStyle & DT_WORDBREAK) != 0 && cchLastGoodWord > 0 ) {
                        cchChars = cchLastGoodWord;
                        cchSize = cchLastGoodSize;                 
                    }
                    if( (uStyle & DT_END_ELLIPSIS) != 0 && cchChars > 0 ) {
                        cchChars -= 1;
                        LPCTSTR pstrPrev = ::CharPrev(pstrText, p);
                        if( cchChars > 0 ) {
                            cchChars -= 1;
                            pstrPrev = ::CharPrev(pstrText, pstrPrev);
                            cchSize -= (int)(p - pstrPrev);
                        }
                        else 
                            cchSize -= (int)(p - pstrPrev);
                        pt.x = rc.right;
                    }
                    bLineEnd = true;
                    cxMaxWidth = MAX(cxMaxWidth, pt.x);
                    break;
                }
                if (!( ( p[0] >= _T('a') && p[0] <= _T('z') ) || ( p[0] >= _T('A') && p[0] <= _T('Z') ) )) {
                    cchLastGoodWord = cchChars;
                    cchLastGoodSize = cchSize;
                }
                if( *p == _T(' ') ) {
                    cchLastGoodWord = cchChars;
                    cchLastGoodSize = cchSize;
                }
                p = ::CharNext(p);
            }
            
            ::GetTextExtentPoint32(hDC, pstrText, cchSize, &szText);
            if( bDraw && bLineDraw ) {
				if( (uStyle & DT_SINGLELINE) == 0 && (uStyle & DT_CENTER) != 0 ) {
					ptPos.x += (rc.right - rc.left - szText.cx)/2;
				}
				else if( (uStyle & DT_SINGLELINE) == 0 && (uStyle & DT_RIGHT) != 0) {
					ptPos.x += (rc.right - rc.left - szText.cx);
				}
				::TextOut(hDC, ptPos.x, ptPos.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, pstrText, cchSize);
				if( pt.x >= rc.right && (uStyle & DT_END_ELLIPSIS) != 0 ) 
                    ::TextOut(hDC, ptPos.x + szText.cx, ptPos.y, _T("..."), 3);
            }
            pt.x += szText.cx;
            cxMaxWidth = MAX(cxMaxWidth, pt.x);
            pstrText += cchSize;
        }

        if( pt.x >= rc.right || *pstrText == _T('\n') || *pstrText == _T('\0') ) bLineEnd = true;
        if( bDraw && bLineEnd ) {
            if( !bLineDraw ) {
                aFontArray.Resize(aLineFontArray.GetSize());
                ::CopyMemory(aFontArray.GetData(), aLineFontArray.GetData(), aLineFontArray.GetSize() * sizeof(LPVOID));
                aColorArray.Resize(aLineColorArray.GetSize());
                ::CopyMemory(aColorArray.GetData(), aLineColorArray.GetData(), aLineColorArray.GetSize() * sizeof(LPVOID));
                aPIndentArray.Resize(aLinePIndentArray.GetSize());
                ::CopyMemory(aPIndentArray.GetData(), aLinePIndentArray.GetData(), aLinePIndentArray.GetSize() * sizeof(LPVOID));

                cyLineHeight = cyLine;
                pstrText = pstrLineBegin;
                bInRaw = bLineInRaw;
                bInSelected = bLineInSelected;

                DWORD clrColor = dwTextColor;
                if( aColorArray.GetSize() > 0 ) clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
                ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                TFontInfo* pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
                if( pFontInfo == NULL ) pFontInfo = pManager->GetDefaultFontInfo();
                pTm = &pFontInfo->tm;
                ::SelectObject(hDC, pFontInfo->hFont);
                if( bInSelected ) ::SetBkMode(hDC, OPAQUE);
            }
            else {
                aLineFontArray.Resize(aFontArray.GetSize());
                ::CopyMemory(aLineFontArray.GetData(), aFontArray.GetData(), aFontArray.GetSize() * sizeof(LPVOID));
                aLineColorArray.Resize(aColorArray.GetSize());
                ::CopyMemory(aLineColorArray.GetData(), aColorArray.GetData(), aColorArray.GetSize() * sizeof(LPVOID));
                aLinePIndentArray.Resize(aPIndentArray.GetSize());
                ::CopyMemory(aLinePIndentArray.GetData(), aPIndentArray.GetData(), aPIndentArray.GetSize() * sizeof(LPVOID));
                pstrLineBegin = pstrText;
                bLineInSelected = bInSelected;
                bLineInRaw = bInRaw;
            }
        }

        ASSERT(iLinkIndex<=nLinkRects);
    }

    nLinkRects = iLinkIndex;

    // Return size of text when requested
    if( (uStyle & DT_CALCRECT) != 0 ) {
        rc.bottom = MAX(cyMinHeight, pt.y + cyLine);
        rc.right = MIN(rc.right, cxMaxWidth);
    }

    if( bDraw ) ::SelectClipRgn(hDC, hOldRgn);
    ::DeleteObject(hOldRgn);
    ::DeleteObject(hRgn);

    ::SelectObject(hDC, hOldFont);
}

HBITMAP CRenderEngine::GenerateBitmap(CPaintManagerUI* pManager, CControlUI* pControl, RECT rc)
{
    int cx = rc.right - rc.left;
    int cy = rc.bottom - rc.top;

    HDC hPaintDC = ::CreateCompatibleDC(pManager->GetPaintDC());
    HBITMAP hPaintBitmap = ::CreateCompatibleBitmap(pManager->GetPaintDC(), rc.right, rc.bottom);
    ASSERT(hPaintDC);
    ASSERT(hPaintBitmap);
    HBITMAP hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
    pControl->DoPaint(hPaintDC, rc);


    LPDWORD pDest = NULL;
    HDC hCloneDC = ::CreateCompatibleDC(pManager->GetPaintDC());
	HBITMAP hBitmap = CRenderEngine::CreateBitmap(pManager->GetPaintDC(), cx, cy, (BYTE**)&pDest);
    ASSERT(hCloneDC);
    ASSERT(hBitmap);

    if( hBitmap != NULL )
    {
        HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
        ::BitBlt(hCloneDC, 0, 0, cx, cy, hPaintDC, rc.left, rc.top, SRCCOPY);
        ::SelectObject(hCloneDC, hOldBitmap);
        ::DeleteDC(hCloneDC);  
        ::GdiFlush();
    }

    // Cleanup
    ::SelectObject(hPaintDC, hOldPaintBitmap);
    ::DeleteObject(hPaintBitmap);
    ::DeleteDC(hPaintDC);

    return hBitmap;
}

SIZE CRenderEngine::GetTextSize( HDC hDC, CPaintManagerUI* pManager , LPCTSTR pstrText, int iFont, UINT uStyle )
{
	SIZE size = {0,0};
	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	if( pstrText == NULL || pManager == NULL ) return size;
	::SetBkMode(hDC, TRANSPARENT);
	HFONT hOldFont = (HFONT)::SelectObject(hDC, pManager->GetFont(iFont));
	GetTextExtentPoint32(hDC, pstrText, _tcslen(pstrText) , &size);
	::SelectObject(hDC, hOldFont);
	return size;
}

void CRenderEngine::RestoreAlphaColor(LPBYTE pBits, int bitsWidth, PRECT rc)
{
	for (int i = rc->top; i < rc->bottom; ++i)
	{
		for (int j = rc->left; j < rc->right; ++j)
		{
			int x = (i*bitsWidth + j) * 4;
			if ((pBits[x + 3] == 0) && (pBits[x + 0] != 0 || pBits[x + 1] != 0 || pBits[x + 2] != 0))
				pBits[x + 3] = 255;
		}
	}
}

} // namespace DuiLib
