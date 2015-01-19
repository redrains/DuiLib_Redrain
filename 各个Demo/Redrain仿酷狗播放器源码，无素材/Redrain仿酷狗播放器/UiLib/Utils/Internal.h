#ifndef Internal_h__
#define Internal_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d9.h>
#include <math.h>
#include "../Core/UIDxAnimation.h"

namespace UiLib
{
	template< class T >
	class CSafeRelease
	{
	public:
		CSafeRelease(T* p) : m_p(p) { };
		~CSafeRelease() { if( m_p != NULL ) m_p->Release(); };
		T* Detach() { T* t = m_p; m_p = NULL; return t; };
		T* m_p;
	};


	class CAnimationSpooler
	{
	public:
		CAnimationSpooler();
		~CAnimationSpooler();

		enum { MAX_BUFFERS = 80 };

		bool Init(HWND hWnd);
		bool PrepareAnimation(HWND hWnd);
		void CancelJobs();
		bool Render();

		bool IsAnimating() const;
		bool IsJobScheduled() const;
		bool AddJob(CDxAnimationUI* pJob);

	protected:
		void Term();

		COLORREF TranslateColor(LPDIRECT3DSURFACE9 pSurface, COLORREF clr) const;
		bool SetColorKey(LPDIRECT3DTEXTURE9 pTexture, LPDIRECT3DSURFACE9 pSurface, int iTexSize, COLORREF clrColorKey);

		bool PrepareJob_Flat(CDxAnimationUI* pJob);
		bool RenderJob_Flat(const CDxAnimationUI* pJob, LPDIRECT3DSURFACE9 pSurface, DWORD dwTick);

	protected:
		struct CUSTOMVERTEX 
		{
			FLOAT x, y, z;
			FLOAT rhw;
			DWORD color;
			FLOAT tu, tv;
		};
		typedef CUSTOMVERTEX CUSTOMFAN[4];

		HWND m_hWnd;
		bool m_bIsAnimating;
		bool m_bIsInitialized;
		CStdPtrArray m_aJobs;
		D3DFORMAT m_ColorFormat;
		LPDIRECT3D9 m_pD3D;
		LPDIRECT3DDEVICE9 m_p3DDevice;
		LPDIRECT3DSURFACE9 m_p3DBackSurface;
		//
		LPDIRECT3DVERTEXBUFFER9 m_p3DVertices[MAX_BUFFERS];
		LPDIRECT3DTEXTURE9 m_p3DTextures[MAX_BUFFERS];
		CUSTOMFAN m_fans[MAX_BUFFERS];
		int m_nBuffers;
	};

}
#endif // Internal_h__
