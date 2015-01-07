#include "StdAfx.h"
#include "UIGifAnim.h"

namespace DuiLib
{

	CGifAnimUI::CGifAnimUI(void)
	{
		m_pGifImage			=	NULL;
		m_pPropertyItem		=	NULL;
		m_nFrameCount		=	0;	
		m_nFramePosition	=	0;	
		m_bIsAutoPlay		=	true;
		m_bIsPlaying		=	false;

	}


	CGifAnimUI::~CGifAnimUI(void)
	{
		DeleteGif();
		m_pManager->KillTimer( this, EVENT_TIEM_ID );

	}

	LPCTSTR CGifAnimUI::GetClass() const
	{
		return _T("GifAnimUI");
	}

	LPVOID CGifAnimUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, DUI_CTR_GIFANIM) == 0 ) return static_cast<CGifAnimUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	void CGifAnimUI::DoPaint( HDC hDC, const RECT& rcPaint )
	{
		if( !::IntersectRect( &m_rcPaint, &rcPaint, &m_rcItem ) ) return;
		if ( NULL == m_pGifImage )
		{		
			InitGifImage();
		}
		DrawFrame( hDC );
	}

	void CGifAnimUI::DoEvent( TEventUI& event )
	{
		if( event.Type == UIEVENT_TIMER )
			OnTimer( (UINT_PTR)event.wParam );
	}

	void CGifAnimUI::SetVisible(bool bVisible /* = true */)
	{
		CLabelUI::SetVisible(bVisible);
		if (bVisible)
			PlayGif();
		else
			StopGif();
	}

	void CGifAnimUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("autoplay")) == 0 ) {
			SetAutoPlay(_tcscmp(pstrValue, _T("true")) == 0);
		}
		else
			CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CGifAnimUI::SetAutoPlay(bool bIsAuto)
	{
		m_bIsAutoPlay = bIsAuto;
	}

	bool CGifAnimUI::IsAutoPlay() const
	{
		return m_bIsAutoPlay;
	}

	void CGifAnimUI::PlayGif()
	{
		if (m_bIsPlaying)
		{
			return;
		}

		long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
		if ( lPause == 0 ) lPause = 100;
		m_pManager->SetTimer( this, EVENT_TIEM_ID, lPause );

		m_bIsPlaying = true;
	}

	void CGifAnimUI::PauseGif()
	{
		if (!m_bIsPlaying)
		{
			return;
		}

		m_pManager->KillTimer(this, EVENT_TIEM_ID);
		this->Invalidate();
		m_bIsPlaying = false;
	}

	void CGifAnimUI::StopGif()
	{
		if (!m_bIsPlaying)
		{
			return;
		}

		m_pManager->KillTimer(this, EVENT_TIEM_ID);
		m_nFramePosition = 0;
		this->Invalidate();
		m_bIsPlaying = false;
	}

	void CGifAnimUI::InitGifImage()
	{
		CDuiString sImageName	=	GetBkImage();
		CDuiString sImagePath	=	CPaintManagerUI::GetResourcePath();
		sImagePath += sImageName;
		if ( NULL == sImagePath ) return;
		m_pGifImage = Gdiplus::Image::FromFile( sImagePath );
		if ( NULL == m_pGifImage ) return;
		UINT nCount	= 0;
		nCount	=	m_pGifImage->GetFrameDimensionsCount();
		GUID* pDimensionIDs	=	new GUID[ nCount ];
		m_pGifImage->GetFrameDimensionsList( pDimensionIDs, nCount );
		m_nFrameCount	=	m_pGifImage->GetFrameCount( &pDimensionIDs[0] );
		int nSize		=	m_pGifImage->GetPropertyItemSize( PropertyTagFrameDelay );
		m_pPropertyItem	=	(Gdiplus::PropertyItem*) malloc( nSize );
		m_pGifImage->GetPropertyItem( PropertyTagFrameDelay, nSize, m_pPropertyItem );
		delete  pDimensionIDs;
		pDimensionIDs = NULL;

		if (m_bIsAutoPlay)
		{
			PlayGif();
		}
	}

	void CGifAnimUI::DeleteGif()
	{
		if ( m_pGifImage != NULL )
		{
			delete m_pGifImage;
			m_pGifImage = NULL;
		}

		if ( m_pPropertyItem != NULL )
		{
			free( m_pPropertyItem );
			m_pPropertyItem = NULL;
		}
		m_nFrameCount		=	0;	
		m_nFramePosition	=	0;	
	}


void CGifAnimUI::OnTimer( UINT_PTR idEvent )
{
	if ( idEvent != EVENT_TIEM_ID )
		return;
	m_pManager->KillTimer( this, EVENT_TIEM_ID );
	this->Invalidate();

	m_nFramePosition = (++m_nFramePosition) % m_nFrameCount;

	long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
	if ( lPause == 0 ) lPause = 100;
	m_pManager->SetTimer( this, EVENT_TIEM_ID, lPause );
}

void CGifAnimUI::DrawFrame( HDC hDC )
{
	if ( NULL == hDC || NULL == m_pGifImage ) return;
	GUID pageGuid = Gdiplus::FrameDimensionTime;
	Gdiplus::Graphics graphics( hDC );
	graphics.DrawImage( m_pGifImage, m_rcItem.left, m_rcItem.top, m_rcItem.right-m_rcItem.left, m_rcItem.bottom-m_rcItem.top );
	m_pGifImage->SelectActiveFrame( &pageGuid, m_nFramePosition );
}

}