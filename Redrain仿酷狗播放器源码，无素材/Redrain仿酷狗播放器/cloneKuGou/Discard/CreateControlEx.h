#ifndef CREATE_CONTROL_EX_H
#define CREATE_CONTROL_EX_H

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass) 
	{
		if (_tcsicmp(pstrClass, kMusicListItemUIInferFace) == 0)
			return	new CMusicListItemUI();
		return NULL;
	}
};


#endif CREATE_CONTROL_EX_H