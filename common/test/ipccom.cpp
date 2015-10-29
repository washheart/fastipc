/// 测试fastipc的通信的相关代码
#include "ipccom.h"
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "common/util/StrUtil.h"
#include <thread>
#include <string>
#include <sstream>

std::wstring serverName;	// 服务端名称
fastipc::Server server;     // IPC服务端
fastipc::Client client;		// IPC客户端
std::stringstream blocks;	// 当一个大数据被分成多块时，这里用来临时缓存每块的数据
DWORD blockSize = 10;		// 一次传输时的数据大小
DWORD sendMsg(std::wstring msg){
	if (!client.isStable()){// 检查client是否已经初始化
		client.create(fastipc::genServerName(serverName), blockSize);// 初始化client
	}
	std::string s = jw::w2s(msg);
	return client.write(LPSTR(s.c_str()), s.size());// 发送消息到服务端
}

class ServerReadListener :public fastipc::ReadListener{
	void onRead(fastipc::MemBlock* readed) override{
		if (readed->dataLen > 0){
			int len = readed->dataLen;
			char * tmp;
			tmp = (char *)malloc(len + 1);
			memcpy(tmp, readed->data, len);
			tmp[len] = '\0';
			if (readed->msgType != MSG_TYPE_NORMAL){
				blocks << tmp;// FIXME：这里在组织数据段时没有根据packId来判断是否是同一组数据，如果存在多个客户端或客户端多线程发送时需要考虑这种情况
				if (readed->msgType == MSG_TYPE_END){
					std::wstring ws = jw::s2w(blocks.str());
					ws.append(L" UID=");
					char * uid;
					uid = (char *)malloc(PACK_ID_LEN + 1);
					memcpy(uid, readed->packId, PACK_ID_LEN);
					uid[PACK_ID_LEN] = '\0';
					ws.append(jw::s2w(uid));
					appendReceied(ws);
					blocks.str("");
				}
			}
			else{
				appendReceied(jw::s2w(tmp));
			}
			delete tmp;
		}
		delete readed;
	}
};

// 启动IPC服务端
void startServer(){
	server.startRead();// 开始接收数据，此方法会阻塞当前线程
}

// 服务端窗口已创建
void windowCreated(HWND hWnd){
	server.setListener(new ServerReadListener());// 设置侦听器，以便对收到的数据进行处理
	std::thread newThread(startServer);			 // 在独立的进程中启动IPC服务器
	newThread.detach();							 // 分离进程，以免阻塞
}