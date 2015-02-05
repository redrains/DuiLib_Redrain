// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 在此处引用程序需要的其他头文件
#include "Uilib.h"
using namespace DuiLib;

#include "FrameWnd.h"



#pragma warning(disable:4192)

#ifdef _UNICODE
	typedef std::wstring	tstring;
#else
	typedef std::string		tstring;
#endif

#ifdef _UNICODE
#	ifdef DEBUG
#		pragma comment(lib, "DuiLib_ud.lib")
#	else
#		pragma comment(lib, "DuiLib_u.lib")
#	endif
#else
#	ifdef DEBUG
#		pragma comment(lib, "DuiLib_d.lib")
#	else
#		pragma comment(lib, "DuiLib.lib")
#	endif
#endif