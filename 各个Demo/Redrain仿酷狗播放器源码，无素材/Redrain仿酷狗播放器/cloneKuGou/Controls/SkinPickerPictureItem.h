//====================================================================
//  这是一个duilib中自定义控件的简单例子，可以让duilib新手朋友学习怎么做自定义控件
//  此类用来做酷狗换肤模块里的每一个背景图项目的控件，用到了自定义属性
//  如有疑问，可以联系我：Redrain  QQ：491646717
//  如果有类似的需求，可以修改这个类
//====================================================================


#ifndef SKIN_PICKER_PICTURE_ITEM_H
#define SKIN_PICKER_PICTURE_ITEM_H

//xml sample:<SkinPikerPictureItem name="" width="118" height="70" bkimage="UI\BKImage\1small.jpg" bkname="测试" author="Redrain" />
//类名和接口名，在CreateControl函数中会用到
const TCHAR kSkinPickerPictureItemClassName[] =	_T("SkinPikerPictureItemUI");
const TCHAR kSkinPickerPictureItemInterface[] =	_T("SkinPikerPictureItem");

//黑色的前景图的位置
const TCHAR kSkinPickerPictureItemForeImage[] =	_T("file='UI\\LeftTab\\listitem\\ListBk.png' fade='150'");

//边框的颜色、图片名称的文字颜色、作者信息的文字颜色
const DWORD kBorderColor					  = 0xFF64B0FA;
const DWORD kBkNameColor					  = 0xFFFFFFFF;
const DWORD kAuthorColor					  = 0xFFAAAAAA;

class CSkinPikerPictureItemUI : public CButtonUI
{
public:
	CSkinPikerPictureItemUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void PaintStatusImage(HDC hDC);

private:
	CDuiString m_BkName;
	CDuiString m_Author;
};

#endif // SKIN_PICKER_PICTURE_ITEM_H