#include "stdafx.h"
#include "stb_image.h"
#include "ThirdParty.h"

DECLARE_HANDLE(HZIP);	// An HZIP identifies a zip file that has been opened
typedef DWORD ZRESULT;
typedef struct
{
	int index;                 // index of this file within the zip
	char name[MAX_PATH];       // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime, ctime, mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRY;
typedef struct
{
	int index;                 // index of this file within the zip
	TCHAR name[MAX_PATH];      // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime, ctime, mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRYW;

extern HZIP OpenZipU(void *z, unsigned int len, DWORD flags);
extern ZRESULT CloseZipU(HZIP hz);

#ifdef _UNICODE
#define ZIPENTRY ZIPENTRYW
#define GetZipItem GetZipItemW
#define FindZipItem FindZipItemW
#else
#define GetZipItem GetZipItemA
#define FindZipItem FindZipItemA
#endif
extern ZRESULT GetZipItemA(HZIP hz, int index, ZIPENTRY *ze);
extern ZRESULT GetZipItemW(HZIP hz, int index, ZIPENTRYW *ze);
extern ZRESULT FindZipItemA(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze);
extern ZRESULT FindZipItemW(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *ze);
extern ZRESULT UnzipItem(HZIP hz, int index, void *dst, unsigned int len, DWORD flags);
///////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	extern unsigned char *stbi_load_from_memory(unsigned char const *buffer, int len, int *x, int *y, \
		int *comp, int req_comp);
	extern void     stbi_image_free(void *retval_from_stbi_load);

};

namespace DuiLib
{
	namespace ThirdParty
	{
		BYTE* ThirdParty::ParseImage(BYTE const *buffer, int len, OUT int &width, OUT int &height)
		{
			int n;
			return stbi_load_from_memory(buffer, len, &width, &height, &n, STBI_rgb_alpha);
		}

		void FreeImage(void *pImage)
		{
			stbi_image_free(pImage);
		}

		HANDLE OpenZip(void *buffer, unsigned int len, DWORD flags)
		{
			return (HANDLE)OpenZipU(buffer, len, flags);
		}

		void CloseZip(HANDLE hz)
		{
			CloseZipU((HZIP)hz);
		}

		BYTE* LoadFromFile(LPCTSTR strFilePath, OUT DWORD &dwFileSize)
		{
			CDuiString sFile = CPaintManagerUI::GetResourcePath();
			sFile += strFilePath;

			HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
				FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
				return NULL;

			dwFileSize = ::GetFileSize(hFile, NULL);
			if (dwFileSize == 0) 
				return NULL;

			DWORD dwRead = 0;
			LPBYTE pData = new BYTE[dwFileSize];
			::ReadFile(hFile, pData, dwFileSize, &dwRead, NULL);
			::CloseHandle(hFile);

			if (dwRead != dwFileSize)
			{
				delete[] pData;
				return NULL;
			}

			return pData;
		}

		BYTE* LoadFromZip(LPCTSTR strFilePath, OUT DWORD &dwFileSize)
		{
			CDuiString sFile = CPaintManagerUI::GetResourcePath();
			sFile += CPaintManagerUI::GetResourceZip();

			HZIP hz = NULL;
			if (CPaintManagerUI::IsCachedResourceZip())
				hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
			else
				hz = OpenZipU((void*)sFile.GetData(), 0, 2);
			if (hz == NULL)
				return NULL;

			ZIPENTRY ze;
			int i;
			if (FindZipItem(hz, strFilePath, true, &i, &ze) != 0)
				return NULL;

			dwFileSize = ze.unc_size;
			if (dwFileSize == 0) 
				return NULL;

			LPBYTE pData = new BYTE[dwFileSize];
			int res = UnzipItem(hz, i, pData, dwFileSize, 3);
			if (res != 0x00000000 && res != 0x00000600) 
			{
				delete[] pData;
				pData = NULL;
				if (!CPaintManagerUI::IsCachedResourceZip()) 
					CloseZipU(hz);

				return NULL;
			}
			if (!CPaintManagerUI::IsCachedResourceZip()) 
				CloseZipU(hz);

			return pData;
		}

		BYTE* LoadFromResource(LPCTSTR strResName, LPCTSTR resType, OUT DWORD &dwFileSize)
		{
			HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), strResName, resType);
			if (hResource == NULL) 
				return NULL;
			HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
			if (hGlobal == NULL)
			{
				FreeResource(hResource);
				return NULL;
			}

			dwFileSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
			if (dwFileSize == 0)
				return NULL;
			LPBYTE pData = new BYTE[dwFileSize];
			::CopyMemory(pData, (LPBYTE)::LockResource(hGlobal), dwFileSize);
			::FreeResource(hResource);

			return pData;
		}

		BYTE* LoadFromAbsoluteFile(LPCTSTR strFilePath, OUT DWORD &dwFileSize)
		{
			HANDLE hFile = ::CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
				FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) 
				return NULL;
			dwFileSize = ::GetFileSize(hFile, NULL);
			if (dwFileSize == 0) 
				return NULL;

			DWORD dwRead = 0;
			LPBYTE pData = new BYTE[dwFileSize];
			::ReadFile(hFile, pData, dwFileSize, &dwRead, NULL);
			::CloseHandle(hFile);

			if (dwRead != dwFileSize) 
			{
				delete[] pData;
				pData = NULL;
			}
			
			return pData;
		}
	}
} // namespace DuiLib