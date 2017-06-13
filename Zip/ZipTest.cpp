#include "ZipFunction.h"
#include <stdlib.h>
#include <iostream>
#include <tchar.h>
#include <atlstr.h>


int main()
{
	//Exp1 解压到指定目录
	ExtractZipToDir("C:\\Users\\Administrator\\Desktop\\filename.zip", "C:\\Users\\Administrator\\Desktop\\foldername");																								//解压到当前目录
	//Exp2 解压到当前目录
	ExtractZipToDir("C:\\Users\\Administrator\\Desktop\\filename.zip");
	//Exp3 压缩到指定目录
	CompressDirToZip("C:\\Users\\Administrator\\Desktop\\filename.zip", "C:\\Users\\Administrator\\Desktop\\foldername");
	//Exp4 压缩到当前目录
	CompressDirToZip("C:\\Users\\Administrator\\Desktop\\filename.zip");
	//Exp5 压缩多个目录
	std::vector<std::string> paths;
	paths.push_back("C:\\Users\\Administrator\\Desktop\\foldername1");
	paths.push_back("C:\\Users\\Administrator\\Desktop\\foldername2");
	paths.push_back("C:\\Users\\Administrator\\Desktop\\foldername3");
	CompressDirToZip(paths, "c:\\users\\administrator\\desktop\\filename.zip");
	//Exp6 获取zip压缩过程中的错误信息并输出
	std::cout << GetZipErrorMessage(0);																												
	system("pause");
	return 0;
}