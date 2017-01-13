// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <string>
// TODO: 在此处引用程序需要的其他头文件
#include "Uilib.h"
#include "resource.h"
using namespace DuiLib;
using namespace std;

#pragma warning(disable:4192)

#ifdef _UNICODE
typedef std::wstring	tstring;
#else
typedef std::string		tstring;
#endif