// TestFastIPCServer.cpp : 定义应用程序的入口点。
//

#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "common/util/StrUtil.h"
#include "common/test/ipccom.h"
#include <thread>
#include <string>
#include <sstream>

extern std::wstring serverName;	// 服务端名称
extern fastipc::Server server;  // IPC服务端
extern DWORD blockSize;			// 一次传输时的数据大小

LPTSTR getWindowTitle(){ return L"IPC测试之服务端"; }
std::wstring getClientExe(){
	// 得到程序启动目录
	TCHAR   szPath[1000];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	std::wstring path(szPath);
	path = path.substr(0, path.find_last_of('\\') + 1);
	path.append(L"TestFastIPCClient.exe");
	return path;
}
DWORD startClientProcess(std::wstring clientExe, std::wstring serverName, DWORD blockSize){
	STARTUPINFO sui;
	ZeroMemory(&sui, sizeof(STARTUPINFO));
	sui.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi; // 保存了所创建子进程的信息
	DWORD dwCreationFlags = CREATE_NEW_PROCESS_GROUP;
	// 生成命令行
	std::wstringstream wss; wss << clientExe << L" " << serverName << L" " << blockSize;
	if (CreateProcess(NULL, LPTSTR(wss.str().c_str()), // 第一个参数置空，可执行文件和命令行放到一起避免恶意程序问题
		NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, &sui, &pi)){
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return pi.dwThreadId;
	}
	return 0;
}

// 程序进入点
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int iCmdShow){
	blockSize = 2048;// 
	blockSize = 2;// 指定一个较小的缓存区空间，以便测试数据的分段传输

	serverName = fastipc::genServerName();
	std::wstring clientExee = getClientExe();
	if (_waccess(LPTSTR(clientExee.c_str()), 0) != 0){
		serverName.append(L"文件找不到！");
		MessageBox(NULL, LPTSTR(serverName.c_str()), L"消息", 0);// 如果文件不存在
		return 0;
	}
	if (server.create(serverName, blockSize) != 0){
		MessageBox(NULL, L"创建IPC服务器失败", L"消息", 0);
		return -1;
	}
	else{
		if (startClientProcess(clientExee, serverName, blockSize) == 0){// 创建子进程，失败后返回
			return -1;
		}
	}
	return createWin(hInstance);
}