#ifndef __DPI_H__
#define __DPI_H__
#pragma once
#include "ShellScalingAPI.h"
#include <VersionHelpers.h>
namespace DuiLib
{

/////不支持了，http://stackoverflow.com/questions/18659208/specify-delay-loading-dll-with-pragma-in-the-cpp-file
//#pragma comment(linker, "/DELAYLOAD:SHCore.dll") // 延迟加载的库


 class UILIB_API CDPI
{
public:
	CDPI(void);

	static int GetMainMonitorDPI();
	static int GetDPIOfMonitor(HMONITOR hMonitor);
	static int GetDPIOfMonitorNearestToPoint(POINT pt);
	int  Scale(int x);

	UINT GetScale();
	UINT GetDPI();
	void SetScale(__in UINT iDPI);
	
	PROCESS_DPI_AWARENESS GetAwareness();
	

	void SetAwareness(PROCESS_DPI_AWARENESS awareness);
	

	// Scale rectangle from raw pixels to relative pixels.
	void ScaleRect(__inout RECT *pRect);
	

	// Scale Point from raw pixels to relative pixels.
	void ScalePoint(__inout POINT *pPoint);

	struct Initializer {
		Initializer() {
			OSVERSIONINFO osvi;
			ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

			GetVersionEx(&osvi);

			//_WIN32_WINNT_WINXP
			bIsWindowsXPOrLater = IsWindowsXPOrGreater();
				

			bIsWindowsVistaOrLater = IsWindowsVistaOrGreater();

			bIsWindows7OrLater = IsWindows7OrGreater();
			bIsWindows8OrLater = IsWindows8OrGreater();

			bIsWindowsBlueOrLater = IsWindows8Point1OrGreater();

			//bIsWindows10OrLater = IsWindows10OrGreater();
		}
	};

	
	static BOOL bIsWindowsXPOrLater;
	static BOOL bIsWindowsVistaOrLater;
	static BOOL bIsWindows7OrLater;
	static BOOL bIsWindows8OrLater;
	static BOOL bIsWindowsBlueOrLater;
	static BOOL bIsWindows10OrLater;
	static Initializer OSVersionInitializationGuard;

private:
	UINT m_nScaleFactor;
	UINT m_nScaleFactorSDA;
	PROCESS_DPI_AWARENESS m_Awareness;
};

}
#endif //__DPI_H__