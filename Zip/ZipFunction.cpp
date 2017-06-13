

/*******************************************************************************
**  AREA1  :  include header files area
*******************************************************************************/
#include <io.h>
#include <ShlObj.h>
#include <vector>
#include <tchar.h>
#include <atlconv.h>
#include <iostream>
#include "zip/ZipDefine.h"
#include "ZipFunction.h"

/*******************************************************************************
**  AREA2 : struture & macro & static-variables & others area
*******************************************************************************/

/* ++++++++++  static-vals sub-area  ++++++++++++++++++++++++++++++++++++++++ */
static std::vector<std::wstring> inZipPath;													//���ڴ洢�ļ������ZIP��Ŀ¼��·��

/*******************************************************************************
**  AREA3  :   function-bodies area
*******************************************************************************/


/*******************************************************************************
*
*                CODE   FUNCTION    SPECIFICATION
*
********************************************************************************
*
*   FUNCTION NAME: ExtractZipToDir
*   Inputs       : unzipSrcPath		zip�ļ�·��
*   Inputs       : unzipDestPath	��ѹ·��	�������NULL ���ѹ����ǰĿ¼
*   Retrun Value : ZRESULT			������룬�����ؽ��Ϊ0ʱ����Ϊ�ɹ�
*
*---------------------------------- PURPOSE -----------------------------------
*  ��zip�ļ����н�ѹ
*******************************************************************************/
ZRESULT ExtractZipToDir(LPCTSTR unzipSrcPath, LPCTSTR unzipDestPath)
{

#ifndef _UNICODE 
	// �ж��ļ��Ƿ����
	if (_access(unzipSrcPath, 0))
	{
		printf("ԴĿ¼�����ڣ���ѹʧ��\n", unzipSrcPath);
		return ZR_NOFILE;
	}
#else
	if (_waccess(unzipSrcPath, 0))
	{

		printf("ԴĿ¼�����ڣ���ѹʧ��\n");
		return ZR_NOFILE;
	}
#endif

	DWORD zResult = ZR_OK;																	//����ֵ
	
	std::wstring strunzipDestPath;															//���ڼ�¼��ѹĿ��Ŀ¼
	if (unzipDestPath)																		// �����ѹ·���������ȴ���, ���ڲ����κ��޸�
	{
		strunzipDestPath = unzipDestPath;
		SHCreateDirectoryEx(NULL, unzipDestPath, NULL);
	}
	else
	{
		TCHAR buffer[MAX_PATH] = { 0 };
		GetCurrentDirectory(MAX_PATH, (LPTSTR)&buffer);
		strunzipDestPath = buffer;
	}

	HZIP hz = OpenZip(unzipSrcPath, 0);														//zip����ʼ
	ZIPENTRY ze;

	GetZipItem(hz, -1, &ze);																//��ȡβ�ڵ���Ϣ
	int numitems = ze.index;
	for (int zi = 0; zi < numitems; zi++)
	{
		ZIPENTRY zb;
		GetZipItem(hz, zi, &zb);
		std::wstring tempPath = strunzipDestPath + _T("\\") + std::wstring(zb.name);
		zResult = UnzipItem(hz, zi, tempPath.c_str());
		if (zResult != ZR_OK)
		{
			break;
		}

	}
	CloseZip(hz);																			//�������
	printf("��ѹ%s\n", (zResult == 0 ? "�ɹ�" : "ʧ��"));									//Debug
	return zResult;
}

/*******************************************************************************
*
*                CODE   FUNCTION    SPECIFICATION
*
********************************************************************************
*
*   FUNCTION NAME: GetDirPath
*   Inputs       : vec				��ѹԴ�ļ���
*   Retrun Value : ret				��ѹĿ���ļ�������
*
*---------------------------------- PURPOSE -----------------------------------
*  ͨ��vectorƴ�Ӷ༶�ļ�������Ϊ·������
*******************************************************************************/
std::wstring GetDirPath(std::vector<std::wstring>& vec)
{
	std::wstring ret = _T("");
	for (int i = 0; i < vec.size(); i++)
	{
		ret = ret + vec[i];
		if (i != (vec.size() - 1))
		{
			ret = ret + _T("\\");
		}
	}
	return ret;
}

/*******************************************************************************
*
*                CODE   FUNCTION    SPECIFICATION
*
********************************************************************************
*
*   FUNCTION NAME: DirToZip
*   Inputs       : srcPath				ѹ��Դ�ļ�������
*   Inputs		 : destPath				ѹ��Ŀ���ļ�����				�����ΪNULL��ѹ����Դ�ļ�����ͬ��Ŀ��λ��ZIP��
*	Outputs		 : hz					zip�ṹָ��
*   Retrun Value : ZRESULT				������룬�����ؽ��Ϊ0ʱ����Ϊ�ɹ�
*---------------------------------- PURPOSE -----------------------------------
*   �ݹ��ѹ
*******************************************************************************/
ZRESULT DirToZip(LPCTSTR srcPath, LPCTSTR destPath, HZIP& hz)
{

	DWORD zResult = ZR_OK;
	WIN32_FIND_DATA fileData;
	std::wstring formatPath = std::wstring(srcPath) + _T("\\*.*");
	HANDLE file = FindFirstFile(formatPath.c_str(), &fileData);

	FindNextFile(file, &fileData);																//ͨ��ɨ���ȡ�ļ���Ϣ
	while (FindNextFile(file, &fileData))
	{
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)								//�������һ���ļ���
		{
			inZipPath.push_back(fileData.cFileName);											//���뵽ѹ�����ļ���·����
			std::wstring dirPath = GetDirPath(inZipPath);
			ZipAddFolder(hz, dirPath.c_str());													//����ļ���
			std::wstring subPath = std::wstring(srcPath) + _T("\\") + fileData.cFileName;		//ƴ����·��
			DirToZip(subPath.c_str(), destPath, hz);											//�ݹ���� �����鿴���ļ���
			inZipPath.pop_back();
		}
		else
		{
			std::wstring zipRelativePath = GetDirPath(inZipPath) + _T("\\") + fileData.cFileName;	//ѹ�����λ��
			std::wstring filesPath = std::wstring(srcPath) + _T("\\") + fileData.cFileName;			//�ļ�·��
			ZipAdd(hz, zipRelativePath.c_str(), filesPath.c_str());									//���浽��ǰ·����
			if (zResult != ZR_OK)
			{
				return zResult;
			}
		}
	}

	return zResult;


}

/*******************************************************************************
*
*                CODE   FUNCTION    SPECIFICATION
*
********************************************************************************
*
*   FUNCTION NAME: CompressDirToZip
*   Inputs       : zipSrcPath				ѹ��Դ�ļ�������
*   Inputs		 : zipDestPath				ѹ��Ŀ���ļ�����				�����ΪNULL��ѹ����Դ�ļ�����ͬ��Ŀ��λ��ZIP��
*   Retrun Value : ZRESULT					������룬�����ؽ��Ϊ0ʱ����Ϊ�ɹ�
*---------------------------------- PURPOSE -----------------------------------
*  ��Ŀ���ļ��н���ѹ��
*******************************************************************************/
ZRESULT CompressDirToZip(LPCTSTR zipSrcPath, LPCTSTR zipDestPath)
{
#ifndef _UNICODE 
	// �ж��ļ��Ƿ����
	if (_access(unzipSrcPath, 0))
	{
		printf("ԴĿ¼�����ڣ�ѹ��ʧ��\n");
		return ZR_NOFILE;
	}
#else
	if (_waccess(zipSrcPath, 0))
	{
		printf("ԴĿ¼�����ڣ�ѹ��ʧ��\n");
		return ZR_NOFILE;
	}
#endif
	HZIP hz = NULL;
	DWORD zResult = ZR_OK;
	std::wstring zipNameStr;			
	
	
	std::wstring strDestPath;
	inZipPath.clear();
	if (zipDestPath)																		//��Ŀ��Ŀ¼���д���
	{
		strDestPath = zipDestPath;
		int pos = strDestPath.rfind(_T("\\"));
		if (pos != std::wstring::npos)														//�������·����ȡ��·��
		{
			if (strDestPath.rfind(_T(".zip")) != std::string::npos)
			{
				zipNameStr = strDestPath.substr(pos + 1);
				strDestPath = strDestPath.substr(0, pos);
			}
			SHCreateDirectoryEx(NULL, strDestPath.c_str(), NULL);							 //������������Ѿ��ڵ�ǰĿ¼�����贴���ļ���
		}

	}
	else
	{
		TCHAR buffer[MAX_PATH] = { 0 };
		GetCurrentDirectory(MAX_PATH, (LPTSTR)&buffer);
		strDestPath = buffer;
	}

	std::wstring srcStrPath = zipSrcPath;													//��ԴĿ¼���г�·
	std::wstring rootFolderName = _T("");
	int pos = srcStrPath.rfind(_T('\\'));													//��ȡ��ѹ���ļ������һ����ļ���
	if (pos != std::wstring::npos)
	{
		rootFolderName = srcStrPath.substr(pos + 1);
	}
	else
	{
		rootFolderName = zipSrcPath;
	}
	inZipPath.push_back(rootFolderName);													//�����ļ��е�Ŀ¼����zip·��
	if (zipNameStr.empty())
		zipNameStr = rootFolderName + _T(".zip");
	std::wstring zipPath;
	zipPath = strDestPath + _T("\\") + zipNameStr;											//��Ŀ���ļ���Ϊ��ʱ��Ϊ����ֻ������·��������Ҫ����ļ���
	hz = CreateZip(zipPath.c_str(), 0);
	ZipAddFolder(hz, rootFolderName.c_str());												//��ѹ��������Ӹ�Ŀ¼
	zResult = DirToZip(zipSrcPath, zipDestPath, hz);
	printf("ѹ��%s\n", (zResult == 0 ? "�ɹ�" : "ʧ��"));									//Debug
	CloseZip(hz);

	return zResult;
}



/*******************************************************************************
*
*                CODE   FUNCTION    SPECIFICATION
*
********************************************************************************
*
*   FUNCTION NAME: CharToWChar
*   Inputs       : str				Դ�ַ���
*   Retrun Value : wchar_t*			���ַ���
*
*---------------------------------- PURPOSE -----------------------------------
*  ��zip�ļ����н�ѹ
*******************************************************************************/
wchar_t* CharToWChar(const char *str)
{
	wchar_t* buffer = NULL;
	if (str)
	{
		size_t nu = strlen(str);
		size_t n = (size_t)MultiByteToWideChar(ZIP_UNICODE, 0, (const char *)str, int(nu), NULL, 0);
		buffer = 0;
		buffer = new wchar_t[n+1];
		MultiByteToWideChar(ZIP_UNICODE, 0, (const char *)str, int(nu), buffer, int(n));
		buffer[n] = 0;
	}
	return buffer;
}



/*******************************************************************************
*
*                CODE   FUNCTION    SPECIFICATION
*
********************************************************************************
*
*   FUNCTION NAME: ExtractZipToDir
*   Inputs       : unzipSrcPath		zip�ļ�·��
*   Inputs       : unzipDestPath	��ѹ·��	�������NULL ���ѹ����ǰĿ¼
*   Retrun Value : ZRESULT			������룬�����ؽ��Ϊ0ʱ����Ϊ�ɹ�
*
*---------------------------------- PURPOSE -----------------------------------
*  ��zip�ļ����н�ѹ
*******************************************************************************/
ZRESULT ExtractZipToDir(const char* unzipSrcPath, const char* unzipDestPath)
{
	LPCTSTR wunzipSrcPath = CharToWChar(unzipSrcPath);
	LPCTSTR wunzipDestPath = CharToWChar(unzipDestPath);
	ZRESULT ret=ExtractZipToDir(wunzipSrcPath, wunzipDestPath);
	delete (wchar_t*)wunzipDestPath;
	delete (wchar_t*)wunzipSrcPath;
	return ret;
}



ZRESULT CompressDirToZip(std::vector<std::string> vzipSrcPath, const char* czipDestPath)
{

	LPCTSTR zipDestPath = CharToWChar(czipDestPath);
	HZIP hz = NULL;
	DWORD zResult = ZR_OK;
	std::wstring zipNameStr;
	std::wstring tmp;
	TCHAR buffer[MAX_PATH] = { 0 };
	

	std::wstring strDestPath;
	if (zipDestPath)																		//��Ŀ��Ŀ¼���д���
	{
		strDestPath = zipDestPath;
		int pos = strDestPath.rfind(_T("\\"));
		if (pos != std::wstring::npos)														//�������·����ȡ��·��
		{
			if (strDestPath.rfind(_T(".zip")) != std::string::npos)
			{
				zipNameStr = strDestPath.substr(pos + 1);
				strDestPath = strDestPath.substr(0, pos);
			}
			SHCreateDirectoryEx(NULL, strDestPath.c_str(), NULL);							 //������������Ѿ��ڵ�ǰĿ¼�����贴���ļ���
		}

	}
	else
	{
		GetCurrentDirectory(MAX_PATH, (LPTSTR)&buffer);
		strDestPath = buffer;
	}
	tmp = strDestPath + _T("\\") + zipNameStr;												//��Ŀ���ļ���Ϊ��ʱ��Ϊ����ֻ������·��������Ҫ����ļ���
	hz = CreateZip(tmp.c_str(), 0);


	
	for (int i = 0; i < vzipSrcPath.size(); i++)
	{
		inZipPath.clear();
		LPCTSTR zipSrcPath=CharToWChar(vzipSrcPath[i].c_str());

#ifndef _UNICODE 
		// �ж��ļ��Ƿ����
		if (_access(zipSrcPath, 0))
		{
			printf("ԴĿ¼�����ڣ�ѹ��ʧ��\n");
			return ZR_NOFILE;
		}
#else
		if (_waccess(zipSrcPath, 0))
		{
			printf("ԴĿ¼�����ڣ�ѹ��ʧ��\n");
			return ZR_NOFILE;
		}
#endif

		std::wstring srcStrPath = zipSrcPath;													//��ԴĿ¼���г�·
		std::wstring rootFolderName = _T("");
		int pos = srcStrPath.rfind(_T('\\'));													//��ȡ��ѹ���ļ������һ����ļ���
		if (pos != std::wstring::npos)
		{
			rootFolderName = srcStrPath.substr(pos + 1);
		}
		else
		{
			rootFolderName = zipSrcPath;
		}
		inZipPath.push_back(rootFolderName);													//�����ļ��е�Ŀ¼����zip·��

		ZipAddFolder(hz, rootFolderName.c_str());												//��ѹ��������Ӹ�Ŀ¼
		zResult = DirToZip(zipSrcPath, zipDestPath, hz);
		printf("ѹ��%s\n", (zResult == 0 ? "�ɹ�" : "ʧ��"));									//Debug

		delete zipSrcPath;

	}



	CloseZip(hz);

	delete zipDestPath;
	return zResult;
}


/*******************************************************************************
*
*                CODE   FUNCTION    SPECIFICATION
*
********************************************************************************
*
*   FUNCTION NAME: CompressDirToZip
*   Inputs       : zipSrcPath				ѹ��Դ�ļ�������
*   Inputs		 : zipDestPath				ѹ��Ŀ���ļ�����				�����ΪNULL��ѹ����Դ�ļ�����ͬ��Ŀ��λ��ZIP��
*   Retrun Value : ZRESULT					������룬�����ؽ��Ϊ0ʱ����Ϊ�ɹ�
*---------------------------------- PURPOSE -----------------------------------
*  ��Ŀ���ļ��н���ѹ��
*******************************************************************************/
ZRESULT CompressDirToZip(const char* zipSrcPath, const char* zipDestPath)
{
	LPCTSTR wzipSrcPath = CharToWChar(zipSrcPath);
	LPCTSTR wzipDestPath = CharToWChar(zipDestPath);
	ZRESULT ret=CompressDirToZip(wzipSrcPath, wzipDestPath);
	delete (wchar_t*)wzipDestPath;
	delete (wchar_t*)wzipSrcPath;
	return ret;
}





/*******************************************************************************
*
*                CODE   FUNCTION    SPECIFICATION
*
********************************************************************************
*
*   FUNCTION NAME: GetZipErrorMessage
*   Inputs       : code				�������
*   Retrun Value : string			������Ϣ
*---------------------------------- PURPOSE -----------------------------------
*  ��ȡZIP������Ϣ
*******************************************************************************/
std::string GetZipErrorMessage(ZRESULT code)
{
	char buf[512];
	FormatZipMessageZ(code, buf, 512);
	return std::string(buf);
}




/*******************************************************************************
*
*                CODE   FUNCTION    SPECIFICATION
*
********************************************************************************
*
*   FUNCTION NAME: SetZipEncode
*   Inputs       : mode				���뷽ʽ ����ZipDefine.h
*   Retrun Value : 
*---------------------------------- PURPOSE -----------------------------------
*   ����Zip�ı��뷽ʽ
*******************************************************************************/
void SetZipEncode(int mode)
{
	ZIP_UNICODE = mode;
}