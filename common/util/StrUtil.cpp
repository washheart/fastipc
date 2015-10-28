#include "StrUtil.h"
#include <windows.h> 
#include <ObjBase.h>
#include <codecvt>
#include <fstream>

using namespace std;
namespace jw{
	// 将wstring转换为string(转换后是UTF-8）
	string w2s(const wstring &ws){
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
		std::u16string u16_cvt_str(ws.begin(),ws.end());
		std::string u8_cvt_str = cvt.to_bytes(u16_cvt_str);
		return u8_cvt_str;
	}
	wstring s2w(const string &s){
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
		std::u16string u16_cvt_str=cvt.from_bytes(s);
		return wstring(u16_cvt_str.begin(), u16_cvt_str.end());
	}	

	char* GenerateGuid()
	{
		char* guidBuf;
		guidBuf = (char *)malloc(64);
		GUID guid;
		CoInitialize(NULL);
		if (S_OK == CoCreateGuid(&guid))
		{
			_snprintf(guidBuf, 64, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
				guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
		}
		CoUninitialize();
		return guidBuf;
	}

	wstring GenerateGuidW(){
		char* guidBuf = GenerateGuid();
		string tmp(guidBuf);
		wstring guid=s2w(tmp);
		delete guidBuf;
		return guid;
	}
}