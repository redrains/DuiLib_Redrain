#ifndef UIlib_h__
#define UIlib_h__

#ifndef UILIB_EXPORTS
#	define UILIB_API
#else
#	ifdef UILIB_EXPORTS
#		if _MSC_VER >= 1500
#			define UILIB_API __declspec(dllexport)
#		else
#			define UILIB_API 
#		endif
#	else
#		if _MSC_VER >= 1500
#			define UILIB_API __declspec(dllimport)
#		else
#			define UILIB_API 
#		endif
#	endif
#endif

#define UILIB_COMDAT __declspec(selectany) 

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <atlbase.h>
#include <atlstr.h>
#include <xstring>
#include <comdef.h>
#include <gdiplus.h>

#pragma comment(lib,"oledlg.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"gdiplus.lib")

#include "Utils/DuiPointer.h"
#include "Utils/Utils.h"
#include "Utils/UIShadow.h"
#include "Utils/Internal.h"
#include "Utils/UIDelegate.h"
#include "Utils/DuiAutoComplete.h"
#include "Utils/DuiTrayIcon.h"

#include "Core/UITimer.h"
#include "Core/UIDefine.h"
#include "Core/UIBase.h"
#include "Core/UIManager.h"
#include "Core/UIDxAnimation.h"

#include "Core/UIControl.h"
#include "Core/UIContainer.h"
#include "Core/UIMarkup.h"
#include "Core/UIDlgBuilder.h"
#include "Core/UIRender.h"

#include "Layout/UIVerticalLayout.h"
#include "Layout/UIHorizontalLayout.h"
#include "Layout/UITileLayout.h"
#include "Layout/UITabLayout.h"
#include "Layout/UIChildLayout.h"

#include "Control/UIList.h"
#include "Control/UICombo.h"
#include "Control/UIScrollBar.h"
#include "Control/UITreeView.h"
#include "Control/UIChartView.h"

#include "Control/UILabel.h"
#include "Control/UIText.h"
#include "Control/UIEdit.h"
#include "Control/UIGifAnim.h"

#include <vector>
#include <algorithm>
#include "Control/UIAnimation.h"
#include "Layout/UIAnimationTabLayout.h"
#include "Control/UIFadeButton.h"
#include "Control/UIButton.h"
#include "Control/UIOption.h"
#include "Control/UICheckBox.h"

#include "Control/UIProgress.h"
#include "Control/UISlider.h"

#include "Control/UIComboBox.h"
#include "Control/UIRichEdit.h"
#include "Control/UIDateTime.h"

#include "Control/UIActiveX.h"
#include "Control/UIWebBrowser.h"
#include "Control/UIFlash.h"

#include "Control/UIMenu.h"

#include "Utils/WinImplBase.h"
#include "Utils/IWindowBase.h"

#include "Control/UIWkeWindowlessBrowser.h"

namespace DuiLib = UiLib;
#endif // UIlib_h__
