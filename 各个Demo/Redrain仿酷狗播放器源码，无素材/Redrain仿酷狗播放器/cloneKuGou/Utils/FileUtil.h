#pragma once
#include <vector>
using namespace std;

class CFileUtil
{
public:
	CFileUtil(void);
	~CFileUtil(void);
public:
	struct FILE_STRUCT
	{		
		CDuiString fileName;
		CDuiString name;
		CDuiString exName;
		int        size;
	};

	//打开对话框 lpstrFilter：过滤字符串   hwndOwner：父窗口  fileNames：完整文件路径
	static BOOL OpenFile(LPCWSTR lpstrFilter, HWND hwndOwner,vector<CDuiString> &fileNames, bool IsMulti = true);

	//浏览文件夹 path：路径  hwndOwner:父窗口  tile:窗口标题
	static BOOL BrowseDir(CDuiString &path,HWND hwndOwner,CDuiString title);

	//遍历文件夹内的所有文件 path：路径  lpstrFilter：过滤字符串   files：文件
	static BOOL BrowseAllFile(CDuiString path,LPCWSTR lpstrFilter,vector<FILE_STRUCT> &files);

	//打开一个路径并选中一个文件
	static void OpenDir(HWND hwndOwner,CDuiString path,CDuiString fileName);

	//把一个完整文件名拆分成，文件名、扩展名
	static void SplitFileName(CDuiString fileName,CDuiString &name,CDuiString &exName);

	//检查文件是否存在
	static BOOL CheckFileExist(CDuiString pathFileName);

	//检查文件后缀名 pstrPath：文件路径 pstrExtFilter：过滤列表
	static bool FindFileExt(LPCTSTR pstrPath, LPCTSTR pstrExtFilter);

	//递归遍历当前目录内文件文件
	static void EnumerateFiles(vector<CDuiString> &vctString);

	//得到文件的大小
	static DWORD GetFileSize(LPCTSTR fileName);

	//文件大小转换为字符换： xx.xxM
	static LPCTSTR FileSizeToString(DWORD dwSize);

};

