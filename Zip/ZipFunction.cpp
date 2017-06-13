

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
static std::vector<std::wstring> inZipPath;													//用于存储文件相对于ZIP根目录的路径

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
*   Inputs       : unzipSrcPath		zip文件路径
*   Inputs       : unzipDestPath	解压路径	如果传入NULL 则解压到当前目录
*   Retrun Value : ZRESULT			错误代码，当返回结果为0时，其为成功
*
*---------------------------------- PURPOSE -----------------------------------
*  对zip文件进行解压
*******************************************************************************/
ZRESULT ExtractZipToDir(LPCTSTR unzipSrcPath, LPCTSTR unzipDestPath)
{

#ifndef _UNICODE 
	// 判断文件是否存在
	if (_access(unzipSrcPath, 0))
	{
		printf("源目录不存在，解压失败\n", unzipSrcPath);
		return ZR_NOFILE;
	}
#else
	if (_waccess(unzipSrcPath, 0))
	{

		printf("源目录不存在，解压失败\n");
		return ZR_NOFILE;
	}
#endif

	DWORD zResult = ZR_OK;																	//返回值
	
	std::wstring strunzipDestPath;															//用于记录解压目标目录
	if (unzipDestPath)																		// 如果解压路径不存在先创建, 存在不做任何修改
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

	HZIP hz = OpenZip(unzipSrcPath, 0);														//zip处理开始
	ZIPENTRY ze;

	GetZipItem(hz, -1, &ze);																//获取尾节点信息
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
	CloseZip(hz);																			//清除缓存
	printf("解压%s\n", (zResult == 0 ? "成功" : "失败"));									//Debug
	return zResult;
}

/*******************************************************************************
*
*                CODE   FUNCTION    SPECIFICATION
*
********************************************************************************
*
*   FUNCTION NAME: GetDirPath
*   Inputs       : vec				解压源文件名
*   Retrun Value : ret				解压目标文件夹名称
*
*---------------------------------- PURPOSE -----------------------------------
*  通过vector拼接多级文件夹名称为路径名称
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
*   Inputs       : srcPath				压缩源文件夹名称
*   Inputs		 : destPath				压缩目标文件名称				如果其为NULL则压缩到源文件名相同的目标位置ZIP下
*	Outputs		 : hz					zip结构指针
*   Retrun Value : ZRESULT				错误代码，当返回结果为0时，其为成功
*---------------------------------- PURPOSE -----------------------------------
*   递归解压
*******************************************************************************/
ZRESULT DirToZip(LPCTSTR srcPath, LPCTSTR destPath, HZIP& hz)
{

	DWORD zResult = ZR_OK;
	WIN32_FIND_DATA fileData;
	std::wstring formatPath = std::wstring(srcPath) + _T("\\*.*");
	HANDLE file = FindFirstFile(formatPath.c_str(), &fileData);

	FindNextFile(file, &fileData);																//通过扫描获取文件信息
	while (FindNextFile(file, &fileData))
	{
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)								//如果这是一个文件夹
		{
			inZipPath.push_back(fileData.cFileName);											//加入到压缩包文件的路径中
			std::wstring dirPath = GetDirPath(inZipPath);
			ZipAddFolder(hz, dirPath.c_str());													//添加文件夹
			std::wstring subPath = std::wstring(srcPath) + _T("\\") + fileData.cFileName;		//拼接子路径
			DirToZip(subPath.c_str(), destPath, hz);											//递归调用 继续查看子文件夹
			inZipPath.pop_back();
		}
		else
		{
			std::wstring zipRelativePath = GetDirPath(inZipPath) + _T("\\") + fileData.cFileName;	//压缩相对位置
			std::wstring filesPath = std::wstring(srcPath) + _T("\\") + fileData.cFileName;			//文件路径
			ZipAdd(hz, zipRelativePath.c_str(), filesPath.c_str());									//保存到当前路径中
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
*   Inputs       : zipSrcPath				压缩源文件夹名称
*   Inputs		 : zipDestPath				压缩目标文件名称				如果其为NULL则压缩到源文件名相同的目标位置ZIP下
*   Retrun Value : ZRESULT					错误代码，当返回结果为0时，其为成功
*---------------------------------- PURPOSE -----------------------------------
*  对目标文件夹进行压缩
*******************************************************************************/
ZRESULT CompressDirToZip(LPCTSTR zipSrcPath, LPCTSTR zipDestPath)
{
#ifndef _UNICODE 
	// 判断文件是否存在
	if (_access(unzipSrcPath, 0))
	{
		printf("源目录不存在，压缩失败\n");
		return ZR_NOFILE;
	}
#else
	if (_waccess(zipSrcPath, 0))
	{
		printf("源目录不存在，压缩失败\n");
		return ZR_NOFILE;
	}
#endif
	HZIP hz = NULL;
	DWORD zResult = ZR_OK;
	std::wstring zipNameStr;			
	
	
	std::wstring strDestPath;
	inZipPath.clear();
	if (zipDestPath)																		//对目标目录进行处理
	{
		strDestPath = zipDestPath;
		int pos = strDestPath.rfind(_T("\\"));
		if (pos != std::wstring::npos)														//如果包含路径则取出路径
		{
			if (strDestPath.rfind(_T(".zip")) != std::string::npos)
			{
				zipNameStr = strDestPath.substr(pos + 1);
				strDestPath = strDestPath.substr(0, pos);
			}
			SHCreateDirectoryEx(NULL, strDestPath.c_str(), NULL);							 //如果不包含则已经在当前目录下无需创建文件夹
		}

	}
	else
	{
		TCHAR buffer[MAX_PATH] = { 0 };
		GetCurrentDirectory(MAX_PATH, (LPTSTR)&buffer);
		strDestPath = buffer;
	}

	std::wstring srcStrPath = zipSrcPath;													//对源目录进行出路
	std::wstring rootFolderName = _T("");
	int pos = srcStrPath.rfind(_T('\\'));													//获取待压缩文件夹最后一层的文件名
	if (pos != std::wstring::npos)
	{
		rootFolderName = srcStrPath.substr(pos + 1);
	}
	else
	{
		rootFolderName = zipSrcPath;
	}
	inZipPath.push_back(rootFolderName);													//将根文件夹的目录放入zip路径
	if (zipNameStr.empty())
		zipNameStr = rootFolderName + _T(".zip");
	std::wstring zipPath;
	zipPath = strDestPath + _T("\\") + zipNameStr;											//当目标文件名为空时因为上文只创建了路径，所以要添加文件名
	hz = CreateZip(zipPath.c_str(), 0);
	ZipAddFolder(hz, rootFolderName.c_str());												//在压缩包中添加根目录
	zResult = DirToZip(zipSrcPath, zipDestPath, hz);
	printf("压缩%s\n", (zResult == 0 ? "成功" : "失败"));									//Debug
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
*   Inputs       : str				源字符串
*   Retrun Value : wchar_t*			宽字符串
*
*---------------------------------- PURPOSE -----------------------------------
*  对zip文件进行解压
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
*   Inputs       : unzipSrcPath		zip文件路径
*   Inputs       : unzipDestPath	解压路径	如果传入NULL 则解压到当前目录
*   Retrun Value : ZRESULT			错误代码，当返回结果为0时，其为成功
*
*---------------------------------- PURPOSE -----------------------------------
*  对zip文件进行解压
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
	if (zipDestPath)																		//对目标目录进行处理
	{
		strDestPath = zipDestPath;
		int pos = strDestPath.rfind(_T("\\"));
		if (pos != std::wstring::npos)														//如果包含路径则取出路径
		{
			if (strDestPath.rfind(_T(".zip")) != std::string::npos)
			{
				zipNameStr = strDestPath.substr(pos + 1);
				strDestPath = strDestPath.substr(0, pos);
			}
			SHCreateDirectoryEx(NULL, strDestPath.c_str(), NULL);							 //如果不包含则已经在当前目录下无需创建文件夹
		}

	}
	else
	{
		GetCurrentDirectory(MAX_PATH, (LPTSTR)&buffer);
		strDestPath = buffer;
	}
	tmp = strDestPath + _T("\\") + zipNameStr;												//当目标文件名为空时因为上文只创建了路径，所以要添加文件名
	hz = CreateZip(tmp.c_str(), 0);


	
	for (int i = 0; i < vzipSrcPath.size(); i++)
	{
		inZipPath.clear();
		LPCTSTR zipSrcPath=CharToWChar(vzipSrcPath[i].c_str());

#ifndef _UNICODE 
		// 判断文件是否存在
		if (_access(zipSrcPath, 0))
		{
			printf("源目录不存在，压缩失败\n");
			return ZR_NOFILE;
		}
#else
		if (_waccess(zipSrcPath, 0))
		{
			printf("源目录不存在，压缩失败\n");
			return ZR_NOFILE;
		}
#endif

		std::wstring srcStrPath = zipSrcPath;													//对源目录进行出路
		std::wstring rootFolderName = _T("");
		int pos = srcStrPath.rfind(_T('\\'));													//获取待压缩文件夹最后一层的文件名
		if (pos != std::wstring::npos)
		{
			rootFolderName = srcStrPath.substr(pos + 1);
		}
		else
		{
			rootFolderName = zipSrcPath;
		}
		inZipPath.push_back(rootFolderName);													//将根文件夹的目录放入zip路径

		ZipAddFolder(hz, rootFolderName.c_str());												//在压缩包中添加根目录
		zResult = DirToZip(zipSrcPath, zipDestPath, hz);
		printf("压缩%s\n", (zResult == 0 ? "成功" : "失败"));									//Debug

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
*   Inputs       : zipSrcPath				压缩源文件夹名称
*   Inputs		 : zipDestPath				压缩目标文件名称				如果其为NULL则压缩到源文件名相同的目标位置ZIP下
*   Retrun Value : ZRESULT					错误代码，当返回结果为0时，其为成功
*---------------------------------- PURPOSE -----------------------------------
*  对目标文件夹进行压缩
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
*   Inputs       : code				错误代码
*   Retrun Value : string			错误信息
*---------------------------------- PURPOSE -----------------------------------
*  获取ZIP错误信息
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
*   Inputs       : mode				编码方式 参照ZipDefine.h
*   Retrun Value : 
*---------------------------------- PURPOSE -----------------------------------
*   设置Zip的编码方式
*******************************************************************************/
void SetZipEncode(int mode)
{
	ZIP_UNICODE = mode;
}