#include "StdAfx.h"
#include "DPI.h"
#pragma comment(lib, "Shcore")
#pragma comment(lib, "delayimp")
namespace DuiLib
{
	//96 DPI = 100% scaling
	//120 DPI = 125% scaling
	//144 DPI = 150% scaling
	//168 DPI = 175% scaling
	//192 DPI = 200% scaling


	 BOOL CDPI::bIsWindowsXPOrLater;
	 BOOL CDPI::bIsWindowsVistaOrLater;
	 BOOL CDPI::bIsWindows7OrLater;
	 BOOL CDPI::bIsWindows8OrLater;
	 BOOL CDPI::bIsWindowsBlueOrLater;
	 BOOL CDPI::bIsWindows10OrLater;
	 CDPI::Initializer OSVersionInitializationGuard;
	CDPI::CDPI()
	{

		m_nScaleFactor = 0;
		m_nScaleFactorSDA = 0;
		m_Awareness = PROCESS_PER_MONITOR_DPI_AWARE;

		SetScale(96);
	}

	int DuiLib::CDPI::GetDPIOfMonitor(HMONITOR hMonitor)
	{
		UINT     dpix = 96, dpiy = 96;
		if (CDPI::bIsWindowsBlueOrLater) {
			
			HRESULT  hr = E_FAIL;
			hr = GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy);

			if (hr != S_OK)
			{
				MessageBox(NULL, (LPCWSTR)L"GetDpiForMonitor failed", (LPCWSTR)L"Notification", MB_OK);
				return 96;
			}

		}
		else {

			HDC screen = GetDC(0);
			dpix = GetDeviceCaps(screen, LOGPIXELSX);
			//dpiy = GetDeviceCaps(screen, LOGPIXELSY);
			ReleaseDC(0, screen);
		}


		return dpix;
	}

	int DuiLib::CDPI::GetDPIOfMonitorNearestToPoint(POINT pt)
	{
		HMONITOR hMonitor;
		hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
		return GetDPIOfMonitor(hMonitor);
	}

	int DuiLib::CDPI::GetMainMonitorDPI()
	{

		
		POINT    pt;
		// Get the DPI for the main monitor
		pt.x = 1;
		pt.y = 1;
		return GetDPIOfMonitorNearestToPoint(pt);
		
	}

	PROCESS_DPI_AWARENESS CDPI::GetAwareness()
	{
		
		if (bIsWindowsBlueOrLater) {
			HANDLE hProcess;
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());
			GetProcessDpiAwareness(hProcess, &m_Awareness);
		}
		
		return m_Awareness;
	}

	void CDPI::SetAwareness(PROCESS_DPI_AWARENESS awareness)
	{
		HRESULT hr = E_FAIL;
		if (bIsWindowsBlueOrLater) {

			hr = SetProcessDpiAwareness(awareness);
			if (hr == S_OK)
			{
				m_Awareness = awareness;
			}
			else
			{
				MessageBox(NULL, (LPCWSTR)L"SetProcessDpiAwareness Error", (LPCWSTR)L"Error", MB_OK);
			}

		}
		else {
			m_Awareness = awareness;
		}


		
		return;
	}

	UINT DuiLib::CDPI::GetDPI()
	{
		if (m_Awareness == PROCESS_DPI_UNAWARE)
		{
			return 96;
		}

		if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE)
		{
			return MulDiv(m_nScaleFactorSDA,96,100);
		}

		return MulDiv(m_nScaleFactor,96,100);
	}

	UINT CDPI::GetScale()
	{
		if (m_Awareness == PROCESS_DPI_UNAWARE)
		{
			return 100;
		}

		if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE)
		{
			return m_nScaleFactorSDA;
		}

		return m_nScaleFactor;
	}


	void CDPI::SetScale(__in UINT iDPI)
	{
		m_nScaleFactor = MulDiv(iDPI, 100, 96);
		if (m_nScaleFactorSDA == 0)
		{
			m_nScaleFactorSDA = m_nScaleFactor;  // Save the first scale factor, which is all that SDA apps know about
		}
		return;
	}

	int  CDPI::Scale(int x)
	{
		// DPI Unaware:  Return the input value with no scaling.
		// These apps are always virtualized to 96 DPI and scaled by the system for the DPI of the monitor where shown.
		if (m_Awareness == PROCESS_DPI_UNAWARE)
		{
			return x;
		}

		// System DPI Aware:  Return the input value scaled by the factor determined by the system DPI when the app was launched.
		// These apps render themselves according to the DPI of the display where they are launched, and they expect that scaling
		// to remain constant for all displays on the system.
		// These apps are scaled up or down when moved to a display with a different DPI from the system DPI.
		if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE)
		{
			return MulDiv(x, m_nScaleFactorSDA, 100);
		}

		// Per-Monitor DPI Aware:  Return the input value scaled by the factor for the display which contains most of the window.
		// These apps render themselves for any DPI, and re-render when the DPI changes (as indicated by the WM_DPICHANGED window message).
		return MulDiv(x, m_nScaleFactor, 100);
	}


	

	


	void CDPI::ScaleRect(__inout RECT *pRect)
	{
		int width = pRect->right - pRect->left;
		int height = pRect->bottom - pRect->top;
		int sw = Scale(width);
		int sh = Scale(height);
		pRect->left = Scale(pRect->left);
		//pRect->right = Scale(pRect->right);
		pRect->top = Scale(pRect->top);
		//pRect->bottom = Scale(pRect->bottom);
		
		pRect->right = pRect->left + sw;
		pRect->bottom = pRect->top + sh;
	}

	void CDPI::ScalePoint(__inout POINT *pPoint)
	{
		pPoint->x = Scale(pPoint->x);
		pPoint->y = Scale(pPoint->y);
	}

}