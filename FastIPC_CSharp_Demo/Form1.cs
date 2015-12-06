using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using org.fastipc;
using System.Threading;
using System.IO;
using System.Diagnostics;
using System.Runtime.InteropServices;
namespace JWebTop_CSharp_Demo {

    public partial class MainForm : Form {

        private String serverName;
        private int blockSize;
        private FastIPCServer server;
        private FastIPCClient client;

        public MainForm() {
            InitializeComponent();
        }

        public class MYReadListener : org.fastipc.RebuildedBlockListener {
            MainForm mainFrame;
            public MYReadListener(MainForm f) {
                this.mainFrame = f;
            }

            public override void OnRead(String data) {
                mainFrame.received(data);
            }
        }


        private void MainForm_Load(object sender, EventArgs e) {
            String dllPath = Path.GetFullPath("FastIPC_CSharp.dll");
            if (File.Exists(dllPath)) {
                startServer();
            }
        }

        private void __startIPCServer() { server.startRead(); }
        void startServer() {
            String dir = "..\\..\\..\\Release\\";
            String clientExe = (dir + "TestFastIPCClient.exe");
            //System.out.println("客户端可执行文件 = " + clientExe.getAbsolutePath());
            if (!File.Exists(clientExe)) {
                throw new Exception("客户端可执行文件[" + clientExe + "]不存在");
            }
            //initDLL(dir);
            this.server = new FastIPCServer();
            this.serverName = FastIPCNative.genServerName();
            //System.out.println("serverName = " + serverName);
            this.blockSize = 5;
            server.create(serverName, blockSize);
            server.setListener(new MYReadListener(this));

            Thread th = new Thread(new ThreadStart(__startIPCServer)); //也可简写为new Thread(ThreadMethod);                
            th.Start(); //启动线程  

            ProcessStartInfo process = new ProcessStartInfo();
            process.FileName = Path.GetFullPath(clientExe);
            process.Arguments = " " + serverName + " " + blockSize;

            process.UseShellExecute = false;
            process.CreateNoWindow = true;

            process.RedirectStandardOutput = true;
            Process.Start(process);

            // string Result = p.StandardOutput.ReadToEnd();// 这里会造成线程等待
        }

        delegate void SetTextCallback(string text);
        public void received(String data) {
            if (this.txtReceived.InvokeRequired) {// 跨线程操作界面时需要采用回调方式
                SetTextCallback d = new SetTextCallback(received);
                this.Invoke(d, new object[] { data });
            } else {
                txtReceived.Text += "\r\n接收 " + data;
            }
        }

        protected void wrtieMsgTo() {
            if (client == null) {
                client = new FastIPCClient();
                client.create(FastIPCNative.genServerName(serverName), blockSize);
            }
            client.write(txtMsg.Text);
            txtSended.Text += "\r\n发送 " + txtMsg.Text;
        }

        private void btnCleanSended_Click(object sender, EventArgs e) {
            txtSended.Text = "";
        }

        private void toolStripButton1_Click(object sender, EventArgs e) {
            txtReceived.Text = "";
        }

        private void btnSendMsg_Click(object sender, EventArgs e) {
            wrtieMsgTo();
        }
    }
}