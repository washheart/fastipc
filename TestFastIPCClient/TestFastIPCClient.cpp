//////////////////////////////////////////////////////////////////////////  
/// 测试IPC通信用的客户端
/// 
/// 源码：http://git.oschina.net/washheart/fastipc
/// 源码：https://github.com/washheart/fastipc
/// 说明：https://github.com/washheart/fastipc/wiki
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-10-22  
////////////////////////////////////////////////////////////////////////// 
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "common/util/StrUtil.h"
#include "common/test/ipccom.h"
#include <windows.h>
#include <thread>
#include <sstream>
#include <string>

LPTSTR getWindowTitle(){ return L"IPC测试之客户端"; }

extern std::wstring serverName;	// 服务端名称
extern fastipc::Server server;  // IPC服务端
extern fastipc::Client client;
extern DWORD blockSize;			// 一次传输时的数据大小

// 程序进入点
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int iCmdShow){
	//MessageBox(NULL, L"调试时用于附加进程！", L"消息", 0);
	int argc = 0;
	LPTSTR * args = CommandLineToArgvW(szCmdLine, &argc);
	if (argc == 0){
		MessageBox(NULL, L"创建IPC时必须在启动参数上指定服务端的名称！", L"消息", 0);
		return -1;
	}
	serverName.append(args[0]);
	if (argc > 1){
		blockSize = jw::parseLong(args[1]);
		if (blockSize == 0){
			MessageBox(NULL, L"创建IPC时指定的块大小应大于0！", L"消息", 0);
			return -1;
		}
	}	
	if (client.create(serverName, blockSize) != 0){
		MessageBox(NULL, L"创建IPC客户端失败！", L"消息", 0);
		return -1;
	}	
	if (server.create(fastipc::genServerName(serverName),blockSize) != 0){
		MessageBox(NULL, L"在IPC客户端创建新的服务器失败！", L"消息", 0);
		return -1;
	}
	return createWin(hInstance);
}