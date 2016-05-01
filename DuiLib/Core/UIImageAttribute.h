#ifndef __UIIMAGE_ATTRIBUTE_H__
#define __UIIMAGE_ATTRIBUTE_H__

#pragma once

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CImageAttribute
{
public:
	CImageAttribute();
	CImageAttribute(const CImageAttribute&);
	const CImageAttribute& operator=(const CImageAttribute&);
	virtual ~CImageAttribute();

    const CDuiString& GetAttributeString() const;
	void SetAttributeString(LPCTSTR pStrImageAttri);
	void ModifyAttribute(LPCTSTR pStrModify);
	bool LoadImage(CPaintManagerUI* pManager);
	bool IsLoadSuccess();

	RECT GetDest() const;
	void SetDest(const RECT &rcDest);
	const TImageInfo* GetImageInfo() const;
private:
	void Clone(const CImageAttribute&);
	void Clear();
	void ParseAttribute(LPCTSTR pStrImageAttri);

protected:
	friend class CRenderEngine;
	CDuiString	m_sImageAttribute;

	CDuiString	m_sImage;
	CDuiString	m_sResType;
	TImageInfo	*m_imageInfo;
	bool		m_bLoadSuccess;

	RECT	m_rcDest;
	RECT	m_rcSource;
	RECT	m_rcCorner;
	BYTE	m_bFade;
	DWORD	m_dwMask;
	bool	m_bHole;
	bool	m_bTiledX;
	bool	m_bTiledY;

};

} // namespace DuiLib

#endif // __UIIMAGE_ATTRIBUTE_H__
