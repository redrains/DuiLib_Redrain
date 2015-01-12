// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"
#include "resource.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <atlbase.h>
#include <atlstr.h>
#include <limits>


// TODO: 在此处引用程序需要的其他头文件


#include "Uilib.h"

//程序入口处调用该函数进行内存泄露检测
//程序退出前调用_CrtDumpMemoryLeaks();函数可查看内存泄露的ID号，该ID号可作为Breakpoint的参数，运行时将会在相应位置自动断点
inline  void  EnableMemLeakCheck(int Breakpoint = 0){
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)  |  _CRTDBG_LEAK_CHECK_DF);
	if(Breakpoint)
		_CrtSetBreakAlloc ( 1912 );
}

#ifdef _DEBUG
#define  new   new(_NORMAL_BLOCK, __FILE__, __LINE__) 
#endif 
#define _CRT_SECURE_NO_DEPRECATE


#if _MSC_VER == 1500
#ifdef _DEBUG
#ifndef Uilib_u_EXPORTS
#pragma comment(lib, ".Uilib_ud.lib")
#else
#pragma comment(lib, "Uilib_ud.lib")
#endif
#else
#ifndef Uilib_u_EXPORTS
#pragma comment(lib, "Uilib_u.lib")
#else
#pragma comment(lib, "Uilib_u.lib")
#endif
#endif
#endif

#if _MSC_VER == 1600
#ifdef _DEBUG
#ifndef Uilib_u_EXPORTS
#pragma comment(lib, "Uilib_ud.lib")
#else
#pragma comment(lib, "Uilib_ud.lib")
#endif
#else
#ifndef Uilib_u_EXPORTS
#pragma comment(lib, "Uilib_u.lib")
#else
#pragma comment(lib, "Uilib_u.lib")
#endif
#endif
#endif

#if _MSC_VER == 1700
#ifdef _DEBUG
#ifndef Uilib_u_EXPORTS
#pragma comment(lib, "Uilib_ud.lib")
#else
#pragma comment(lib, "Uilib_ud.lib")
#endif
#else
#ifndef Uilib_u_EXPORTS
#pragma comment(lib, "Uilib_u.lib")
#else
#pragma comment(lib, "Uilib_u_u.lib")
#endif
#endif
#endif
