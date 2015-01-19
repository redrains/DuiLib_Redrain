#ifndef UIDxAnimation_h__
#define UIDxAnimation_h__

#pragma once

namespace UiLib
{
	typedef enum
	{
		UIANIMTYPE_FLAT,
		UIANIMTYPE_SWIPE,
	} UITYPE_ANIM;

	class UILIB_API CDxAnimationUI
	{
	public:
		CDxAnimationUI(const CDxAnimationUI& src);
		CDxAnimationUI(UITYPE_ANIM AnimType, DWORD dwStartTick, DWORD dwDuration, COLORREF clrBack, COLORREF clrKey, RECT rcFrom, int xtrans, int ytrans, int ztrans, int alpha, float zrot);
		~CDxAnimationUI(void);

		typedef enum
		{
			INTERPOLATE_LINEAR,
			INTERPOLATE_COS,
		} INTERPOLATETYPE;

		typedef struct PLOTMATRIX
		{
			int xtrans;
			int ytrans;
			int ztrans;
			int alpha;
			float zrot;
		} PLOTMATRIX;

		UITYPE_ANIM AnimType;
		DWORD dwStartTick;
		DWORD dwDuration;
		int iBufferStart;
		int iBufferEnd;
		union
		{
			struct 
			{
				COLORREF clrBack;
				COLORREF clrKey;
				RECT rcFrom;
				PLOTMATRIX mFrom;
				INTERPOLATETYPE iInterpolate;
			} plot;
		} data;
	};
}

#endif // UIDxAnimation_h__
