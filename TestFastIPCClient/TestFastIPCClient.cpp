
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

// 程序进入点
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int iCmdShow){
	serverName.append(szCmdLine);
	if (serverName.size() == 0){
		MessageBox(NULL, L"创建IPC时必须在启动参数上指定服务端的名称！", L"消息", 0);
		return -1;
	}
	if (client.create(serverName) != 0){
		MessageBox(NULL, L"创建IPC客户端失败！", L"消息", 0);
		return -1;
	}	
	if (server.create(fastipc::genServerName(serverName)) != 0){
		MessageBox(NULL, L"在IPC客户端创建新的服务器失败！", L"消息", 0);
		return -1;
	}
	return createWin(hInstance);
}