//////////////////////////////////////////////////////////////////////////  
/// 定义fastipc用到的数据结构和变量  
///  
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-10-22  
////////////////////////////////////////////////////////////////////////// 
#pragma once

#ifndef FastIPC_model_H
#define FastIPC_model_H
#include <Windows.h>
#include <string>

// 常见错误类型
#define ERR_ServerCreate        100 // 服务器创建失败
#define ERR_ClientCreate		101	// 客户端创建失败
#define ERR_EventCreate_W		201 // 服务器创建写事件失败
#define ERR_EventCreate_R		202 // 服务器创建读事件失败
#define ERR_EventOpen_W			203 // 客户端打开写事件失败
#define ERR_EventOpen_R			204 // 客户端打开读事件失败
#define ERR_EventWait_W			205 // 客户端等待可写事件失败
#define ERR_EventWait_R			206 // 服务器等待可读事件失败
#define ERR_MappingCreate       301	// 服务器创建映射文件失败
#define ERR_MappingOpen			302	// 服客户端打开映射文件失败
#define ERR_MappingMap			303 // 服务器/客户端映射内存文件失败

#define MEM_SIZE				4096// MemBuff结构中data的长度，即一次内存读写的长度
#define PACK_ID_LEN				100	// MemBuff结构中packId的长度

// 定义消息类型常量
#define MSG_TYPE_NORMAL			0	// 正常的消息
#define MSG_TYPE_PART			1	// 此消息是一完整数据的一部分
#define MSG_TYPE_END			2	// 此消息是一完整数据的结尾

// 定义内存状态常量
#define MEM_CAN_WRITE			0	 // 内存处于可写状态
#define MEM_CAN_READ			1	// 内存处于可读状态
#define	MEM_IS_BUSY				2	// 内存处于忙碌状态（可能在读、也可能在写）

namespace fastipc{
	// 定义共享内存区数据结构
	struct _declspec(dllexport)  MemBuff{
		volatile LONG	state = MEM_CAN_WRITE;	// 内存状态，初始状态为可写
		LONG			msgType;
		//LONG			packIdx;				// 由于不考虑发送失败后重发的问题，且发送时是顺序发送，所以可以必要packIdx变量。当一个大数据块被拆分时的索引：如果packIdx=-1,不用考虑组装问题
		char			packId[PACK_ID_LEN];	// 当一个大数据块被拆分时接收放用来接收数据
		DWORD			dataLen;
		char			data[MEM_SIZE];			// 待传输的数据
	};

	/// 生成服务端名称，此名称用来创建读写事件和内存文件，在创建fastipc的服务端时使用
	/// @return 返回生成的服务器名称，此处是生成一个uuid
	_declspec(dllexport) 	std::wstring genServerName();

	/// 生成服务端名称，此名称用来创建读写事件和内存文件，一般是在客户端再次创建用于侦听其对应服务端的回写的时候使用
	/// @param servername 客户端当前对应的服务器端的名称
	/// @return 一个新的服务端名称，此处为：servername_c
	_declspec(dllexport) std::wstring genServerName(const std::wstring servername);

	/// 根据服务端名称生成用于创建写完成事件的名称
	/// @param servername 服务器端的名称
	/// @return 写完成事件的名称，此处为：servername_w
	_declspec(dllexport) std::wstring genWritedEventName(const std::wstring servername);

	/// 根据服务端名称生成用于创建读完成事件的名称
	/// @param servername 服务器端的名称
	/// @return 读完成事件的名称，此处为：servername_r
	_declspec(dllexport) std::wstring genReadedEventName(const std::wstring servername);

	/// 根据服务端名称生成用于创建内存映射文件的名称
	/// @param servername 服务器端的名称
	/// @return 内存映射文件的名称，此处为：servername_m
	_declspec(dllexport) std::wstring genMappingFileName(const std::wstring servername);
};
#endif