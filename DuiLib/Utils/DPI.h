#ifndef __DPI_H__
#define __DPI_H__
#pragma once
#include "ShellScalingAPI.h"
namespace DuiLib
{

/////不支持了，http://stackoverflow.com/questions/18659208/specify-delay-loading-dll-with-pragma-in-the-cpp-file
//#pragma comment(linker, "/DELAYLOAD:SHCore.dll") // 延迟加载的库


 class UILIB_API CDPI
{
public:
	CDPI(void);

	int  Scale(int x);

	UINT GetScale();

	void SetScale(__in UINT iDPI);
	
	PROCESS_DPI_AWARENESS GetAwareness();
	

	void SetAwareness(PROCESS_DPI_AWARENESS awareness);
	

	// Scale rectangle from raw pixels to relative pixels.
	void ScaleRect(__inout RECT *pRect);
	

	// Scale Point from raw pixels to relative pixels.
	void ScalePoint(__inout POINT *pPoint);
	BOOL bIsWindowsXPOrLater;
	BOOL bIsWindows7OrLater;
	BOOL bIsWindows8OrLater;
	

private:
	UINT m_nScaleFactor;
	UINT m_nScaleFactorSDA;
	PROCESS_DPI_AWARENESS m_Awareness;
};

 __declspec(dllimport) CDPI   g_Dpi;
}
#endif //__DPI_H__