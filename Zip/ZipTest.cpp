#include "ZipFunction.h"
#include <stdlib.h>
#include <iostream>
#include <tchar.h>
#include <atlstr.h>


int main()
{
	//Exp1 ��ѹ��ָ��Ŀ¼
	ExtractZipToDir("C:\\Users\\Administrator\\Desktop\\filename.zip", "C:\\Users\\Administrator\\Desktop\\foldername");																								//��ѹ����ǰĿ¼
	//Exp2 ��ѹ����ǰĿ¼
	ExtractZipToDir("C:\\Users\\Administrator\\Desktop\\filename.zip");
	//Exp3 ѹ����ָ��Ŀ¼
	CompressDirToZip("C:\\Users\\Administrator\\Desktop\\filename.zip", "C:\\Users\\Administrator\\Desktop\\foldername");
	//Exp4 ѹ������ǰĿ¼
	CompressDirToZip("C:\\Users\\Administrator\\Desktop\\filename.zip");
	//Exp5 ѹ�����Ŀ¼
	std::vector<std::string> paths;
	paths.push_back("C:\\Users\\Administrator\\Desktop\\foldername1");
	paths.push_back("C:\\Users\\Administrator\\Desktop\\foldername2");
	paths.push_back("C:\\Users\\Administrator\\Desktop\\foldername3");
	CompressDirToZip(paths, "c:\\users\\administrator\\desktop\\filename.zip");
	//Exp6 ��ȡzipѹ�������еĴ�����Ϣ�����
	std::cout << GetZipErrorMessage(0);																												
	system("pause");
	return 0;
}