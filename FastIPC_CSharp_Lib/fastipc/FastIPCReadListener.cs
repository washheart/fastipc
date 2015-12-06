using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace org {
    namespace fastipc {
        /**
         * 服务端侦听器，当客户端有数据写入时，会触发。<br/>
         * 源码：https://github.com/washheart/fastipc<br/>
         * 源码：http://git.oschina.net/washheart/fastipc<br/>
         * 说明：https://github.com/washheart/fastipc/wiki<br/>
         * 
         * @author washheart@163.com
         */
        public interface FastIPCReadListener {
            /**
	         * 当FastIPC服务端成功接收客户端写入的数据后，会回调此方法
	         * 
	         * @param msgType
	         *            消息的类型，参见FastIPCNative.MSG_TYPE_*
	         * @param packId
	         *            当消息是被拆分消息时(msgType!=MSG_TYPE_NORMAL)，用于将多个消息组织在一起
	         * @param data
	         *            传递的消息具体内容，注意消息可能是分段的，此时需要根据packId进行组装
	         */
            void OnRead(int msgType, IntPtr packId, IntPtr data, int dataLen);
        }
        public abstract class RebuildedBlockListener : org.fastipc.FastIPCReadListener {
            private Dictionary<String, Stream> caches = new Dictionary<String, Stream>();

            public abstract void OnRead(String data);

            public void OnRead(int msgType, IntPtr _packId, IntPtr data, int dataLen) {
                string packId = FastIPCNative.ptr2string(_packId);
                if (msgType == FastIPCNative.MSG_TYPE_NORMAL) {
                    OnRead(FastIPCNative.ptr2string(data, dataLen));
                } else {
                    Stream bos = null;
                    if (caches.ContainsKey(packId)) {
                        bos = caches[packId];
                    } else {
                        bos = new MemoryStream();
                        caches.Add(packId, bos);
                    }
                    FastIPCNative.readPtr(bos, data, dataLen);
                    if (msgType == FastIPCNative.MSG_TYPE_END) {
                        caches.Remove(packId);
                        byte[] bytes = new byte[bos.Length];
                        bos.Position = 0; // 设置当前流的位置为流的开始
                        bos.Read(bytes, 0, bytes.Length);
                        string rtn = Encoding.UTF8.GetString(bytes);
                        OnRead(rtn);
                    }
                }
            }
        }

    }
}
