
#ifndef ZIP_FUNCTION
#define ZIP_FUNCTION

/*******************************************************************************
**  AREA1  :  include header files area
*******************************************************************************/

#include "zip/unzip.h"
#include "zip/zip.h"
#include <string>
#include <vector>

/*******************************************************************************
**  AREA2  : code function declare area
*******************************************************************************/

ZRESULT ExtractZipToDir(const char* unzipSrcPath, const char* unzipDestPath = NULL);
ZRESULT CompressDirToZip(const char* zipSrcPath, const char* zipDestPath = NULL);
ZRESULT CompressDirToZip(std::vector<std::string> vzipSrcPath, const char* czipDestPath);
ZRESULT ExtractZipToDir(LPCTSTR unzipSrcPath, LPCTSTR unzipDestPath = NULL);
ZRESULT CompressDirToZip(LPCTSTR zipSrcPath, LPCTSTR zipDestPath = NULL);


std::string GetZipErrorMessage(ZRESULT code);
void SetZipEncode(int mode);
#endif