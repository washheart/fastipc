package org.fastipc;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.WindowEvent;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.util.HashMap;
import java.util.Map;

import javax.swing.AbstractAction;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.border.EmptyBorder;

/**
 * 测试FastIPC的java服务端/客户端。<br/>
 * 本类创建一个服务端，然后启动c++例子TestFastIPCClient.exe，并与之交互。<br/>
 * 源码：https://github.com/washheart/fastipc<br/>
 * 源码：http://git.oschina.net/washheart/fastipc<br/>
 * 说明：https://github.com/washheart/fastipc/wiki<br/>
 * 
 * @author washheart@163.com
 */
public class TestFastIPCServer {
	static void initDLL(String dir) throws Exception {
		File dllFile = new File(dir, "FastIPC.dll");
		if (!dllFile.exists()) {
			throw new Exception("DLL文件[" + dllFile + "]不存在");
		}
		System.load(dllFile.getAbsolutePath());
		dllFile = new File(dir, "FastIPC_JNI.dll");
		if (!dllFile.exists()) {
			throw new Exception("DLL文件[" + dllFile + "]不存在");
		}
		System.load(dllFile.getAbsolutePath());
	}

	private String serverName;
	private int blockSize;
	private FastIPCServer server;
	private FastIPCClient client;
	JFrame mainFrame = null;
	private JTextArea txtReceived = new JTextArea("在这里显示已收到的消息"), txtSended = new JTextArea("在这里显示已发送的消息");
	private JTextField txtMsg = new JTextField("在这里输入要发送的消息");

	class MYReadListener implements FastIPCReadListener {
		Map<String, ByteArrayOutputStream> caches = new HashMap<String, ByteArrayOutputStream>();

		@Override
		public void OnRead(int msgType, String packId, byte[] data) {
			System.out.println("msgType = " + msgType + " packId = " + packId);
			try {
				if (msgType == FastIPCNative.MSG_TYPE_NORMAL) {
					received(new String(data, "utf-8"));
				} else {
					ByteArrayOutputStream bos = caches.get(packId);
					if (bos == null) {
						bos = new ByteArrayOutputStream();
						caches.put(packId, bos);
					}
					bos.write(data);
					if (msgType == FastIPCNative.MSG_TYPE_END) {
						caches.remove(packId);
						received(new String(bos.toByteArray(), "utf-8"));
					}
				}
			} catch (Throwable e) {
				e.printStackTrace();
			}
		}
	}

	void startServer() throws Exception {
		final String dir = new File("..\\Release\\").getCanonicalPath() + File.separator;
		final File clientExe = new File(dir + "TestFastIPCClient.exe");
		System.out.println("客户端可执行文件 = " + clientExe.getAbsolutePath());
		if (!clientExe.exists()) {
			throw new Exception("客户端可执行文件[" + clientExe + "]不存在");
		}
		initDLL(dir);
		this.server = new FastIPCServer();
		this.serverName = FastIPCNative.genServerName();
		System.out.println("serverName = " + serverName);
		this.blockSize = 5;
		server.create(serverName, blockSize);
		server.setListener(new MYReadListener());
		new Thread() {
			@Override
			public void run() {
				server.startRead();
			}
		}.start();
		String[] cmds = { clientExe.getAbsolutePath(), serverName, Integer.toString(blockSize) };
		try {
			Runtime.getRuntime().exec(cmds, null, clientExe.getParentFile());
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}

	public void received(String data) {
		txtReceived.setText(txtReceived.getText()//
				+ "接收 " + data + "\r\n");
	}

	protected void wrtieMsgTo() {
		if (client == null) {
			client = new FastIPCClient();
			client.create(FastIPCNative.genServerName(serverName), blockSize);
		}
		client.write(txtMsg.getText());
		txtSended.setText(txtSended.getText()//
				+ "发送 " + txtMsg.getText() + "\r\n");
	}

	void createView() {
		mainFrame = new JFrame() {
			@Override
			protected void processWindowEvent(WindowEvent e) {
				if (e.getID() == WindowEvent.WINDOW_CLOSING) {
					if (client != null) {
						client.close();
						client = null;
					}
					if (server != null) {
						server.close();
						server = null;
					}
				}
				super.processWindowEvent(e);
			}
		};
		JPanel toolPanel = new JPanel(new BorderLayout());
		toolPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
		JPanel toolLeft = new JPanel(new FlowLayout(FlowLayout.LEFT, 5, 0));
		JPanel toolRight = new JPanel(new FlowLayout(FlowLayout.RIGHT, 5, 0));
		toolLeft.add(new JButton(new AbstractAction("清空发送") {
			@Override
			public void actionPerformed(ActionEvent e) {
				txtSended.setText("");
			}
		}));
		toolLeft.add(new JButton(new AbstractAction("清空接收") {
			@Override
			public void actionPerformed(ActionEvent e) {
				txtReceived.setText("");
			}
		}));
		toolRight.add(new JButton(new AbstractAction("发送消息") {
			@Override
			public void actionPerformed(ActionEvent e) {
				wrtieMsgTo();
			}
		}));
		toolPanel.add(toolLeft, BorderLayout.WEST);
		toolPanel.add(txtMsg, BorderLayout.CENTER);
		toolPanel.add(toolRight, BorderLayout.EAST);
		JSplitPane splitPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
		splitPane.add(new JScrollPane(txtSended), JSplitPane.TOP);
		splitPane.add(new JScrollPane(txtReceived), JSplitPane.BOTTOM);
		mainFrame.add(toolPanel, BorderLayout.NORTH);
		mainFrame.add(splitPane, BorderLayout.CENTER);
		mainFrame.setTitle("IPC测试之服务端");
		mainFrame.setSize(800, 600);
		splitPane.setDividerLocation(300);
		mainFrame.setVisible(true);
		mainFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	}

	public static void main(String[] args) {
		TestFastIPCServer test = null;
		try {
			test = new TestFastIPCServer();
			test.createView();
			test.startServer();
		} catch (Throwable e) {
			e.printStackTrace();
			JOptionPane.showMessageDialog(test == null ? null : test.mainFrame, e.getMessage());
		} finally {}
	}
}