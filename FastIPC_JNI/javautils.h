/// 定义一些jni使用的类和方法
/// 
/// 源码：http://git.oschina.net/washheart/fastipc
/// 源码：https://github.com/washheart/fastipc
/// 说明：https://github.com/washheart/fastipc/wiki
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-10-31  
////////////////////////////////////////////////////////////////////////// 
#include <windows.h> 
#include "jni_jdk1_6_24/jni.h"
#include "common/util/StrUtil.h"

//std::string jstring2string(JNIEnv * env, jstring jstr);
std::wstring jstring2wstring(JNIEnv * env, jstring jstr);
