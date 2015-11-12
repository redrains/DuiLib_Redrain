#include "StdAfx.h"

namespace UiLib
{

	CDialogBuilder::CDialogBuilder() : m_pCallback(NULL), m_pstrtype(NULL)
	{

	}

	CControlUI* CDialogBuilder::Create(STRINGorID xml, LPCTSTR type, IDialogBuilderCallback* pCallback, 
		CPaintManagerUI* pManager, CControlUI* pParent)
	{
		//资源ID为0-65535，两个字节；字符串指针为4个字节
		//字符串以<开头认为是XML字符串，否则认为是XML文件

		if( HIWORD(xml.m_lpstr) != NULL ) {
			if( *(xml.m_lpstr) == _T('<') ) {
				if( !m_xml.Load(xml.m_lpstr) ) return NULL;
			}
			else {
				if( !m_xml.LoadFromFile(xml.m_lpstr) ) return NULL;
			}
		}
		else {
			HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), xml.m_lpstr, type);
			if( hResource == NULL ) return NULL;
			HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
			if( hGlobal == NULL ) {
				FreeResource(hResource);
				return NULL;
			}

			m_pCallback = pCallback;
			if( !m_xml.LoadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource) )) return NULL;
			::FreeResource(hResource);
			m_pstrtype = type;
		}

		return Create(pCallback, pManager, pParent);
	}

	CControlUI* CDialogBuilder::Create(IDialogBuilderCallback* pCallback, CPaintManagerUI* pManager, CControlUI* pParent)
	{
		m_pCallback = pCallback;
		CMarkupNode root = m_xml.GetRoot();
		if( !root.IsValid() ) return NULL;

		if( pManager ) {
			LPCTSTR pstrClass = NULL;
			int nAttributes = 0;
			LPCTSTR pstrName = NULL;
			LPCTSTR pstrValue = NULL;
			LPTSTR pstr = NULL;
			for( CMarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() ) {
				pstrClass = node.GetName();
				if( _tcscmp(pstrClass, _T("Image")) == 0 ) {
					nAttributes = node.GetAttributeCount();
					LPCTSTR pImageName = NULL;
					LPCTSTR pImageResType = NULL;
					DWORD mask = 0;
					for( int i = 0; i < nAttributes; i++ ) {
						pstrName = node.GetAttributeName(i);
						pstrValue = node.GetAttributeValue(i);
						if( _tcscmp(pstrName, _T("name")) == 0 ) {
							pImageName = pstrValue;
						}
						else if( _tcscmp(pstrName, _T("restype")) == 0 ) {
							pImageResType = pstrValue;
						}
						else if( _tcscmp(pstrName, _T("mask")) == 0 ) {
							if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
							mask = _tcstoul(pstrValue, &pstr, 16);
						}
					}
					if( pImageName ) pManager->AddImage(pImageName, pImageResType, mask);
				}
				else if( _tcscmp(pstrClass, _T("Font")) == 0 ) {
					nAttributes = node.GetAttributeCount();
					LPCTSTR pFontName = NULL;
					int size = 12;
					bool bold = false;
					bool underline = false;
					bool italic = false;
					bool defaultfont = false;
					for( int i = 0; i < nAttributes; i++ ) {
						pstrName = node.GetAttributeName(i);
						pstrValue = node.GetAttributeValue(i);
						if( _tcscmp(pstrName, _T("name")) == 0 ) {
							pFontName = pstrValue;
						}
						else if( _tcscmp(pstrName, _T("size")) == 0 ) {
							size = _tcstol(pstrValue, &pstr, 10);
						}
						else if( _tcscmp(pstrName, _T("bold")) == 0 ) {
							bold = (_tcscmp(pstrValue, _T("true")) == 0);
						}
						else if( _tcscmp(pstrName, _T("underline")) == 0 ) {
							underline = (_tcscmp(pstrValue, _T("true")) == 0);
						}
						else if( _tcscmp(pstrName, _T("italic")) == 0 ) {
							italic = (_tcscmp(pstrValue, _T("true")) == 0);
						}
						else if( _tcscmp(pstrName, _T("default")) == 0 ) {
							defaultfont = (_tcscmp(pstrValue, _T("true")) == 0);
						}
					}
					if( pFontName ) {
						pManager->AddFont(pFontName, size, bold, underline, italic);
						if( defaultfont ) pManager->SetDefaultFont(pFontName, size, bold, underline, italic);
					}
				}
				else if( _tcscmp(pstrClass, _T("Default")) == 0 ) {
					nAttributes = node.GetAttributeCount();
					LPCTSTR pControlName = NULL;
					LPCTSTR pControlValue = NULL;
					for( int i = 0; i < nAttributes; i++ ) {
						pstrName = node.GetAttributeName(i);
						pstrValue = node.GetAttributeValue(i);
						if( _tcscmp(pstrName, _T("name")) == 0 ) {
							pControlName = pstrValue;
						}
						else if( _tcscmp(pstrName, _T("value")) == 0 ) {
							pControlValue = pstrValue;
						}
					}
					if( pControlName ) {
						pManager->AddDefaultAttributeList(pControlName, pControlValue);
					}
				}
				else if( _tcscmp(pstrClass, _T("ActionScript")) == 0) {

					for(CMarkupNode AGroupNode = node.GetChild();AGroupNode.IsValid();AGroupNode = AGroupNode.GetSibling()){
						LPCTSTR pstrClass = AGroupNode.GetName();
						if( _tcscmp(pstrClass, _T("AGroup")) != 0 || !AGroupNode.HasAttribute(_T("name")) || !AGroupNode.HasAttribute(_T("msgtype")) || !AGroupNode.HasAttribute(_T("msgvalue")))
							continue;

						if(_tcscmp(_T("notify"),AGroupNode.GetAttributeValue(_T("msgtype"))) != 0 && 
							_tcscmp(_T("event"),AGroupNode.GetAttributeValue(_T("msgtype"))) != 0)
							continue;

						int  iDefaultInterval	= _ttoi(AGroupNode.GetAttributeValue(_T("defaultinterval")));
						int  iDefaultTimer		= _ttoi(AGroupNode.GetAttributeValue(_T("defaulttimer")));
						bool bDefaultAutoStart	= _tcscmp(_T("true"),AGroupNode.GetAttributeValue(_T("defaultautostart"))) == 0?true:false;
						bool bDefaultLoop		= _tcscmp(_T("true"),AGroupNode.GetAttributeValue(_T("defaultloop"))) == 0?true:false;
						bool bDefaultReverse	= _tcscmp(_T("true"),AGroupNode.GetAttributeValue(_T("defaultreverse"))) == 0?true:false;

						if(_tcscmp(_T("notify"),AGroupNode.GetAttributeValue(_T("msgtype"))) == 0){
							if(!pManager->AddActionScriptGroup(
								AGroupNode.GetAttributeValue(_T("name")),
								AGroupNode.GetAttributeValue(_T("msgvalue")),
								iDefaultInterval,iDefaultTimer,
								bDefaultReverse,bDefaultLoop,bDefaultAutoStart
								))
								continue;
						}
						else if(_tcscmp(_T("event"),AGroupNode.GetAttributeValue(_T("msgtype"))) == 0){
							if(!pManager->AddActionScriptGroup(
								AGroupNode.GetAttributeValue(_T("name")),
								(EVENTTYPE_UI)_ttoi(AGroupNode.GetAttributeValue(_T("msgvalue"))),
								iDefaultInterval,iDefaultTimer,
								bDefaultReverse,bDefaultLoop,bDefaultAutoStart
								))
								continue;
						}

						TAGroup* pTAGroup = pManager->GetActionScriptGroup(AGroupNode.GetAttributeValue(_T("name")));
						if(!pTAGroup)
							continue;

						for(CMarkupNode ActionNode = AGroupNode.GetChild();ActionNode.IsValid();ActionNode = ActionNode.GetSibling()){
							if( _tcscmp(ActionNode.GetName(), _T("Property")) != 0 
								|| !ActionNode.HasAttribute(_T("name")) 
								|| !ActionNode.HasAttribute(_T("type")) 
								|| (!ActionNode.HasAttribute(_T("startvalue")) || !ActionNode.HasAttribute(_T("endvalue")))
								|| _tcscmp(ActionNode.GetAttributeValue(_T("startvalue")),ActionNode.GetAttributeValue(_T("endvalue"))) == 0)
								continue;

							int  iInterval	= iDefaultInterval;
							int	 iTimer		= iDefaultTimer;
							int	 iDelay		= 0;
							bool bAutoStart	= bDefaultAutoStart;
							bool bLoop		= bDefaultLoop;
							bool bReverse	= bDefaultReverse;

							LPCTSTR sName		= ActionNode.GetAttributeValue(_T("name"));
							LPCTSTR sType		= ActionNode.GetAttributeValue(_T("type"));
							LPCTSTR sStartVal	= _T("none");
							LPCTSTR sEndVal		= _T("none");

							if(ActionNode.HasAttribute(_T("startvalue")))
								sStartVal	= ActionNode.GetAttributeValue(_T("startvalue"));
							if(ActionNode.HasAttribute(_T("endvalue")))
								sEndVal		= ActionNode.GetAttributeValue(_T("endvalue"));
							if(ActionNode.HasAttribute(_T("interval")))
								iInterval	= _ttoi(ActionNode.GetAttributeValue(_T("interval"))) <= 0 ?0:_ttoi(ActionNode.GetAttributeValue(_T("interval")));
							if(ActionNode.HasAttribute(_T("timer")))
								iTimer		= _ttoi(ActionNode.GetAttributeValue(_T("timer"))) <= 0 ?0:_ttoi(ActionNode.GetAttributeValue(_T("timer")));
							if(ActionNode.HasAttribute(_T("autostart")))
								bAutoStart	= _tcscmp(_T("true"),ActionNode.GetAttributeValue(_T("autostart"))) == 0?true:false;
							if(ActionNode.HasAttribute(_T("loop")))
								bLoop		= _tcscmp(_T("true"),ActionNode.GetAttributeValue(_T("loop"))) == 0?true:false;
							if(ActionNode.HasAttribute(_T("reverse")))
								bReverse	= _tcscmp(_T("true"),ActionNode.GetAttributeValue(_T("reverse"))) == 0?true:false;

							pManager->AddPropertyAction(*pTAGroup,sName,sType,sStartVal,sEndVal,iInterval,iTimer,iDelay,bReverse,bLoop,bAutoStart);
						}
					}
				}
				else if( _tcscmp(pstrClass, _T("Styles")) == 0) {

					nAttributes = node.GetAttributeCount();
					CDuiString pStrStylesName;
					bool nIsDefaultStyles = false;

					for( int i = 0; i < nAttributes; i++ ) {

						pstrName = node.GetAttributeName(i);
						pstrValue = node.GetAttributeValue(i);

						if( _tcscmp(pstrName, _T("name")) == 0 ){
							pStrStylesName = pstrValue;
						}
						else if( _tcscmp(pstrName, _T("default")) == 0 && _tcscmp(pstrValue, _T("true")) == 0 ){
							nIsDefaultStyles = true;
						}
						else if( _tcscmp(pstrName, _T("inherit")) == 0 && pstrValue ){
							if(CStdStringPtrMap* pStylesMap = pManager->GetControlsStyles(pstrValue))
							{
								for(int nIndex = 0;nIndex < pStylesMap->GetSize();nIndex++)
								{
									CDuiString nKey				= pStylesMap->GetAt(nIndex);
									CStdStringPtrMap* pStyleMap	= static_cast<CStdStringPtrMap*>(pStylesMap->Find(nKey.GetData()));
									if(pStyleMap)
									{
										for(int nSubIndex = 0;nSubIndex < pStyleMap->GetSize();nSubIndex++)
										{
											CDuiString nSubKey	= pStyleMap->GetAt(nSubIndex);
											CDuiString* nSubVal = static_cast<CDuiString*>(pStyleMap->Find(nSubKey.GetData()));
											pManager->SetControlStyle(nKey.GetData(),nSubKey.GetData(),nSubVal->GetData(),pStrStylesName.GetData());
										}
									}
								}
							}
						}
						else if( _tcscmp(pstrName, _T("stylefile")) == 0 ){
							CMarkup nStyleFile;
							if(nStyleFile.LoadFromFile(pstrValue)){
								for(CMarkupNode nStyleNode = nStyleFile.GetRoot().GetChild();nStyleNode.IsValid();nStyleNode = nStyleNode.GetSibling())
								{
									LPCTSTR pstrClass = nStyleNode.GetName();

									if( _tcscmp(pstrClass, _T("Style")) != 0 || !nStyleNode.HasAttribute(_T("stylename")))
										continue;

									CDuiString nControlStyleName = nStyleNode.GetAttributeValue(_T("stylename"));
									if(nControlStyleName.GetLength() <= 0)
										continue;
									
									int nStyleAttributes = nStyleNode.GetAttributeCount();
									for( int nIndex = 0; nIndex < nStyleAttributes; nIndex++ ) {
										pstrName = nStyleNode.GetAttributeName(nIndex);
										pstrValue = nStyleNode.GetAttributeValue(nIndex);
										if( _tcscmp(pstrName, _T("stylename")) == 0 )
											continue;
										pManager->SetControlStyle(nControlStyleName.GetData(),pstrName,pstrValue,pStrStylesName.GetData());
									}
								}
							}
						}
					}

					for(CMarkupNode nStyleNode = node.GetChild();nStyleNode.IsValid();nStyleNode = nStyleNode.GetSibling())
					{
						LPCTSTR pstrClass = nStyleNode.GetName();

						if( _tcscmp(pstrClass, _T("Style")) != 0 || !nStyleNode.HasAttribute(_T("stylename")))
							continue;

						CDuiString nControlStyleName = nStyleNode.GetAttributeValue(_T("stylename"));
						if(nControlStyleName.GetLength() <= 0)
							continue;

						int nStyleAttributes = nStyleNode.GetAttributeCount();
						for( int i = 0; i < nStyleAttributes; i++ ) {
							pstrName = nStyleNode.GetAttributeName(i);
							pstrValue = nStyleNode.GetAttributeValue(i);
							if( _tcscmp(pstrName, _T("stylename")) == 0 )
								continue;

							pManager->SetControlStyle(nControlStyleName.GetData(),pstrName,pstrValue,pStrStylesName.GetData());
						}
					}
					if(nIsDefaultStyles && !pStrStylesName.IsEmpty())
						pManager->SetCurStyles(pStrStylesName.GetData(),false);

					continue;
				}
				else if( _tcscmp(pstrClass, _T("EffectsStyles")) == 0 && node.HasChildren() ) {

					nAttributes = node.GetAttributeCount();

					for( int i = 0; i < nAttributes; i++ ) {
						pstrName = node.GetAttributeName(i);
						pstrValue = node.GetAttributeValue(i);

						if( _tcscmp(pstrName, _T("stylefile")) == 0 ){
							CMarkup mEffectsStyleFile;
							if(mEffectsStyleFile.LoadFromFile(pstrValue))
							{
								for( CMarkupNode nEffectsStyleNode = mEffectsStyleFile.GetRoot().GetChild(); nEffectsStyleNode.IsValid(); nEffectsStyleNode = nEffectsStyleNode.GetSibling() ) {
									LPCTSTR pstrClass = nEffectsStyleNode.GetName();

									if( _tcscmp(pstrClass, _T("EffectsStyle")) != 0)
										continue;

									nAttributes = nEffectsStyleNode.GetAttributeCount();
									LPCTSTR pControlName = NULL;
									LPCTSTR pControlValue = NULL;
									for( int i = 0; i < nAttributes; i++ ) {
										pstrName = nEffectsStyleNode.GetAttributeName(i);
										pstrValue = nEffectsStyleNode.GetAttributeValue(i);
										if( _tcscmp(pstrName, _T("name")) == 0 ) {
											pControlName = pstrValue;
										}
										else if( _tcscmp(pstrName, _T("value")) == 0 ) {
											pControlValue = pstrValue;
										}
									}
									if( pControlName ) {
										pManager->AddEffectsStyle(pControlName, pControlValue);
									}
								}
							}
							break;
						}
					}

					for( CMarkupNode nEffectsStyleNode = node.GetChild() ; nEffectsStyleNode.IsValid(); nEffectsStyleNode = nEffectsStyleNode.GetSibling() ) {
						LPCTSTR pstrClass = nEffectsStyleNode.GetName();

						if( _tcscmp(pstrClass, _T("EffectsStyle")) != 0)
							continue;

						nAttributes = nEffectsStyleNode.GetAttributeCount();
						LPCTSTR pControlName = NULL;
						LPCTSTR pControlValue = NULL;
						for( int i = 0; i < nAttributes; i++ ) {
							pstrName = nEffectsStyleNode.GetAttributeName(i);
							pstrValue = nEffectsStyleNode.GetAttributeValue(i);
							if( _tcscmp(pstrName, _T("name")) == 0 ) {
								pControlName = pstrValue;
							}
							else if( _tcscmp(pstrName, _T("value")) == 0 ) {
								pControlValue = pstrValue;
							}
						}
						if( pControlName ) {
							pManager->AddEffectsStyle(pControlName, pControlValue);
						}
					}
				}
			}

			pstrClass = root.GetName();
			if( _tcscmp(pstrClass, _T("Window")) == 0 ) {
				if( pManager->GetPaintWindow() ) {
					int nAttributes = root.GetAttributeCount();
					for( int i = 0; i < nAttributes; i++ ) {
						pstrName = root.GetAttributeName(i);
						pstrValue = root.GetAttributeValue(i);
						if( _tcscmp(pstrName, _T("size")) == 0 ) {
							LPTSTR pstr = NULL;
							int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
							pManager->SetInitSize(cx, cy);
						} 
						else if( _tcscmp(pstrName, _T("sizebox")) == 0 ) {
							RECT rcSizeBox = { 0 };
							LPTSTR pstr = NULL;
							rcSizeBox.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							rcSizeBox.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
							rcSizeBox.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
							rcSizeBox.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
							pManager->SetSizeBox(rcSizeBox);
						}
						else if( _tcscmp(pstrName, _T("caption")) == 0 ) {
							RECT rcCaption = { 0 };
							LPTSTR pstr = NULL;
							rcCaption.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							rcCaption.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
							rcCaption.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
							rcCaption.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
							pManager->SetCaptionRect(rcCaption);
						}
						else if( _tcscmp(pstrName, _T("roundcorner")) == 0 ) {
							LPTSTR pstr = NULL;
							int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
							pManager->SetRoundCorner(cx, cy);
						} 
						else if( _tcscmp(pstrName, _T("mininfo")) == 0 ) {
							LPTSTR pstr = NULL;
							int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
							pManager->SetMinInfo(cx, cy);
						}
						else if( _tcscmp(pstrName, _T("maxinfo")) == 0 ) {
							LPTSTR pstr = NULL;
							int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
							pManager->SetMaxInfo(cx, cy);
						}
						else if( _tcscmp(pstrName, _T("alpha")) == 0 ) {
							pManager->SetTransparent(_ttoi(pstrValue));
						} 
						else if( _tcscmp(pstrName, _T("bktrans")) == 0 ) {
							pManager->SetBackgroundTransparent(_tcscmp(pstrValue, _T("true")) == 0);
						} 
						else if( _tcscmp(pstrName, _T("disabledfontcolor")) == 0 ) {
							if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
							LPTSTR pstr = NULL;
							DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
							pManager->SetDefaultDisabledColor(clrColor);
						} 
						else if( _tcscmp(pstrName, _T("defaultfontcolor")) == 0 ) {
							if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
							LPTSTR pstr = NULL;
							DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
							pManager->SetDefaultFontColor(clrColor);
						}
						else if( _tcscmp(pstrName, _T("linkfontcolor")) == 0 ) {
							if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
							LPTSTR pstr = NULL;
							DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
							pManager->SetDefaultLinkFontColor(clrColor);
						} 
						else if( _tcscmp(pstrName, _T("linkhoverfontcolor")) == 0 ) {
							if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
							LPTSTR pstr = NULL;
							DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
							pManager->SetDefaultLinkHoverFontColor(clrColor);
						} 
						else if( _tcscmp(pstrName, _T("selectedcolor")) == 0 ) {
							if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
							LPTSTR pstr = NULL;
							DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
							pManager->SetDefaultSelectedBkColor(clrColor);
						}
						else if(_tcscmp(pstrName, _T("trayiconid")) == 0 ) {
							if(_ttoi(pstrValue) > 0)
								pManager->GetTrayObject().CreateTrayIcon(pManager->GetPaintWindow(),_ttoi(pstrValue));
						}
						else if(_tcscmp(pstrName, _T("traytiptext")) == 0 ) {
							pManager->GetTrayObject().SetTooltipText(pstrValue);
						}
						else if( _tcscmp(pstrName, _T("shadowsize")) == 0 ) {
							pManager->GetShadow()->SetSize(_ttoi(pstrValue));
						}
						else if( _tcscmp(pstrName, _T("shadowsharpness")) == 0 ) {
							pManager->GetShadow()->SetSharpness(_ttoi(pstrValue));
						}
						else if( _tcscmp(pstrName, _T("shadowdarkness")) == 0 ) {
							pManager->GetShadow()->SetDarkness(_ttoi(pstrValue));
						}
						else if( _tcscmp(pstrName, _T("shadowposition")) == 0 ) {
							LPTSTR pstr = NULL;
							int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
							pManager->GetShadow()->SetPosition(cx, cy);
						}
						else if( _tcscmp(pstrName, _T("shadowcolor")) == 0 ) {
							if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
							LPTSTR pstr = NULL;
							DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
							pManager->GetShadow()->SetColor(clrColor);
						}
						else if( _tcscmp(pstrName, _T("shadowcorner")) == 0 ) {
							RECT rcCorner = { 0 };
							LPTSTR pstr = NULL;
							rcCorner.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
							rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
							rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
							pManager->GetShadow()->SetShadowCorner(rcCorner);
						}
						else if( _tcscmp(pstrName, _T("shadowimage")) == 0 ) {
							pManager->GetShadow()->SetImage(pstrValue);
						}
						else if( _tcscmp(pstrName, _T("showshadow")) == 0 ) {
							pManager->GetShadow()->ShowShadow(_tcscmp(pstrValue, _T("true")) == 0);
						} 
						else if( _tcscmp(pstrName, _T("gdiplustext")) == 0 ) {
							pManager->SetUseGdiplusText(_tcscmp(pstrValue, _T("true")) == 0);
						} 
					}
				}
			}
		}
		if(NULL!=pManager && pManager->GetCurStylesName().IsEmpty() && pManager->GetStylesCount() > 0)
			pManager->SetCurStyles(0);
		return _Parse(&root, pParent, pManager);
	}

	CMarkup* CDialogBuilder::GetMarkup()
	{
		return &m_xml;
	}

	void CDialogBuilder::GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const
	{
		return m_xml.GetLastErrorMessage(pstrMessage, cchMax);
	}

	void CDialogBuilder::GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const
	{
		return m_xml.GetLastErrorLocation(pstrSource, cchMax);
	}

	CControlUI* CDialogBuilder::_Parse(CMarkupNode* pRoot, CControlUI* pParent, CPaintManagerUI* pManager)
	{
		IContainerUI* pContainer = NULL;
		CControlUI* pReturn = NULL;
		for( CMarkupNode node = pRoot->GetChild() ; node.IsValid(); node = node.GetSibling() ) {
			LPCTSTR pstrClass = node.GetName();
			if( _tcscmp(pstrClass, _T("Image")) == 0 || _tcscmp(pstrClass, _T("Font")) == 0 \
				|| _tcscmp(pstrClass, _T("Default")) == 0 || _tcscmp(pstrClass, _T("EffectsStyles")) == 0
				|| _tcscmp(pstrClass, _T("Styles")) == 0 || _tcscmp(pstrClass, _T("ActionScript")) == 0) continue;

			CControlUI* pControl = NULL;
			if( _tcscmp(pstrClass, _T("Include")) == 0 ) {
				if( !node.HasAttributes() ) continue;
				int count = 1;
				LPTSTR pstr = NULL;
				TCHAR szValue[500] = { 0 };
				SIZE_T cchLen = lengthof(szValue) - 1;
				if ( node.GetAttributeValue(_T("count"), szValue, cchLen) )
					count = _tcstol(szValue, &pstr, 10);
				cchLen = lengthof(szValue) - 1;
				if ( !node.GetAttributeValue(_T("source"), szValue, cchLen) ) continue;
				for ( int i = 0; i < count; i++ ) {
					CDialogBuilder builder;
					if( m_pstrtype != NULL ) { // 使用资源dll，从资源中读取
						WORD id = (WORD)_tcstol(szValue, &pstr, 10); 
						pControl = builder.Create((UINT)id, m_pstrtype, m_pCallback, pManager, pParent);
					}
					else {
						pControl = builder.Create((LPCTSTR)szValue, (UINT)0, m_pCallback, pManager, pParent);
					}
				}
				continue;
			}
			//树控件XML解析
			else if( _tcscmp(pstrClass, _T("TreeNode")) == 0 && pParent ) {
				CTreeNodeUI* pParentNode	= static_cast<CTreeNodeUI*>(pParent->GetInterface(_T("TreeNode")));
				CTreeNodeUI* pNode			= new CTreeNodeUI();
				if(pParentNode){
					if(!pParentNode->Add(pNode)){
						delete pNode;
						continue;
					}
				}

				// 若有控件默认配置先初始化默认属性
				if( pManager ) {
					pNode->SetManager(pManager, NULL, false);
					LPCTSTR pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass);
					if( pDefaultAttributes ) {
						pNode->ApplyAttributeList(pDefaultAttributes);
					}
				}

				// 解析所有XML属性并覆盖默认属性
				if( node.HasAttributes() ) {
					int nAttributes = node.GetAttributeCount();
					for( int i = 0; i < nAttributes; i++ ) {
						pNode->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
					}
				}

				//检索子节点及附加控件
				if(node.HasChildren())
					CControlUI* pSubControl = _Parse(&node,pNode,pManager);
				
				if(!pParentNode){
					CTreeViewUI* pTreeView = static_cast<CTreeViewUI*>(pParent->GetInterface(_T("TreeView")));
					ASSERT(pTreeView);
					if( pTreeView == NULL ) return NULL;
					if( !pTreeView->Add(pNode) ) {
						delete pNode;
						continue;
					}
				}
				continue;
			}
			else {
				SIZE_T cchLen = _tcslen(pstrClass);
				switch( cchLen ) {
				case 4:
					if( _tcscmp(pstrClass, DUI_CTR_EDIT) == 0 )                   pControl = new CEditUI;
					else if( _tcscmp(pstrClass, DUI_CTR_LIST) == 0 )              pControl = new CListUI;
					else if( _tcscmp(pstrClass, DUI_CTR_TEXT) == 0 )              pControl = new CTextUI;
					break;
				case 5:
					if( _tcscmp(pstrClass, DUI_CTR_COMBO) == 0 )                  pControl = new CComboUI;
					else if( _tcscmp(pstrClass, DUI_CTR_LABEL) == 0 )             pControl = new CLabelUI;
					else if( _tcscmp(pstrClass, DUI_CTR_FLASH) == 0 )             pControl = new CFlashUI;
					break;
				case 6:
					if( _tcscmp(pstrClass, DUI_CTR_BUTTON) == 0 )                 pControl = new CButtonUI;
					else if( _tcscmp(pstrClass, DUI_CTR_OPTION) == 0 )            pControl = new COptionUI;
					else if( _tcscmp(pstrClass, DUI_CTR_SLIDER) == 0 )            pControl = new CSliderUI;
					break;
				case 7:
					if( _tcscmp(pstrClass, DUI_CTR_CONTROL) == 0 )                pControl = new CControlUI;
					else if( _tcscmp(pstrClass, DUI_CTR_ACTIVEX) == 0 )           pControl = new CActiveXUI;
					else if( _tcscmp(pstrClass, DUI_CTR_GIFANIM) == 0 )           pControl = new CGifAnimUI;
					break;
				case 8:
					if( _tcscmp(pstrClass, DUI_CTR_PROGRESS) == 0 )               pControl = new CProgressUI;
					else if( _tcscmp(pstrClass, DUI_CTR_RICHEDIT) == 0 )          pControl = new CRichEditUI;
					else if( _tcscmp(pstrClass, DUI_CTR_CHECKBOX) == 0 )		  pControl = new CCheckBoxUI;
					else if( _tcscmp(pstrClass, DUI_CTR_COMBOBOX) == 0 )		  pControl = new CComboBoxUI;
					else if( _tcscmp(pstrClass, DUI_CTR_DATETIME) == 0 )		  pControl = new CDateTimeUI;
					else if( _tcscmp(pstrClass, DUI_CTR_TREEVIEW) == 0 )		  pControl = new CTreeViewUI;
					else if( _tcscmp(pstrClass, DUI_CTR_TREENODE) == 0 )		  pControl = new CTreeNodeUI;
					break;
				case 9:
					if( _tcscmp(pstrClass, DUI_CTR_CONTAINER) == 0 )              pControl = new CContainerUI;
					else if( _tcscmp(pstrClass, DUI_CTR_TABLAYOUT) == 0 )         pControl = new CTabLayoutUI;
					else if( _tcscmp(pstrClass, DUI_CTR_SCROLLBAR) == 0 )         pControl = new CScrollBarUI; 
					else if( _tcscmp(pstrClass, DUI_CTR_CHARTVIEW) == 0 )         pControl = new CChartViewUI; 
					break;
				case 10:
					if( _tcscmp(pstrClass, DUI_CTR_LISTHEADER) == 0 )             pControl = new CListHeaderUI;
					else if( _tcscmp(pstrClass, DUI_CTR_TILELAYOUT) == 0 )        pControl = new CTileLayoutUI;
					else if( _tcscmp(pstrClass, DUI_CTR_FADEBUTTON) == 0 )        pControl = new CFadeButtonUI;					
					else if( _tcscmp(pstrClass, DUI_CTR_WEBBROWSER) == 0 )        pControl = new CWebBrowserUI;
					break;
				case 11:
					if (_tcscmp(pstrClass, DUI_CTR_CHILDLAYOUT) == 0)			  pControl = new CChildLayoutUI;
					break;
				case 14:
					if( _tcscmp(pstrClass, DUI_CTR_VERTICALLAYOUT) == 0 )         pControl = new CVerticalLayoutUI;
					else if( _tcscmp(pstrClass, DUI_CTR_LISTHEADERITEM) == 0 )    pControl = new CListHeaderItemUI;
					break;
				case 15:
					if( _tcscmp(pstrClass, DUI_CTR_LISTTEXTELEMENT) == 0 )        pControl = new CListTextElementUI;
					break;
				case 16:
					if( _tcscmp(pstrClass, DUI_CTR_HORIZONTALLAYOUT) == 0 )       pControl = new CHorizontalLayoutUI;
					else if( _tcscmp(pstrClass, DUI_CTR_LISTLABELELEMENT) == 0 )  pControl = new CListLabelElementUI;
					break;
				case 18:
					if( _tcscmp(pstrClass, DUI_CTR_ANIMATIONTABLAYOUT) == 0 )     pControl = new CAnimationTabLayoutUI;
					break;
				case 20:
					if( _tcscmp(pstrClass, DUI_CTR_LISTCONTAINERELEMENT) == 0 )   pControl = new CListContainerElementUI;
					else if( _tcscmp(pstrClass, DUI_CTR_LISTIMAGETEXTELEMENT) == 0 )   pControl = new CListImageTextElementUI;					
					break;
				}

				// User-supplied control factory
				if( pControl == NULL ) {
					CStdPtrArray* pPlugins = CPaintManagerUI::GetPlugins();
					LPCREATECONTROL lpCreateControl = NULL;
					for( int i = 0; i < pPlugins->GetSize(); ++i ) {
						lpCreateControl = (LPCREATECONTROL)pPlugins->GetAt(i);
						if( lpCreateControl != NULL ) {
							pControl = lpCreateControl(pstrClass);
							if( pControl != NULL ) break;
						}
					}
				}
				if( pControl == NULL && m_pCallback != NULL ) {
					pControl = m_pCallback->CreateControl(pstrClass);
				}
			}

#ifndef _DEBUG
			ASSERT(pControl);
#endif // _DEBUG
			if( pControl == NULL )
			{
#ifdef _DEBUG
				DUITRACE(_T("未知控件:%s"),pstrClass);
#else
				continue;
#endif
			}

			// Add children
			if( node.HasChildren() ) {
				_Parse(&node, pControl, pManager);
			}
			// Attach to parent
			// 因为某些属性和父窗口相关，比如selected，必须先Add到父窗口
			if( pParent != NULL ) {
				CTreeNodeUI* pContainerNode = static_cast<CTreeNodeUI*>(pParent->GetInterface(_T("TreeNode")));
				if(pContainerNode)
					pContainerNode->GetTreeNodeHoriznotal()->Add(pControl);
				else
				{
					if( pContainer == NULL ) pContainer = static_cast<IContainerUI*>(pParent->GetInterface(_T("IContainer")));
					ASSERT(pContainer);
					if( pContainer == NULL ) return NULL;
					if( !pContainer->Add(pControl) ) {
						delete pControl;
						continue;
					}
				}
			}
			// Init default attributes
			if( pManager ) {
				pControl->SetManager(pManager, NULL, false);
				LPCTSTR pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass);
				if( pDefaultAttributes ) {
					pControl->ApplyAttributeList(pDefaultAttributes);
				}
			}
			// 若定义了属性模版，则初始化模版属性，将覆盖Default定义的默认属性
			if( node.HasAttributes() ) {
				TCHAR szValue[500] = { 0 };
				SIZE_T cchLen = lengthof(szValue) - 1;
				// Set ordinary attributes
				int nAttributes = node.GetAttributeCount();
				
				CDuiString szName = node.GetAttributeValue(_T("name"));

				pControl->SetStyleName(node.GetName());
				
				if(node.HasAttribute(_T("name")) && szName.GetLength() > 0)
					pControl->SetStyleName(szName.GetData());

				for( int i = 0; i < nAttributes; i++ ) {
					if(_tcscmp(node.GetAttributeName(i),_T("style")) == 0){
						pControl->SetStyleName(node.GetAttributeValue(i),pManager);
						break;
					}
				}
			}
			// Process attributes
			if( node.HasAttributes() ) {
				TCHAR szValue[500] = { 0 };
				SIZE_T cchLen = lengthof(szValue) - 1;
				// Set ordinary attributes
				int nAttributes = node.GetAttributeCount();
				for( int i = 0; i < nAttributes; i++ ) {
					if(_tcscmp(node.GetAttributeName(i),_T("style")) == 0){
						continue;
					}
					pControl->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
				}
			}
			if( pManager ) {
				pControl->SetManager(NULL, NULL, false);
			}
			// Return first item
			if( pReturn == NULL ) pReturn = pControl;
		}
		return pReturn;
	}

} // namespace UiLib
