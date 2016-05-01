#ifndef __THIRD_PARTY_H__
#define __THIRD_PARTY_H__

#pragma once

namespace DuiLib
{
	namespace ThirdParty
	{
		BYTE* ParseImage(BYTE const *buffer, int len, OUT int &width, OUT int &height);
		void  FreeImage(void *pImage);

		// 如果flags为2，表示从文件加载，指定buffer为文件名，不需要指定len
		// 如果flags为3，表示从内存加载，指定buffer为内存块,len为buffer长度
		HANDLE OpenZip(void *buffer, unsigned int len, DWORD flags);
		void CloseZip(HANDLE hz);

		// strFilePath:相对皮肤文件夹的文件路径
		BYTE* LoadFromFile(LPCTSTR strFilePath, OUT DWORD &dwFileSize);

		// strFilePath:相对皮肤文件夹的文件路径
		BYTE* LoadFromZip(LPCTSTR strFilePath, OUT DWORD &dwFileSize);

		// strFilePath:文件绝对路径
		BYTE* LoadFromAbsoluteFile(LPCTSTR strFilePath, OUT DWORD &dwFileSize);

		// strResName:资源名字；resType:资源类型
		BYTE* LoadFromResource(LPCTSTR strResName, LPCTSTR resType, OUT DWORD &dwFileSize);
	}
}// namespace DuiLib

#endif // __THIRD_PARTY_H__