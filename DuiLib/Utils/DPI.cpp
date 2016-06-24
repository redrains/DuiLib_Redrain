#include "StdAfx.h"
#include "DPI.h"
#pragma comment(lib, "Shcore")
#pragma comment(lib, "delayimp")
namespace DuiLib
{
	/*STDAPI SetProcessDpiAwareness(
		_In_ PROCESS_DPI_AWARENESS value);

	STDAPI GetProcessDpiAwareness(
		_In_opt_ HANDLE hprocess,
		_Out_ PROCESS_DPI_AWARENESS *value);
*/



	CDPI::CDPI()
	{


		OSVERSIONINFO osvi;
		

		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		GetVersionEx(&osvi);

		//_WIN32_WINNT_WINXP
		bIsWindowsXPOrLater =
			((osvi.dwMajorVersion > 5) ||
			((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1)));

		bIsWindows7OrLater =
			((osvi.dwMajorVersion > 6) ||
			((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion >= 1)));
		bIsWindows8OrLater =
			((osvi.dwMajorVersion > 6) ||
			((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion >= 2)));

		m_nScaleFactor = 0;
		m_nScaleFactorSDA = 0;
		m_Awareness = PROCESS_PER_MONITOR_DPI_AWARE;

		//SetScale(120);
	}

	PROCESS_DPI_AWARENESS CDPI::GetAwareness()
	{
		
		if (bIsWindows8OrLater) {
			HANDLE hProcess;
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());
			GetProcessDpiAwareness(hProcess, &m_Awareness);
		}
		
		return m_Awareness;
	}

	void CDPI::SetAwareness(PROCESS_DPI_AWARENESS awareness)
	{
		HRESULT hr = E_FAIL;
		if (bIsWindows8OrLater) {

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
		pRect->left = Scale(pRect->left);
		pRect->right = Scale(pRect->right);
		pRect->top = Scale(pRect->top);
		pRect->bottom = Scale(pRect->bottom);
	}

	void CDPI::ScalePoint(__inout POINT *pPoint)
	{
		pPoint->x = Scale(pPoint->x);
		pPoint->y = Scale(pPoint->y);
	}

}