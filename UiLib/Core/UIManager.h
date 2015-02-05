#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__

#pragma once
#include <gdiplus.h>

namespace UiLib {
/////////////////////////////////////////////////////////////////////////////////////
//
#define WM_EFFECTS		WM_USER+1680
#define WM_RELOADSTYLE	WM_USER+1681

class CControlUI;
class CRichEditUI;

/////////////////////////////////////////////////////////////////////////////////////
//
static UINT MapKeyState()
{
	UINT uState = 0;
	if( ::GetKeyState(VK_CONTROL) < 0 ) uState |= MK_CONTROL;
	if( ::GetKeyState(VK_RBUTTON) < 0 ) uState |= MK_LBUTTON;
	if( ::GetKeyState(VK_LBUTTON) < 0 ) uState |= MK_RBUTTON;
	if( ::GetKeyState(VK_SHIFT) < 0 ) uState |= MK_SHIFT;
	if( ::GetKeyState(VK_MENU) < 0 ) uState |= MK_ALT;
	return uState;
}

typedef enum EVENTTYPE_UI
{
	UIEVENT__ALL,
    UIEVENT__FIRST,
    UIEVENT__KEYBEGIN,
    UIEVENT_KEYDOWN,
    UIEVENT_KEYUP,
    UIEVENT_CHAR,
    UIEVENT_SYSKEY,
    UIEVENT__KEYEND,
    UIEVENT__MOUSEBEGIN,
    UIEVENT_MOUSEMOVE,
    UIEVENT_MOUSELEAVE,
    UIEVENT_MOUSEENTER,
    UIEVENT_MOUSEHOVER,
    UIEVENT_BUTTONDOWN,
    UIEVENT_BUTTONUP,
    UIEVENT_RBUTTONDOWN,
    UIEVENT_DBLCLICK,
    UIEVENT_CONTEXTMENU,
    UIEVENT_SCROLLWHEEL,
    UIEVENT__MOUSEEND,
    UIEVENT_KILLFOCUS,
    UIEVENT_SETFOCUS,
    UIEVENT_WINDOWSIZE,
    UIEVENT_SETCURSOR,
    UIEVENT_TIMER,
    UIEVENT_NOTIFY,
	UIEVENT_COMMAND,
	UIEVENT_RELOADSTYLE,
	UIEVENT_TRAYICON,
    UIEVENT__LAST,
};

typedef enum
{
	UIFONT__FIRST = 0,
	UIFONT_NORMAL,
	UIFONT_BOLD,
	UIFONT_CAPTION,
	UIFONT_MENU,
	UIFONT_LINK,
	UIFONT_TITLE,
	UIFONT_HEADLINE,
	UIFONT_SUBSCRIPT,
	UIFONT__LAST,
} UITYPE_FONT;

typedef enum
{
	UICOLOR__FIRST = 0,
	UICOLOR_WINDOW_BACKGROUND,
	UICOLOR_WINDOW_TEXT,
	UICOLOR_DIALOG_BACKGROUND,
	UICOLOR_DIALOG_TEXT_NORMAL,
	UICOLOR_DIALOG_TEXT_DARK,
	UICOLOR_MENU_BACKGROUND,
	UICOLOR_MENU_TEXT_NORMAL,
	UICOLOR_MENU_TEXT_HOVER,
	UICOLOR_MENU_TEXT_SELECTED,
	UICOLOR_TEXTCOLOR_NORMAL,
	UICOLOR_TEXTCOLOR_HOVER,
	UICOLOR_TAB_BACKGROUND_NORMAL,
	UICOLOR_TAB_BACKGROUND_SELECTED,
	UICOLOR_TAB_FOLDER_NORMAL,
	UICOLOR_TAB_FOLDER_SELECTED,
	UICOLOR_TAB_BORDER,
	UICOLOR_TAB_TEXT_NORMAL,
	UICOLOR_TAB_TEXT_SELECTED,
	UICOLOR_TAB_TEXT_DISABLED,
	UICOLOR_NAVIGATOR_BACKGROUND,
	UICOLOR_NAVIGATOR_BUTTON_HOVER,
	UICOLOR_NAVIGATOR_BUTTON_PUSHED,
	UICOLOR_NAVIGATOR_BUTTON_SELECTED,
	UICOLOR_NAVIGATOR_BORDER_NORMAL,
	UICOLOR_NAVIGATOR_BORDER_SELECTED,
	UICOLOR_NAVIGATOR_TEXT_NORMAL,
	UICOLOR_NAVIGATOR_TEXT_SELECTED,
	UICOLOR_NAVIGATOR_TEXT_PUSHED,
	UICOLOR_BUTTON_BACKGROUND_NORMAL,
	UICOLOR_BUTTON_BACKGROUND_DISABLED,
	UICOLOR_BUTTON_BACKGROUND_PUSHED,
	UICOLOR_BUTTON_TEXT_NORMAL,
	UICOLOR_BUTTON_TEXT_PUSHED,
	UICOLOR_BUTTON_TEXT_DISABLED,
	UICOLOR_BUTTON_BORDER_LIGHT,
	UICOLOR_BUTTON_BORDER_DARK,
	UICOLOR_BUTTON_BORDER_DISABLED,
	UICOLOR_BUTTON_BORDER_FOCUS,
	UICOLOR_CONTROL_BACKGROUND_NORMAL,
	UICOLOR_CONTROL_BACKGROUND_SELECTED,
	UICOLOR_CONTROL_BACKGROUND_DISABLED,
	UICOLOR_CONTROL_BACKGROUND_READONLY,
	UICOLOR_CONTROL_BACKGROUND_HOVER,
	UICOLOR_CONTROL_BACKGROUND_SORTED,
	UICOLOR_CONTROL_BACKGROUND_EXPANDED,
	UICOLOR_CONTROL_BORDER_NORMAL,
	UICOLOR_CONTROL_BORDER_SELECTED,
	UICOLOR_CONTROL_BORDER_DISABLED,
	UICOLOR_CONTROL_TEXT_NORMAL,
	UICOLOR_CONTROL_TEXT_SELECTED,
	UICOLOR_CONTROL_TEXT_DISABLED,
	UICOLOR_CONTROL_TEXT_READONLY,
	UICOLOR_TOOL_BACKGROUND_NORMAL,
	UICOLOR_TOOL_BACKGROUND_DISABLED,
	UICOLOR_TOOL_BACKGROUND_HOVER,
	UICOLOR_TOOL_BACKGROUND_PUSHED,
	UICOLOR_TOOL_BORDER_NORMAL,
	UICOLOR_TOOL_BORDER_DISABLED,
	UICOLOR_TOOL_BORDER_HOVER,
	UICOLOR_TOOL_BORDER_PUSHED,
	UICOLOR_EDIT_BACKGROUND_NORMAL,
	UICOLOR_EDIT_BACKGROUND_HOVER,
	UICOLOR_EDIT_BACKGROUND_DISABLED,
	UICOLOR_EDIT_BACKGROUND_READONLY,
	UICOLOR_EDIT_TEXT_NORMAL,
	UICOLOR_EDIT_TEXT_DISABLED,
	UICOLOR_EDIT_TEXT_READONLY,
	UICOLOR_TITLE_BACKGROUND,
	UICOLOR_TITLE_TEXT,
	UICOLOR_TITLE_BORDER_LIGHT,
	UICOLOR_TITLE_BORDER_DARK,
	UICOLOR_HEADER_BACKGROUND,
	UICOLOR_HEADER_BORDER,
	UICOLOR_HEADER_SEPARATOR,
	UICOLOR_HEADER_TEXT,
	UICOLOR_TASK_BACKGROUND,
	UICOLOR_TASK_CAPTION,
	UICOLOR_TASK_BORDER,
	UICOLOR_TASK_TEXT,
	UICOLOR_LINK_TEXT_HOVER,
	UICOLOR_LINK_TEXT_NORMAL,
	UICOLOR_STANDARD_BLACK,
	UICOLOR_STANDARD_YELLOW,
	UICOLOR_STANDARD_RED,
	UICOLOR_STANDARD_GREY,
	UICOLOR_STANDARD_LIGHTGREY,
	UICOLOR_STANDARD_WHITE,
	UICOLOR__LAST,
	UICOLOR__INVALID,
} UITYPE_COLOR;
/////////////////////////////////////////////////////////////////////////////////////
//

// Flags for CControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_TOP_FIRST     0x00000008
#define UIFIND_ME_FIRST      0x80000000

// Flags for the CDialogLayout stretching
#define UISTRETCH_NEWGROUP   0x00000001
#define UISTRETCH_NEWLINE    0x00000002
#define UISTRETCH_MOVE_X     0x00000004
#define UISTRETCH_MOVE_Y     0x00000008
#define UISTRETCH_SIZE_X     0x00000010
#define UISTRETCH_SIZE_Y     0x00000020

// Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_READONLY     0x00000020
#define UISTATE_CAPTURED     0x00000040



/////////////////////////////////////////////////////////////////////////////////////
//
typedef struct tagEffectAge
{
	bool		m_bEnableEffect;
	int			m_iZoom;
	DWORD		m_dFillingBK;
	int			m_iOffectX;
	int			m_iOffectY;
	int			m_iAlpha;
	float		m_fRotation;
	int			m_iNeedTimer;
}TEffectAge;

typedef struct tagTProperty
{
	union unValue
	{
		RECT		rcValue;
		INT			iValue;
		UINT		uValue;
		LONG		lValue;
		DWORD		dwValue;
		SIZE		siValue;
	};
	
	UINT			uInterval;
	UINT			uTimer;
	UINT			uDelay;
	bool			bLoop;
	bool			bReverse;
	bool			bAutoStart;
	CDuiString		sImageLeft;
	CDuiString		sImageRight;
	CDuiString		sName;
	CDuiString		sType;
	CDuiString		sStartValue;
	CDuiString		sEndValue;
	unValue			nStartValue;
	unValue			nEndValue;
	unValue			nDiffValue;

	INT GetDiffINT(INT _StartValue,INT _EndValue,int _CurFrame){
		if(_CurFrame == 1 && nDiffValue.iValue != _StartValue)
			nDiffValue.iValue = max(_StartValue,_EndValue) - min(_StartValue,_EndValue);
		
		return nDiffValue.iValue;
	}

	INT CalDiffInt(INT _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		if(_CurFrame == 1 && _IsStartNone)
			nStartValue.iValue = _SrcValue;
		if(_CurFrame == 1 && _IsEndNone)
			nEndValue.iValue = _SrcValue;

		int iPartValue	= GetDiffINT(nStartValue.iValue,nEndValue.iValue,_CurFrame) / _TotalFrame;
		int iCurValue;
		if(_CurFrame == _TotalFrame)
			iCurValue = nEndValue.iValue;
		else
			iCurValue = nStartValue.iValue < nEndValue.iValue?(_CurFrame * iPartValue + nStartValue.iValue):(nStartValue.iValue - _CurFrame * iPartValue);
		
		return iCurValue;
	}

	UINT GetDiffUINT(UINT _StartValue,UINT _EndValue,int _CurFrame){
		if(_CurFrame == 1 && nDiffValue.iValue != _StartValue)
			nDiffValue.uValue = max(_StartValue,_EndValue) - min(_StartValue,_EndValue);

		return nDiffValue.uValue;
	}

	UINT CalDiffUint(UINT _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		if(_CurFrame == 1 && _IsStartNone)
			nStartValue.uValue = _SrcValue;
		if(_CurFrame == 1 && _IsEndNone)
			nEndValue.uValue = _SrcValue;

		int iPartValue	= GetDiffUINT(nStartValue.uValue,nEndValue.uValue,_CurFrame) / _TotalFrame;
		int iCurValue;
		if(_CurFrame == _TotalFrame)
			iCurValue = nEndValue.iValue;
		else
			iCurValue = nStartValue.iValue < nEndValue.iValue?(_CurFrame * iPartValue + nStartValue.iValue):(nStartValue.iValue - _CurFrame * iPartValue);

		return iCurValue;
	}

	LONG GetDiffLONG(LONG _StartValue,LONG _EndValue,int _CurFrame){
		if(_CurFrame == 1 && nDiffValue.iValue != _StartValue)
			nDiffValue.lValue = max(_StartValue,_EndValue) - min(_StartValue,_EndValue);

		return nDiffValue.lValue;
	}

	LONG CalDiffLong(LONG _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		if(_CurFrame == 1 && _IsStartNone)
		nStartValue.lValue = _SrcValue;
		if(_CurFrame == 1 && _IsEndNone)
			nEndValue.lValue = _SrcValue;

		int iPartValue	= GetDiffLONG(nStartValue.lValue,nEndValue.lValue,_CurFrame) / _TotalFrame;
		int iCurValue;
		if(_CurFrame == _TotalFrame)
			iCurValue = nEndValue.iValue;
		else
			iCurValue = nStartValue.iValue < nEndValue.iValue?(_CurFrame * iPartValue + nStartValue.iValue):(nStartValue.iValue - _CurFrame * iPartValue);

		return iCurValue;
	}

	RECT GetDiffRect(RECT _StartValue,RECT _EndValue,int _CurFrame){
		if(_CurFrame == 1 && (nDiffValue.rcValue.left != _StartValue.left || nDiffValue.rcValue.right != _StartValue.right || nDiffValue.rcValue.top != _StartValue.top || nDiffValue.rcValue.bottom != _StartValue.bottom)){
			nDiffValue.rcValue.left		= max(_StartValue.left,_EndValue.left) - min(_StartValue.left,_EndValue.left);
			nDiffValue.rcValue.right	= max(_StartValue.right,_EndValue.right) - min(_StartValue.right,_EndValue.right);
			nDiffValue.rcValue.top		= max(_StartValue.top,_EndValue.top) - min(_StartValue.top,_EndValue.top);
			nDiffValue.rcValue.bottom	= max(_StartValue.bottom,_EndValue.bottom) - min(_StartValue.bottom,_EndValue.bottom);
		}

		return nDiffValue.rcValue;
	}

	RECT CalDiffRect(RECT _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		if(_CurFrame == 1 && _IsStartNone)
			nStartValue.rcValue = _SrcValue;
		if(_CurFrame == 1 && _IsEndNone)
			nEndValue.rcValue = _SrcValue;

		RECT nDiffRect	= GetDiffRect(nStartValue.rcValue,nEndValue.rcValue,_CurFrame);
		RECT nCurRect;
		if(_CurFrame == _TotalFrame){
			nCurRect	= nEndValue.rcValue;
		}
		else{
			nCurRect.left	= nStartValue.iValue < nEndValue.iValue?(nDiffRect.left / _TotalFrame * _CurFrame + nStartValue.rcValue.left):(nStartValue.rcValue.left - nDiffRect.left / _TotalFrame * _CurFrame);
			nCurRect.right	= nStartValue.iValue < nEndValue.iValue?(nDiffRect.right / _TotalFrame * _CurFrame + nStartValue.rcValue.right):(nStartValue.rcValue.right - nDiffRect.right / _TotalFrame * _CurFrame);
			nCurRect.top	= nStartValue.iValue < nEndValue.iValue?(nDiffRect.top / _TotalFrame * _CurFrame + nStartValue.rcValue.top):(nStartValue.rcValue.top - nDiffRect.top / _TotalFrame * _CurFrame);
			nCurRect.bottom	= nStartValue.iValue < nEndValue.iValue?(nDiffRect.bottom / _TotalFrame * _CurFrame + nStartValue.rcValue.bottom):(nStartValue.rcValue.bottom - nDiffRect.bottom / _TotalFrame * _CurFrame);
		}
		return nCurRect;
	}

	DWORD GetDiffColor(DWORD _StartValue,DWORD _EndValue,int _CurFrame){
		if(_CurFrame == 1 && nDiffValue.dwValue != _StartValue){
			Gdiplus::Color nStartColor(_StartValue);
			Gdiplus::Color nEndColor(_EndValue);

			int iDiffValueA = max(nEndColor.GetA(),nStartColor.GetA()) - min(nEndColor.GetA(),nStartColor.GetA());
			int iDiffValueR = max(nEndColor.GetR(),nStartColor.GetR()) - min(nEndColor.GetR(),nStartColor.GetR());
			int iDiffValueG = max(nEndColor.GetG(),nStartColor.GetG()) - min(nEndColor.GetG(),nStartColor.GetG());
			int iDiffValueB = max(nEndColor.GetB(),nStartColor.GetB()) - min(nEndColor.GetB(),nStartColor.GetB());

			Gdiplus::Color nRetColor(iDiffValueA,iDiffValueR,iDiffValueG,iDiffValueB);

			nDiffValue.dwValue = nRetColor.GetValue();
		}
		return nDiffValue.dwValue;
	}

	DWORD CalCurColor(DWORD _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		if(_CurFrame == 1 && _IsStartNone)
		nStartValue.dwValue = _SrcValue;
		if(_CurFrame == 1 && _IsEndNone)
			nEndValue.dwValue = _SrcValue;

		Gdiplus::Color nStartColor(nStartValue.dwValue);
		Gdiplus::Color nDiffColor(GetDiffColor(nStartValue.dwValue,nEndValue.dwValue,_CurFrame));
		DWORD nCurColor;
		if(_CurFrame == _TotalFrame){
			nCurColor = Gdiplus::Color(nEndValue.dwValue).GetValue();
		}
		else{
			int iPartValueA = nStartValue.iValue < nEndValue.iValue?(nDiffColor.GetA() / _TotalFrame * _CurFrame + nStartColor.GetA()):(nStartColor.GetA() - nDiffColor.GetA() / _TotalFrame * _CurFrame);
			int iPartValueR = nStartValue.iValue < nEndValue.iValue?(nDiffColor.GetR() / _TotalFrame * _CurFrame + nStartColor.GetR()):(nStartColor.GetR() - nDiffColor.GetR() / _TotalFrame * _CurFrame);
			int iPartValueG = nStartValue.iValue < nEndValue.iValue?(nDiffColor.GetG() / _TotalFrame * _CurFrame + nStartColor.GetG()):(nStartColor.GetG() - nDiffColor.GetG() / _TotalFrame * _CurFrame);
			int iPartValueB = nStartValue.iValue < nEndValue.iValue?(nDiffColor.GetB() / _TotalFrame * _CurFrame + nStartColor.GetB()):(nStartColor.GetB() - nDiffColor.GetB() / _TotalFrame * _CurFrame);
			nCurColor = Gdiplus::Color(iPartValueA,iPartValueR,iPartValueG,iPartValueB).GetValue();
		}
		return nCurColor;
	}

	SIZE GetDiffSize(SIZE _StartValue,SIZE _EndValue,int _CurFrame){
		if(_CurFrame == 1 && (nDiffValue.siValue.cx != _StartValue.cx || nDiffValue.siValue.cy != _StartValue.cy)){
			nDiffValue.siValue.cx	= max(_StartValue.cx,_EndValue.cx) - min(_StartValue.cx,_EndValue.cx);
			nDiffValue.siValue.cy	= max(_StartValue.cy,_EndValue.cy) - min(_StartValue.cy,_EndValue.cy);
		}

		return nDiffValue.siValue;
	}

	SIZE CalCurSize(SIZE _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		if(_CurFrame == 1 && _IsStartNone)
			nStartValue.siValue = _SrcValue;
		if(_CurFrame == 1 && _IsEndNone)
			nEndValue.siValue = _SrcValue;

		SIZE nDiffSize = GetDiffSize(nStartValue.siValue,nEndValue.siValue,_CurFrame);
		SIZE nCurSize;
		if(_CurFrame == _TotalFrame){
			nCurSize = nEndValue.siValue;
		}
		else{
			nCurSize.cx	= nStartValue.iValue < nEndValue.iValue?(nDiffSize.cx / _TotalFrame * _CurFrame + nDiffSize.cx):(nDiffSize.cx - nDiffSize.cx / _TotalFrame * _CurFrame);
			nCurSize.cy	= nStartValue.iValue < nEndValue.iValue?(nDiffSize.cy / _TotalFrame * _CurFrame + nDiffSize.cy):(nDiffSize.cy - nDiffSize.cy / _TotalFrame * _CurFrame);
		}

		return nCurSize;
	}
	
	CDuiString CalCurImageSource(LPCTSTR _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		CDuiString nSrcValue = _SrcValue;

		int nLeft = nSrcValue.Find(_T("source='"));
		if(sImageLeft.IsEmpty())
			sImageLeft = nSrcValue.Left(nLeft + 8);
		int nRight = nSrcValue.Find(_T("'"),nLeft + 8);
		CDuiString nValueStr = nSrcValue.Mid(nLeft+8,nRight-nLeft-8);
		if(sImageRight.IsEmpty())
			sImageRight = nSrcValue.Mid(nRight);

		LPTSTR pStartStr = NULL;
		RECT rcSrcValue;
		rcSrcValue.left		= _tcstol(nValueStr.GetData(), &pStartStr, 10);		ASSERT(pStartStr);
		rcSrcValue.top		= _tcstol(pStartStr + 1, &pStartStr, 10);	ASSERT(pStartStr);
		rcSrcValue.right	= _tcstol(pStartStr + 1, &pStartStr, 10);	ASSERT(pStartStr);
		rcSrcValue.bottom	= _tcstol(pStartStr + 1, &pStartStr, 10);	ASSERT(pStartStr);

		RECT nCurRect = CalDiffRect(rcSrcValue,_TotalFrame,_CurFrame,_IsStartNone,_IsEndNone);

		CDuiString nNewImageValue;
		nNewImageValue.Format(_T("%s%d,%d,%d,%d%s"),sImageLeft.GetData(),nCurRect.left,nCurRect.top,nCurRect.right,nCurRect.bottom,sImageRight.GetData());

		DUITRACE(_T("%s"),nNewImageValue.GetData());
		return nNewImageValue.GetData();
	}
	CDuiString CalCurImageMask(LPCTSTR _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		CDuiString nSrcValue = _SrcValue;

		int nLeft = nSrcValue.Find(_T("make='"));
		if(sImageLeft.IsEmpty())
			sImageLeft = nSrcValue.Left(nLeft + 6);
		int nRight = nSrcValue.Find(_T("'"),nLeft + 6);
		int nValue = _ttoi(nSrcValue.Mid(nLeft+6,nRight-nLeft-6));
		if(sImageRight.IsEmpty())
			sImageRight = nSrcValue.Mid(nRight);

		CDuiString nNewImageValue;
		nNewImageValue.Format(_T("%s%d%s"),sImageLeft.GetData(),CalDiffInt(nValue,_TotalFrame,_CurFrame,_IsStartNone,_IsEndNone),sImageRight.GetData());
		DUITRACE(_T("%s"),nNewImageValue.GetData());
		return nNewImageValue.GetData();
	}
	CDuiString CalCurImageCorner(LPCTSTR _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		CDuiString nSrcValue = _SrcValue;

		int nLeft = nSrcValue.Find(_T("corner='"));
		if(sImageLeft.IsEmpty())
			sImageLeft = nSrcValue.Left(nLeft + 8);
		int nRight = nSrcValue.Find(_T("'"),nLeft + 8);
		CDuiString nValueStr = nSrcValue.Mid(nLeft+8,nRight-nLeft-8);
		if(sImageRight.IsEmpty())
			sImageRight = nSrcValue.Mid(nRight);

		LPTSTR pStartStr = NULL;
		RECT rcSrcValue;
		rcSrcValue.left		= _tcstol(nValueStr.GetData(), &pStartStr, 10);		ASSERT(pStartStr);
		rcSrcValue.top		= _tcstol(pStartStr + 1, &pStartStr, 10);	ASSERT(pStartStr);
		rcSrcValue.right	= _tcstol(pStartStr + 1, &pStartStr, 10);	ASSERT(pStartStr);
		rcSrcValue.bottom	= _tcstol(pStartStr + 1, &pStartStr, 10);	ASSERT(pStartStr); 

		RECT nCurRect = CalDiffRect(rcSrcValue,_TotalFrame,_CurFrame,_IsStartNone,_IsEndNone);

		CDuiString nNewImageValue;
		nNewImageValue.Format(_T("%s%d,%d,%d,%d%s"),sImageLeft.GetData(),nCurRect.left,nCurRect.top,nCurRect.right,nCurRect.bottom,sImageRight.GetData());
		DUITRACE(_T("%s"),nNewImageValue.GetData());
		return nNewImageValue.GetData();
	}
	CDuiString CalCurImageFade(LPCTSTR _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		CDuiString nSrcValue = _SrcValue;

		int nLeft = nSrcValue.Find(_T("fade='"));
		if(sImageLeft.IsEmpty())
			sImageLeft = nSrcValue.Left(nLeft + 6);
		int nRight = nSrcValue.Find(_T("'"),nLeft + 6);
		int nValue = _ttoi(nSrcValue.Mid(nLeft+6,nRight-nLeft-6));
		if(sImageRight.IsEmpty())
			sImageRight = nSrcValue.Mid(nRight);

		CDuiString nNewImageValue;
		nNewImageValue.Format(_T("%s%d%s"),sImageLeft.GetData(),CalDiffInt(nValue,_TotalFrame,_CurFrame,_IsStartNone,_IsEndNone),sImageRight.GetData());
		DUITRACE(_T("%s"),nNewImageValue.GetData());
		return nNewImageValue.GetData();
	}
	CDuiString CalCurImageDest(LPCTSTR _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		CDuiString nSrcValue = _SrcValue;

		int nLeft = nSrcValue.Find(_T("dest='"));
		if(sImageLeft.IsEmpty())
			sImageLeft = nSrcValue.Left(nLeft + 6);
		int nRight = nSrcValue.Find(_T("'"),nLeft + 6);
		CDuiString nValueStr = nSrcValue.Mid(nLeft+6,nRight-nLeft-6);
		if(sImageRight.IsEmpty())
			sImageRight = nSrcValue.Mid(nRight);

		LPTSTR pStartStr = NULL;
		RECT rcSrcValue;
		rcSrcValue.left		= _tcstol(nValueStr.GetData(), &pStartStr, 10);		ASSERT(pStartStr);
		rcSrcValue.top		= _tcstol(pStartStr + 1, &pStartStr, 10);	ASSERT(pStartStr);
		rcSrcValue.right	= _tcstol(pStartStr + 1, &pStartStr, 10);	ASSERT(pStartStr);
		rcSrcValue.bottom	= _tcstol(pStartStr + 1, &pStartStr, 10);	ASSERT(pStartStr); 

		RECT nCurRect = CalDiffRect(rcSrcValue,_TotalFrame,_CurFrame,_IsStartNone,_IsEndNone);

		CDuiString nNewImageValue;
		nNewImageValue.Format(_T("%s%d,%d,%d,%d%s"),sImageLeft.GetData(),nCurRect.left,nCurRect.top,nCurRect.right,nCurRect.bottom,sImageRight.GetData());
		DUITRACE(_T("%s"),nNewImageValue.GetData());
		return nNewImageValue.GetData();
	}

	bool IsStartNull(){
		return _tcscmp(_T("none"),sStartValue.GetData()) == 0;
	}

	bool IsEndNull(){
		return _tcscmp(_T("none"),sEndValue.GetData()) == 0;
	}

	tagTProperty():uInterval(0),uDelay(0),uTimer(0),bLoop(false),bReverse(false){
		::ZeroMemory((void*)&nStartValue, sizeof(unValue));
		::ZeroMemory((void*)&nEndValue, sizeof(unValue));
	}

}TProperty;

typedef struct tagTActionGroup
{
	UINT		uDefaultInterval;
	UINT		uDefaultTimer;
	bool		bDefaultLoop;
	bool		bDefaultAutoStart;
	bool		bDefaultReverse;
	INT			iEventValue;
	CDuiString	sMsgValue;
	CDuiString	sMsgType;
	CDuiString	sName;
	TStdPtrArray<TProperty*> mPropertys;

	tagTActionGroup(void){
		uDefaultInterval	= 0;
		bDefaultLoop		= false;
		bDefaultReverse		= false;
		iEventValue			= 0;
	}

	~tagTActionGroup(void){
		for(int i = 0;i < mPropertys.GetSize();i++){
			TProperty* pTp = mPropertys.GetAt(i);
			if(!pTp)
				continue;

			delete pTp;
			pTp = NULL;
			mPropertys.Remove(i);
		}
		mPropertys.Empty();
	}
}TAGroup;

typedef struct tagTFontInfo
{
    HFONT hFont;
    CDuiString sFontName;
    int iSize;
    bool bBold;
    bool bUnderline;
    bool bItalic;
    TEXTMETRIC tm;
} TFontInfo;

typedef struct tagTImageInfo
{
    HBITMAP hBitmap;
    int nX;
    int nY;
    bool alphaChannel;
    CDuiString sResType;
    DWORD dwMask;
} TImageInfo;

// Structure for notifications from the system
// to the control implementation.
typedef struct tagTEventUI
{
    int Type;
    CControlUI* pSender;
    DWORD dwTimestamp;
    POINT ptMouse;
    TCHAR chKey;
    WORD wKeyState;
    WPARAM wParam;
    LPARAM lParam;
} TEventUI;

// Structure for relative position to the parent
typedef struct tagTRelativePosUI
{
	bool bRelative;
	SIZE szParent;
	int nMoveXPercent;
	int nMoveYPercent;
	int nZoomXPercent;
	int nZoomYPercent;
}TRelativePosUI;

// Listener interface
class INotifyUI
{
public:
    virtual void Notify(TNotifyUI& msg) = 0;
};

// MessageFilter interface
class IMessageFilterUI
{
public:
    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};

class ITranslateAccelerator
{
public:
	virtual LRESULT TranslateAccelerator(MSG *pMsg) = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//
typedef CControlUI* (*LPCREATECONTROL)(LPCTSTR pstrType);


class UILIB_API CPaintManagerUI
{
public:
    CPaintManagerUI();
    ~CPaintManagerUI();

public:
    void Init(HWND hWnd);
    void NeedUpdate();
    void Invalidate(RECT& rcItem);

    HDC GetPaintDC() const;
    HWND GetPaintWindow() const;
    HWND GetTooltipWindow() const;

    POINT GetMousePos() const;
    SIZE GetClientSize() const;
    SIZE GetInitSize();
    void SetInitSize(int cx, int cy);
    RECT& GetSizeBox();
    void SetSizeBox(RECT& rcSizeBox);
    RECT& GetCaptionRect();
    void SetCaptionRect(RECT& rcCaption);
    SIZE GetRoundCorner() const;
    void SetRoundCorner(int cx, int cy);
    SIZE GetMinInfo() const;
    void SetMinInfo(int cx, int cy);
    SIZE GetMaxInfo() const;
    void SetMaxInfo(int cx, int cy);
	int GetTransparent() const;
    void SetTransparent(int nOpacity);
    void SetBackgroundTransparent(bool bTrans);
    bool IsShowUpdateRect() const;
    void SetShowUpdateRect(bool show);
	//redrain
	bool IsBackgroundTransparent();
	bool ShowCaret(bool bShow);
	bool SetCaretPos(CRichEditUI* obj, int x, int y);
	CRichEditUI* GetCurrentCaretObject();
	bool CreateCaret(HBITMAP hBmp, int nWidth, int nHeight);
	void DrawCaret(HDC hDC, const RECT& rcPaint);
	CShadowUI* GetShadow();
	void SetUseGdiplusText(bool bUse);
	bool IsUseGdiplusText() const;

    static HINSTANCE GetInstance();
    static CDuiString GetInstancePath();
    static CDuiString GetCurrentPath();
    static HINSTANCE GetResourceDll();
    static const CDuiString& GetResourcePath();
    static const CDuiString& GetResourceZip();
    static bool IsCachedResourceZip();
    static HANDLE GetResourceZipHandle();
    static void SetInstance(HINSTANCE hInst);
    static void SetCurrentPath(LPCTSTR pStrPath);
    static void SetResourceDll(HINSTANCE hInst);
    static void SetResourcePath(LPCTSTR pStrPath);
	static void SetResourceZip(LPVOID pVoid, unsigned int len);
    static void SetResourceZip(LPCTSTR pstrZip, bool bCachedResourceZip = false);
    static void GetHSL(short* H, short* S, short* L);
    static void SetHSL(bool bUseHSL, short H, short S, short L); // H:0~360, S:0~200, L:0~200 
    static void ReloadSkin();
    static bool LoadPlugin(LPCTSTR pstrModuleName);
    static CStdPtrArray* GetPlugins();

    bool UseParentResource(CPaintManagerUI* pm);
    CPaintManagerUI* GetParentResource() const;

    DWORD GetDefaultDisabledColor() const;
    void SetDefaultDisabledColor(DWORD dwColor);
    DWORD GetDefaultFontColor() const;
    void SetDefaultFontColor(DWORD dwColor);
    DWORD GetDefaultLinkFontColor() const;
    void SetDefaultLinkFontColor(DWORD dwColor);
    DWORD GetDefaultLinkHoverFontColor() const;
    void SetDefaultLinkHoverFontColor(DWORD dwColor);
    DWORD GetDefaultSelectedBkColor() const;
    void SetDefaultSelectedBkColor(DWORD dwColor);
    TFontInfo* GetDefaultFontInfo();
    void SetDefaultFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    DWORD GetCustomFontCount() const;
    HFONT AddFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    HFONT AddFontAt(int index, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    HFONT GetFont(int index);
    HFONT GetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    bool FindFont(HFONT hFont);
    bool FindFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    int GetFontIndex(HFONT hFont);
    int GetFontIndex(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    bool RemoveFont(HFONT hFont);
    bool RemoveFontAt(int index);
    void RemoveAllFonts();
    TFontInfo* GetFontInfo(int index);
    TFontInfo* GetFontInfo(HFONT hFont);

    const TImageInfo* GetImage(LPCTSTR bitmap);
    const TImageInfo* GetImageEx(LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0);
    const TImageInfo* AddImage(LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0);
    const TImageInfo* AddImage(LPCTSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha);
    bool RemoveImage(LPCTSTR bitmap);
    void RemoveAllImages();
    void ReloadAllImages();

    void AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList);
    LPCTSTR GetDefaultAttributeList(LPCTSTR pStrControlName) const;
    bool RemoveDefaultAttributeList(LPCTSTR pStrControlName);
    const CStdStringPtrMap& GetDefaultAttribultes() const;
    void RemoveAllDefaultAttributeList();

    bool AttachDialog(CControlUI* pControl);
    bool InitControls(CControlUI* pControl, CControlUI* pParent = NULL);
    void ReapObjects(CControlUI* pControl);

    bool AddOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
    CStdPtrArray* GetOptionGroup(LPCTSTR pStrGroupName);
    void RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
    void RemoveAllOptionGroups();

	bool AddEffectsStyle(LPCTSTR pStrStyleName,LPCTSTR pStrStyleValue);
	CDuiString GetEffectsStyle(LPCTSTR pStrStyleName);
	const CStdStringPtrMap& GetEffectsStyles() const;
	bool RemoveEffectStyle(LPCTSTR pStrStyleName);
	void RemoveAllEffectStyle();

	bool AddControlStyle(LPCTSTR pStrStyleName,LPCTSTR pStrKey,LPCTSTR pStrVal,LPCTSTR pStylesName = NULL);
	bool AddControlStyle(LPCTSTR pStrStyleName,CStdStringPtrMap* _StyleMap,LPCTSTR pStylesName = NULL);
	bool SetControlStyle(LPCTSTR pStrStyleName,LPCTSTR pStrKey,LPCTSTR pStrVal,LPCTSTR pStylesName = NULL);
	bool SetControlStyle(LPCTSTR pStrStyleName,CStdStringPtrMap* _StyleMap,LPCTSTR pStylesName = NULL);
	CDuiString GetControlStyle(LPCTSTR pStrStyleName,LPCTSTR pStrKey,LPCTSTR pStylesName = NULL);
	CStdStringPtrMap* GetControlsStyles(LPCTSTR pStylesName = NULL) const;
	CStdStringPtrMap* GetControlStyles(LPCTSTR pStrStyleName,LPCTSTR pStylesName = NULL) const;
	bool RemoveControlStyle(LPCTSTR pStrStyleName,LPCTSTR pStrKey = NULL,LPCTSTR pStylesName = NULL);
	void RemoveAllControlStyle(LPCTSTR pStrStyleName = NULL,LPCTSTR pStylesName = NULL);
	bool SetCurStyles(LPCTSTR pStylesName = NULL,bool _NowUpdate = true);
	bool SetCurStyles(int _iStyleIndex = 0,bool _NowUpdate = true);
	UINT GetStylesCount();
	CDuiString GetCurStylesName();
	bool RemoveStyles(LPCTSTR pStylesName);
	void RemoveAllStyles();

	bool AddPropertyAction(LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop = false,bool bAutoStart = true);
	bool AddPropertyAction(TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop = false,bool bAutoStart = true);
	bool SetPropertyAction(TAGroup& tAGroup,TProperty* tProperty,bool bAutoCreate = false);
	bool SetPropertyAction(LPCTSTR pAGroupName,TProperty* tProperty,bool bAutoCreate = false);
	bool SetPropertyAction(LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop = false,bool bAutoStart = true,bool bAutoCreate = false);
	bool SetPropertyAction(TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop = false,bool bAutoStart = true,bool bAutoCreate = false);
	TProperty* GetPropertyAction(LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType) const;
	TProperty* GetPropertyAction(TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType) const;
	TProperty* GetPropertyAction(LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType,int& iIndex) const;
	TProperty* GetPropertyAction(TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType,int& iIndex) const;
	bool RemovePropertyAction(LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType);
	bool SetPropertyActionParse(TProperty& nTProperty,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop = false,bool bAutoStart = true);
	int HasPropertyMsgType(TAGroup& tAGroup,LPCTSTR pType);
	int HasPropertyMsgType(LPCTSTR pAGroupName,LPCTSTR pType);

	bool AddActionScriptGroup(LPCTSTR pAGroupName,LPCTSTR pNotifyName,int iDefaultInterval = 0,int iDefaultTimer = 500,bool bDefaultReverse = false,bool bDefaultLoop = false,bool bDefaultAutoStart = true);
	bool AddActionScriptGroup(LPCTSTR pAGroupName,EVENTTYPE_UI pEventType,int iDefaultInterval = 0,int iDefaultTimer = 500,bool bDefaultReverse = false,bool bDefaultLoop = false,bool bDefaultAutoStart = true);
	bool SetActionScriptGroup(TAGroup** tAGroup,bool bMergerProperty = true);
	bool SetActionScriptGroup(LPCTSTR pAGroupName,LPCTSTR pNotifyName,int iDefaultInterval = 0,int iDefaultTimer = 500,bool bDefaultReverse = false,bool bDefaultLoop = false,bool bDefaultAutoStart = true,bool bMergerProperty = false);
	bool SetActionScriptGroup(LPCTSTR pAGroupName,EVENTTYPE_UI pEventType,int iDefaultInterval = 0,int iDefaultTimer = 500,bool bDefaultReverse = false,bool bDefaultLoop = false,bool bDefaultAutoStart = true,bool bMergerProperty = true);
	TAGroup* GetActionScriptGroup(LPCTSTR pAGroupName) const;
	bool RemoveActionScriptGroup(LPCTSTR pAGroupName);
	void RemoveActionScriptGroupAll();
	int HasActionScriptGroup(TAGroup& tAGroup,LPCTSTR pMsgType = NULL);
	int HasActionScriptGroup(TAGroup& tAGroup,int pEventValue = 0);
	int HasActionScriptGroup(LPCTSTR pAGroupName,LPCTSTR pMsgType = NULL);
	int HasActionScriptGroup(LPCTSTR pAGroupName,int pEventValue = 0);

    CControlUI* GetFocus() const;
    void SetFocus(CControlUI* pControl);
    void SetFocusNeeded(CControlUI* pControl);

    bool SetNextTabControl(bool bForward = true);

    bool SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse);
    bool KillTimer(CControlUI* pControl, UINT nTimerID);
    void KillTimer(CControlUI* pControl);
    void RemoveAllTimers();

    void SetCapture();
    void ReleaseCapture();
    bool IsCaptured();

	void EventAllControl(TEventUI& event,CControlUI* pControl = NULL);
    bool AddNotifier(INotifyUI* pControl);
    bool RemoveNotifier(INotifyUI* pControl);
    void SendNotify(TNotifyUI& Msg, bool bAsync = false);
    void SendNotify(CControlUI* pControl, LPCTSTR pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

    bool AddPreMessageFilter(IMessageFilterUI* pFilter);
    bool RemovePreMessageFilter(IMessageFilterUI* pFilter);

    bool AddMessageFilter(IMessageFilterUI* pFilter);
    bool RemoveMessageFilter(IMessageFilterUI* pFilter);

    int GetPostPaintCount() const;
    bool AddPostPaint(CControlUI* pControl);
    bool RemovePostPaint(CControlUI* pControl);
    bool SetPostPaintIndex(CControlUI* pControl, int iIndex);

    void AddDelayedCleanup(CControlUI* pControl);

	bool AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool TranslateAccelerator(LPMSG pMsg);

    CControlUI* GetRoot() const;
    CControlUI* FindControl(POINT pt) const;
    CControlUI* FindControl(LPCTSTR pstrName) const;
    CControlUI* FindSubControlByPoint(CControlUI* pParent, POINT pt) const;
    CControlUI* FindSubControlByName(CControlUI* pParent, LPCTSTR pstrName) const;
    CControlUI* FindSubControlByClass(CControlUI* pParent, LPCTSTR pstrClass, int iIndex = 0);
    CStdPtrArray* FindSubControlsByClass(CControlUI* pParent, LPCTSTR pstrClass);
    CStdPtrArray* GetSubControlsByClass();

    static void MessageLoop();
    static bool TranslateMessage(const LPMSG pMsg);
	static void Term();

    bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
    bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	void UsedVirtualWnd(bool bUsed);

	bool AddAnimationJob(const CDxAnimationUI& _DxAnimationUI);
	HPEN GetThemePen(UITYPE_COLOR Index) const;
	HFONT GetThemeFont(UITYPE_FONT Index) const;
	HBRUSH GetThemeBrush(UITYPE_COLOR Index) const;
	COLORREF GetThemeColor(UITYPE_COLOR Index) const;
	HICON GetThemeIcon(int Index, int cxySize) const;
	const TEXTMETRIC& GetThemeFontInfo(UITYPE_FONT Index) const;
	bool GetThemeColorPair(UITYPE_COLOR Index, COLORREF& clr1, COLORREF& clr2) const;

	CEventSource& GetEventSource();
	CDuiTrayIcon& GetTrayObject();
private:
    static CControlUI* CALLBACK __FindControlFromNameHash(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromCount(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromPoint(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromTab(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromShortcut(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromUpdate(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromName(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromClass(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlsFromClass(CControlUI* pThis, LPVOID pData);

private:
    HWND m_hWndPaint;
	int m_nOpacity;
    HDC m_hDcPaint;
	LPBYTE m_pBmpBackgroundBits;
    HDC m_hDcOffscreen;
    HDC m_hDcBackground;
    HBITMAP m_hbmpOffscreen;
    HBITMAP m_hbmpBackground;
    HWND m_hwndTooltip;
    TOOLINFO m_ToolTip;
    bool m_bShowUpdateRect;
	bool m_bIsRestore;
	//redrain
	RECT m_rtCaret;
	bool m_bCaretActive;
	bool m_bCaretShowing;
	CRichEditUI* m_currentCaretObject;

	CShadowUI m_shadow;
	bool m_bUseGdiplusText;
	//
	ULONG_PTR						m_gdiplusToken;
	Gdiplus::GdiplusStartupInput	*m_pGdiplusStartupInput;
	//
	CDuiTrayIcon mDuiTray;
    //
    CControlUI* m_pRoot;
    CControlUI* m_pFocus;
    CControlUI* m_pEventHover;
    CControlUI* m_pEventClick;
    CControlUI* m_pEventKey;
    //
    POINT m_ptLastMousePos;
    SIZE m_szMinWindow;
    SIZE m_szMaxWindow;
    SIZE m_szInitWindowSize;
    RECT m_rcSizeBox;
    SIZE m_szRoundCorner;
    RECT m_rcCaption;
    UINT m_uTimerID;
    bool m_bFirstLayout;
    bool m_bUpdateNeeded;
    bool m_bFocusNeeded;
    bool m_bOffscreenPaint;
    bool m_bAlphaBackground;
    bool m_bMouseTracking;
    bool m_bMouseCapture;
	bool m_bUsedVirtualWnd;
	//
	CDuiString m_sCurStylesName;
    //
    CStdPtrArray m_aNotifiers;
    CStdPtrArray m_aTimers;
    CStdPtrArray m_aPreMessageFilters;
    CStdPtrArray m_aMessageFilters;
    CStdPtrArray m_aPostPaintControls;
    CStdPtrArray m_aDelayedCleanup;
    CStdPtrArray m_aAsyncNotify;
    CStdPtrArray m_aFoundControls;
    CStdStringPtrMap m_mNameHash;
    CStdStringPtrMap m_mOptionGroup;
	CStdStringPtrMap m_mControlsStyle;
	CStdStringPtrMap* m_pControlsStyle;
	CStdStringPtrMap m_mStyles;
	CStdStringPtrMap m_mEffectsStyle;
	TStdStringPtrMap<TAGroup*>	m_mActionScript;
    //
    CPaintManagerUI* m_pParentResourcePM;
    DWORD m_dwDefaultDisabledColor;
    DWORD m_dwDefaultFontColor;
    DWORD m_dwDefaultLinkFontColor;
    DWORD m_dwDefaultLinkHoverFontColor;
    DWORD m_dwDefaultSelectedBkColor;
    TFontInfo m_DefaultFontInfo;
    CStdPtrArray m_aCustomFonts;

    CStdStringPtrMap m_mImageHash;
    CStdStringPtrMap m_DefaultAttrHash;
    //
    static HINSTANCE m_hInstance;
    static HINSTANCE m_hResourceInstance;
    static CDuiString m_pStrResourcePath;
    static CDuiString m_pStrResourceZip;
    static bool m_bCachedResourceZip;
    static HANDLE m_hResourceZip;
    static short m_H;
    static short m_S;
    static short m_L;
    static CStdPtrArray m_aPreMessages;
	static CStdPtrArray m_aPlugins;

public:
	static CDuiString m_pStrDefaultFontName;
	CStdPtrArray m_aTranslateAccelerator;
};

} // namespace UiLib

#endif // __UIMANAGER_H__
