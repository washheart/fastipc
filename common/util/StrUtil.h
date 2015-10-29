//////////////////////////////////////////////////////////////////////////  
/// 定义fastipc用到的数据结构和变量  
///  
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-05-16  
////////////////////////////////////////////////////////////////////////// 
#ifndef __MYUTIL_H__
#define __MYUTIL_H__
#include <xstring>
#include <windows.h>
using namespace std;

namespace jw{
	_declspec(dllexport) string w2s(const wstring &ws);
	_declspec(dllexport) wstring s2w(const string &s);

	// 生成唯一性字符串
	_declspec(dllexport) char* GenerateGuid();
	_declspec(dllexport) std::wstring GenerateGuidW();

	_declspec(dllexport) long parseLong(const wstring &ws);
}
#endif