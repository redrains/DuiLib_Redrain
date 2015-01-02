#include "..\duilib.h"
#include <commdlg.h>
#include <shlobj.h>
#include <shellapi.h>


CFileUtil::CFileUtil(void)
{
}


CFileUtil::~CFileUtil(void)
{
}

BOOL CFileUtil::OpenFile(LPCWSTR lpstrFilter, HWND hwndOwner, vector<CDuiString> &fileNames, bool IsMulti)
{
	DWORD dwFlag = IsMulti ? OFN_ALLOWMULTISELECT : 0;
	TCHAR szFileName[MAX_PATH * 101 + 1] = _T("");

	OPENFILENAME openfilename = {0};

	ZeroMemory(&openfilename, sizeof(OPENFILENAME));

	CDuiString s_title;

	openfilename.lStructSize       = sizeof(OPENFILENAME);
	openfilename.hwndOwner         = hwndOwner;
	openfilename.hInstance         = NULL;
	openfilename.lpstrFilter       = lpstrFilter;
	openfilename.lpstrCustomFilter = NULL;
	openfilename.nMaxCustFilter    = 0L;
	openfilename.nFilterIndex      = 1L;
	openfilename.lpstrFile         = szFileName;
	openfilename.nMaxFile          = MAX_PATH * 101 + 1;
	openfilename.lpstrFileTitle    = NULL;
	openfilename.nMaxFileTitle     = 0;
	openfilename.lpstrInitialDir   = NULL ;
	openfilename.lpstrTitle        = s_title;
	openfilename.nFileOffset       = 0;
	openfilename.nFileExtension    = 0;
	openfilename.lpstrDefExt       = _T("*.*");
	openfilename.lCustData         = 0;
	openfilename.Flags             = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_READONLY | OFN_EXPLORER | dwFlag;
	
	// 弹出打开文件的对话框
	CDuiString str;

	if (::GetOpenFileName(&openfilename))
	{
		LPTSTR p = szFileName;
		CDuiString TempPath;
		if (*p != NULL)
		{
			TempPath =  p;
			p    += TempPath.GetLength() + 1;
		}

		if (*p == NULL)
		{
		//	TempPath = TempPath.Left(TempPath.ReverseFind(L'\\'));
			fileNames.push_back(TempPath);
		}


		while (*p != NULL)
		{
			CDuiString str = p;

			p += str.GetLength() + 1;

			fileNames.push_back(TempPath + _T("\\") + str);
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

BOOL CFileUtil::BrowseDir(CDuiString &path,HWND hwndOwner,CDuiString title)
{
	TCHAR szPathName[MAX_PATH]; 
	BROWSEINFO bInfo={0};  
	bInfo.hwndOwner=hwndOwner;//父窗口  
	bInfo.lpszTitle=title;  
	bInfo.ulFlags=BIF_RETURNONLYFSDIRS |BIF_USENEWUI/*包含一个编辑框 用户可以手动填写路径 对话框可以调整大小之类的..*/|  
		BIF_UAHINT/*带TIPS提示*/ |BIF_NONEWFOLDERBUTTON /*不带新建文件夹按钮*/;  
	LPITEMIDLIST lpDlist;  
	lpDlist=SHBrowseForFolder(&bInfo);  
	if (lpDlist!=NULL)//单击了确定按钮  
	{  
		SHGetPathFromIDList(lpDlist,szPathName); 
		path.Format(_T("%s"),szPathName);  
		return TRUE;
	}else
	{
		return FALSE;
	}
}

BOOL CFileUtil::BrowseAllFile(CDuiString path,LPCWSTR lpstrFilter,vector<FILE_STRUCT> &files)
{
	vector<CDuiString> filters;
	CDuiString filter;
	if (lpstrFilter!=NULL)
	{
		LPCWSTR p = lpstrFilter;
		while (*p!=NULL)
		{
			filter = p;
			p+=filter.GetLength()+1;
			filters.push_back(filter);
		}

	}

	FILE_STRUCT file;
	WIN32_FIND_DATA fd;  
	HANDLE fHandle=FindFirstFile(path,&fd);
	if (fHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	file.fileName = fd.cFileName;
	file.size = fd.nFileSizeLow-fd.nFileSizeHigh;
	file.name = file.fileName.Left(file.fileName.Find(L"."));
	file.exName = file.fileName.Right(file.fileName.GetLength()-file.name.GetLength()-1);

	if (lpstrFilter!=NULL)
	{
		for (UINT i =0;i<filters.size();i++)
		{
			if (filters[i] == file.exName)
			{
				files.push_back(file);
			}
		}

	}else
	{
		files.push_back(file);
	}


	if(fHandle!=INVALID_HANDLE_VALUE)  
	{  
		while(FindNextFile(fHandle,&fd))  
		{  
			file.fileName = fd.cFileName;
			file.size = fd.nFileSizeLow-fd.nFileSizeHigh;
			file.name = file.fileName.Left(file.fileName.Find(L"."));
			file.exName = file.fileName.Right(file.fileName.GetLength()-file.name.GetLength()-1);
			if (lpstrFilter!=NULL)
			{
				for (UINT i =0;i<filters.size();i++)
				{
					if (filters[i] == file.exName)
					{
						files.push_back(file);
					}
				}

			}else
			{
				files.push_back(file);
			}
		}  
	}  

	return TRUE;
}

void CFileUtil::OpenDir(HWND hwndOwner,CDuiString path,CDuiString fileName)
{
	CDuiString str;
	str.Format(L"/select,%s",path);
	str.Append(L"\\");
	str.Append(fileName);
	ShellExecute(hwndOwner,_T("open"),_T("Explorer.exe"),str,NULL,SW_SHOWNORMAL);
}

void CFileUtil::SplitFileName(CDuiString fileName,CDuiString &name,CDuiString &exName)
{
	name = fileName.Left(fileName.Find(L"."));
	exName = fileName.Right(fileName.GetLength()-name.GetLength()-1);
}

BOOL CFileUtil::CheckFileExist(CDuiString pathFileName)
{
	WIN32_FIND_DATA fd; 
	HANDLE hFind = FindFirstFile(pathFileName, &fd); 
	if (hFind==INVALID_HANDLE_VALUE)
		return FALSE;
	else
		return TRUE;
}


bool CFileUtil::FindFileExt(LPCTSTR pstrPath, LPCTSTR pstrExtFilter)
{
	if (! pstrPath || ! pstrExtFilter)
	{
		return false;
	}

	TCHAR szExt[_MAX_EXT] = _T("");

	_tsplitpath_s(pstrPath, NULL, 0, NULL, 0, NULL, 0, szExt, _MAX_EXT);
	_tcslwr_s(szExt, _MAX_EXT);

	if(_tcslen(szExt))  
	{
		_tcscat_s(szExt, _MAX_EXT, _T(";"));    // .mo不符合条件，由于会匹配到.mov，所以在后面加上【;】来判断是否完全匹配
		return NULL != _tcsstr(pstrExtFilter, szExt);
	}

	return false;
}

void CFileUtil::EnumerateFiles(vector<CDuiString> &vctString)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(_T("*.*"), &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do 
		{
			// 如果为目录
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(fd.cFileName, _T(".")) && _tcscmp(fd.cFileName, _T("..")))
				{
					::SetCurrentDirectory(fd.cFileName);    
					EnumerateFiles(vctString);              
					::SetCurrentDirectory(_T(".."));        
				}
			}
			// 如果为文件
			else
			{
				CDuiString strDir;
				TCHAR      lpDir[MAX_PATH];

				::GetCurrentDirectory(MAX_PATH, lpDir);
				strDir = lpDir;
				if ( strDir.Right(1) != _T("\\") )
				{
					strDir += _T("\\");
				}
				strDir += fd.cFileName;
				if (CFileUtil::FindFileExt(strDir.GetData(), STR_SUPPORT_FILE_EXT))
					vctString.push_back(strDir.GetData());
			}
		} while (::FindNextFile(hFind, &fd));

		::FindClose(hFind);
	}

}

DWORD CFileUtil::GetFileSize(LPCTSTR fileName)
{
	HANDLE hFile = ::CreateFile(fileName, 0, 0, NULL,OPEN_EXISTING, 0, NULL);
	DWORD dwSize = ::GetFileSize(hFile,0);
	if (dwSize != INVALID_FILE_SIZE)
	{
		return dwSize;
	}
	else
		return 0;

	CloseHandle(hFile);
	
}

LPCTSTR CFileUtil::FileSizeToString(DWORD dwSize)
{
	TCHAR* strSize = new TCHAR[20];
	ZeroMemory(strSize,sizeof(TCHAR) * 20);
	_stprintf_s(strSize,20,_T("%.2f M"),float(dwSize) / 1024 /1024);
	return strSize;
}

