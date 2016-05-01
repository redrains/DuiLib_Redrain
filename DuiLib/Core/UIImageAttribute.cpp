#include "StdAfx.h"
#include "UIImageAttribute.h"
namespace DuiLib {

	CImageAttribute::CImageAttribute()
	{
		Clear();
	}

	CImageAttribute::CImageAttribute(const CImageAttribute& image)
	{
		Clone(image);
	}

	const CImageAttribute& CImageAttribute::operator=(const CImageAttribute& image)
	{
		Clone(image);
		return *this;
	}

	void CImageAttribute::Clone(const CImageAttribute& image)
	{
		m_sImageAttribute = image.m_sImageAttribute;

		m_sImage = image.m_sImage;
		m_sResType = image.m_sResType;
		m_imageInfo = image.m_imageInfo;
		m_bLoadSuccess = image.m_bLoadSuccess;

		m_rcDest = image.m_rcDest;
		m_rcSource = image.m_rcSource;
		m_rcCorner = image.m_rcCorner;
		m_bFade = image.m_bFade;
		m_dwMask = image.m_dwMask;
		m_bHole = image.m_bHole;
		m_bTiledX = image.m_bTiledX;
		m_bTiledY = image.m_bTiledY;
	}

	CImageAttribute::~CImageAttribute()
	{

	}

	const CDuiString& CImageAttribute::GetAttributeString() const
	{
		return m_sImageAttribute;
	}

	void CImageAttribute::SetAttributeString(LPCTSTR pStrImageAttri)
	{
		if (m_sImageAttribute == pStrImageAttri)
			return;

		Clear();

		m_sImageAttribute = pStrImageAttri;
		m_sImage = m_sImageAttribute;
		ParseAttribute(pStrImageAttri);
	}

	bool CImageAttribute::LoadImage(CPaintManagerUI* pManager)
	{
		if (m_imageInfo != NULL)
			return true;
		if (!m_bLoadSuccess)
			return false;

		const TImageInfo* data = NULL;
		if (m_sResType.IsEmpty())
		{
			data = pManager->GetImageEx((LPCTSTR)m_sImage, NULL, m_dwMask);
		}
		else
		{
			data = pManager->GetImageEx((LPCTSTR)m_sImage, (LPCTSTR)m_sResType, m_dwMask);
		}
		if (data == NULL)
		{
			m_bLoadSuccess = false;
			return false;
		}
		else
		{
			m_bLoadSuccess = true;
		}
		
		if (m_rcSource.left == 0 && m_rcSource.right == 0 && m_rcSource.top == 0 && m_rcSource.bottom == 0)
		{
			m_rcSource.right = data->nX;
			m_rcSource.bottom = data->nY;
		}
		if (m_rcSource.right > data->nX) m_rcSource.right = data->nX;
		if (m_rcSource.bottom > data->nY) m_rcSource.bottom = data->nY;
		m_imageInfo = const_cast<TImageInfo*>(data);

		return true;
	}

	bool CImageAttribute::IsLoadSuccess()
	{
		return !m_sImageAttribute.IsEmpty() && m_bLoadSuccess;
	}

	void CImageAttribute::ModifyAttribute(LPCTSTR pStrModify)
	{
		ParseAttribute(pStrModify);
	}

	void CImageAttribute::Clear()
	{
		m_sImageAttribute.Empty();
		m_sImage.Empty();
		m_sResType.Empty();
		m_imageInfo = NULL;
		m_bLoadSuccess = true;
		ZeroMemory(&m_rcDest, sizeof(RECT));
		ZeroMemory(&m_rcSource, sizeof(RECT));
		ZeroMemory(&m_rcCorner, sizeof(RECT));
		m_bFade = 0xFF;
		m_dwMask = 0;
		m_bHole = false;
		m_bTiledX = false;
		m_bTiledY = false;
	}

	void CImageAttribute::ParseAttribute(LPCTSTR pStrImage)
	{
		if (pStrImage == NULL)
			return;

		// 1¡¢aaa.jpg
		// 2¡¢file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' 
		// mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false'
		CDuiString sItem;
		CDuiString sValue;
		LPTSTR pstr = NULL;

		while (*pStrImage != _T('\0'))
		{
			sItem.Empty();
			sValue.Empty();
			while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
			while (*pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' '))
			{
				LPTSTR pstrTemp = ::CharNext(pStrImage);
				while (pStrImage < pstrTemp)
				{
					sItem += *pStrImage++;
				}
			}
			while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
			if (*pStrImage++ != _T('=')) break;
			while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
			if (*pStrImage++ != _T('\'')) break;
			while (*pStrImage != _T('\0') && *pStrImage != _T('\''))
			{
				LPTSTR pstrTemp = ::CharNext(pStrImage);
				while (pStrImage < pstrTemp)
				{
					sValue += *pStrImage++;
				}
			}
			if (*pStrImage++ != _T('\'')) break;
			if (!sValue.IsEmpty())
			{
				if (sItem == _T("file") || sItem == _T("res"))
				{
					m_sImage = sValue;
				}
				else if (sItem == _T("restype"))
				{					
					m_sResType = sValue;
				}
				else if (sItem == _T("dest"))
				{
					m_rcDest.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);
					m_rcDest.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					m_rcDest.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					m_rcDest.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				}
				else if (sItem == _T("source"))
				{
					m_rcSource.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);
					m_rcSource.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					m_rcSource.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					m_rcSource.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				}
				else if (sItem == _T("corner"))
				{
					m_rcCorner.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);
					m_rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					m_rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					m_rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				}
				else if (sItem == _T("mask"))
				{
					if (sValue[0] == _T('#')) m_dwMask = _tcstoul(sValue.GetData() + 1, &pstr, 16);
					else m_dwMask = _tcstoul(sValue.GetData(), &pstr, 16);
				}
				else if (sItem == _T("fade"))
				{
					m_bFade = (BYTE)_tcstoul(sValue.GetData(), &pstr, 10);
				}
				else if (sItem == _T("hole"))
				{
					m_bHole = (_tcscmp(sValue.GetData(), _T("true")) == 0);
				}
				else if (sItem == _T("xtiled"))
				{
					m_bTiledX = (_tcscmp(sValue.GetData(), _T("true")) == 0);
				}
				else if (sItem == _T("ytiled"))
				{
					m_bTiledY = (_tcscmp(sValue.GetData(), _T("true")) == 0);
				}
			}
			if (*pStrImage++ != _T(' ')) break;
		}
	}

	void CImageAttribute::SetDest(const RECT &rcDest)
	{
		m_rcDest = rcDest;
	}

	RECT CImageAttribute::GetDest() const
	{
		return m_rcDest;
	}

	const TImageInfo* CImageAttribute::GetImageInfo() const
	{
		return m_imageInfo;
	}

} // namespace DuiLib
