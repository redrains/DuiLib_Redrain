#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "Uilib.h"
using namespace DuiLib;

#include "Controls/AnimLayout.h"
#include "FrameWnd.h"



#pragma warning(disable:4192)

#ifdef _UNICODE
	typedef std::wstring	tstring;
#else
	typedef std::string		tstring;
#endif

#ifdef _UNICODE
#	ifdef _DEBUG
#		pragma comment(lib, "DuiLib_ud.lib")
#	else
#		pragma comment(lib, "DuiLib_u.lib")
#	endif
#else
#	ifdef _DEBUG
#		pragma comment(lib, "DuiLib_d.lib")
#	else
#		pragma comment(lib, "DuiLib.lib")
#	endif
#endif