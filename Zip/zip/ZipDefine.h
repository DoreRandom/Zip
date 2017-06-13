#ifndef ZIP_DEFINE_H
#define ZIP_DEFINE_H



#define ZIP_ACP                    0           // default to ANSI code page
#define ZIP_OEMCP                  1           // default to OEM  code page
#define ZIP_MACCP                  2           // default to MAC  code page
#define ZIP_THREAD_ACP             3           // current thread's ANSI code page
#define ZIP_SYMBOL                 42          // SYMBOL translations

#define ZIP_UTF7                   65000        // UTF-7 translation
#define ZIP_UTF8                   65001        // UTF-8 translation

static int ZIP_UNICODE = ZIP_ACP;				//使用所需编码的宏

#endif