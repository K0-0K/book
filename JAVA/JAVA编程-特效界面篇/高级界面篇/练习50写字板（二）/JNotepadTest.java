//JNotepadTest.java
import java.awt.*; 
import java.awt.event.*; 
import java.awt.print.*; 
import java.awt.datatransfer.*; 
import java.io.*; 
import javax.swing.*; 
import javax.swing.event.*; 
import javax.swing.text.*; 

public class JNotepadTest extends JPanel 
{ 
//变量定义
JTextArea jta = new JTextArea("", 24, 40); 
JScrollPane jsp = new JScrollPane(jta); 
//菜单条
JMenuBar jmb = new JMenuBar(); 
JMenu file = new JMenu("文件"); 
JMenu edit = new JMenu("编辑"); 
JMenu search = new JMenu("查找"); 
//工具条
JToolBar toolBar = new JToolBar(); 
//菜单内容
JMenuItem jmi; 
//实例化剪切板
Clipboard clipbd = getToolkit().getSystemClipboard(); 
//打印功能
PrinterJob prtMe = PrinterJob.getPrinterJob(); 
String text = ""; 
//构造函数
public JNotepadTest() 
{ 
class newL implements ActionListener 
{ 
public void actionPerformed(ActionEvent e) 
{ 
jta.setDocument(new PlainDocument()); 
} 
} 
//打开功能
class openL implements ActionListener 
{ 
public void actionPerformed(ActionEvent e) 
{ 
JFileChooser fc = new JFileChooser(); 
int returnVal = fc.showDialog(JNotepadTest.this, "打开"); 
if(returnVal == JFileChooser.APPROVE_OPTION) 
{ 
String file = fc.getSelectedFile().getPath(); 
if(file == null) 
{ 
return; 
} 
//读取文件
try 
{ 
Reader in = new FileReader(file); 
char[] buff = new char[4096]; 
int nch; 
while((nch = in.read(buff, 0, buff.length)) != -1) 
{ 
jta.setDocument(new PlainDocument()); 
jta.append(new String(buff, 0, nch)); 
} 
} 
catch (IOException io) 
{ 
System.err.println("IOException: " + io.getMessage()); 
} 
} 
else 
{ 
return; 
} 
} 
} 
//保存文件
class saveL implements ActionListener 
{ 
public void actionPerformed(ActionEvent e) 
{ 
JFileChooser fc = new JFileChooser(); 
int returnVal = fc.showSaveDialog(JNotepadTest.this); 
if(returnVal == JFileChooser.APPROVE_OPTION) 
{ 
String savefile = fc.getSelectedFile().getPath(); 
if(savefile == null) 
{ 
return; 
} 
else 
{ 
String docToSave = jta.getText(); 
if(docToSave != null) 
{ 
FileOutputStream fstrm = null; 
BufferedOutputStream ostrm = null; 
try 
{ 
fstrm = new FileOutputStream(savefile); 
ostrm = new BufferedOutputStream(fstrm); 
byte[] bytes = null; 
try 
{ 
bytes = docToSave.getBytes(); 
} 
catch(Exception e1) 
{ 
e1.printStackTrace(); 
} 
ostrm.write(bytes); 
} 
catch(IOException io) 
{ 
System.err.println("IOException: " + 
io.getMessage()); 
} 
finally 
{ 
try 
{ 
ostrm.flush(); 
fstrm.close(); 
ostrm.close(); 
} 
catch(IOException ioe) 
{ 
System.err.println("IOException: " + 
ioe.getMessage()); 
} 
} 
} 
} 
} 
else 
{ 
return; 
} 
} 
} 
//页面设置
class pageSetupL implements ActionListener 
{ 
public void actionPerformed(ActionEvent e) 
{ 
prtMe.printDialog(); 
} 
} 
//打印
class printL implements ActionListener 
{ 
public void actionPerformed(ActionEvent e) 
{ 
try 
{ 
prtMe.print(); 
} 
catch(Exception ew) 
{ 
} 
} 
} 
//退出
class exitL implements ActionListener 
{ 
public void actionPerformed(ActionEvent e) 
{ 
System.exit(0); 
} 
} 
//复制
class copyL implements ActionListener 
{ 
public void actionPerformed(ActionEvent e) 
{ 
String selection = jta.getSelectedText(); 
StringSelection clipString = new StringSelection(selection); 
clipbd.setContents(clipString, clipString); 
} 
} 
//剪切
class cutL implements ActionListener 
{ 
public void actionPerformed(ActionEvent e) 
{ 
String selection = jta.getSelectedText(); 
StringSelection clipString = new StringSelection(selection); 
clipbd.setContents(clipString, clipString); 
jta.replaceRange("", jta.getSelectionStart(), 
jta.getSelectionEnd()); 
} 
} 
//粘贴
class pasteL implements ActionListener 
{ 
public void actionPerformed(ActionEvent e) 
{ 
Transferable clipData = clipbd.getContents(JNotepadTest.this); 
try 
{ 
String clipString = 
(String)clipData.getTransferData( 
DataFlavor.stringFlavor); 
jta.replaceRange(clipString, 
jta.getSelectionStart(), jta.getSelectionEnd()); 
} 
catch(Exception ex) 
{ 
} 
} 
} 
//删除
class deleteL implements ActionListener 
{ 
public void actionPerformed(ActionEvent e) 
{ 
String selection = jta.getSelectedText(); 
jta.replaceRange("", jta.getSelectionStart(), 
jta.getSelectionEnd()); 
} 
} 
//全部选择
class selectAllL implements ActionListener 
{ 
public void actionPerformed(ActionEvent e) 
{ 
jta.selectAll(); 
} 
} 
//查找
class findL implements ActionListener 
{ 
public void actionPerformed(ActionEvent e) 
{ 
String find = ""; 
find = JOptionPane.showInputDialog( 
"查找的内容："); 
} 
} 
//查找下一个
class findNextL implements ActionListener 
{ 
public void actionPerformed(ActionEvent e) 
{ 
} 
} 
//事件监听
class jtaL implements ActionListener 
{ 
public void actionPerformed(ActionEvent e) 
{ 
} 
} 
//快捷键设置
file.add(jmi = new JMenuItem("新建", KeyEvent.VK_N)); 
jmi.addActionListener(new newL()); 
file.add(jmi = new JMenuItem("打开", KeyEvent.VK_O)); 
jmi.addActionListener(new openL()); 
file.add(jmi = new JMenuItem("保存", KeyEvent.VK_S)); 
jmi.addActionListener(new saveL()); 
file.addSeparator(); 
file.add(jmi = new JMenuItem("页面设置", KeyEvent.VK_G)); 
jmi.addActionListener(new pageSetupL()); 
file.add(jmi = new JMenuItem("打印", KeyEvent.VK_P)); 
jmi.addActionListener(new printL()); 
file.addSeparator(); 
file.add(jmi = new JMenuItem("退出", KeyEvent.VK_X)); 
jmi.addActionListener(new exitL()); 

edit.add(jmi = new JMenuItem("复制", KeyEvent.VK_C)); 
jmi.addActionListener(new copyL()); 
edit.add(jmi = new JMenuItem("剪切", KeyEvent.VK_T)); 
jmi.addActionListener(new cutL()); 
edit.add(jmi = new JMenuItem("粘帖", KeyEvent.VK_P)); 
jmi.addActionListener(new pasteL()); 
edit.add(jmi = new JMenuItem("删除", KeyEvent.VK_D)); 
jmi.addActionListener(new deleteL()); 
edit.addSeparator(); 
edit.add(jmi = new JMenuItem("全选", KeyEvent.VK_A)); 
jmi.addActionListener(new selectAllL()); 

search.add(jmi = new JMenuItem("查找", KeyEvent.VK_F)); 
jmi.addActionListener(new findL()); 
search.add(jmi = new JMenuItem("查找下一个", KeyEvent.VK_N)); 
jmi.addActionListener(new findNextL()); 
//设置页面布局风格
setLayout(new BorderLayout()); 
file.setMnemonic(KeyEvent.VK_F); 
jmb.add(file); 
edit.setMnemonic(KeyEvent.VK_E); 
jmb.add(edit); 
search.setMnemonic(KeyEvent.VK_S); 
jmb.add(search); 
jmb.add(Box.createHorizontalGlue()); 

toolBar.setFloatable(true); 
addButtons(toolBar); 

add(jmb, BorderLayout.NORTH); 
add(toolBar, BorderLayout.CENTER); 
add(jsp, BorderLayout.SOUTH); 

jta.getCaret().setVisible(true); 
jta.setCaretPosition(0); 
} 

//添加按钮
protected void addButtons(JToolBar toolBar) 
{ 
JButton button = new JButton(new ImageIcon("images/new.gif")); 
button.setToolTipText("新建一个文档"); 
button.addActionListener(new ActionListener() 
{ 
public void actionPerformed(ActionEvent e) 
{ 
jta.setDocument(new PlainDocument()); 
} 
}); 
toolBar.add(button); 
//打开按钮
JButton button1 = new JButton(new ImageIcon("images/open.gif")); 
button1.setToolTipText("打开一个文档"); 
//添加按钮的事件监听
button1.addActionListener(new ActionListener() 
{ 
public void actionPerformed(ActionEvent e) 
{ 
JFileChooser fc = new JFileChooser(); 
int returnVal = fc.showDialog(JNotepadTest.this, "打开文件"); 
if(returnVal == JFileChooser.APPROVE_OPTION) 
{ 
String file = fc.getSelectedFile().getPath(); 
if(file == null) 
{ 
return; 
} 
try 
{ 
Reader in = new FileReader(file); 
char[] buff = new char[4096]; 
int nch; 
while((nch = in.read(buff, 0, buff.length)) != -1) 
{ 
jta.setDocument(new PlainDocument()); 
jta.append(new String(buff, 0, nch)); 
} 
} 
catch (IOException io) 
{ 
System.err.println("IOException: " + io.getMessage()); 
} 
} 
else 
{ 
return; 
} 
} 
}); 
toolBar.add(button1); 
//保存按钮
JButton button2 = new JButton(new ImageIcon("images/save.gif")); 
button2.setToolTipText("保存文档"); 
button2.addActionListener(new ActionListener() 
{ 
public void actionPerformed(ActionEvent e) 
{ 
JFileChooser fc = new JFileChooser(); 
int returnVal = fc.showSaveDialog(JNotepadTest.this); 
if(returnVal == JFileChooser.APPROVE_OPTION) 
{ 
String savefile = fc.getSelectedFile().getPath(); 
if(savefile == null) 
{ 
return; 
} 
else 
{ 
String docToSave = jta.getText(); 
if(docToSave != null) 
{ 
FileOutputStream fstrm = null; 
BufferedOutputStream ostrm = null; 
try 
{ 
fstrm = new FileOutputStream(savefile); 
ostrm = new BufferedOutputStream(fstrm); 
byte[] bytes = null; 
try 
{ 
bytes = docToSave.getBytes(); 
} 
catch(Exception e1) 
{ 
e1.printStackTrace(); 
} 
ostrm.write(bytes); 
} 
catch(IOException io) 
{ 
System.err.println("IOException: " + 
io.getMessage()); 
} 
finally 
{ 
try 
{ 
ostrm.flush(); 
fstrm.close(); 
ostrm.close(); 
} 
catch(IOException ioe) 
{ 
System.err.println("IOException: " + 
ioe.getMessage()); 
} 
} 
} 
} 
} 
else 
{ 
return; 
} 
} 
}); 
toolBar.add(button2); 
//复制按钮
JButton button3 = new JButton(new ImageIcon("images/copy.gif")); 
button3.setToolTipText("复制选择的内容"); 
button3.addActionListener(new ActionListener() 
{ 
public void actionPerformed(ActionEvent e) 
{ 
String selection = jta.getSelectedText(); 
StringSelection clipString = new StringSelection(selection); 
clipbd.setContents(clipString, clipString); 
} 
}); 
toolBar.add(button3); 
//剪切按钮
JButton button4 = new JButton(new ImageIcon("images/cut.gif")); 
button4.setToolTipText("剪切选择的内容"); 
button4.addActionListener(new ActionListener() 
{ 
public void actionPerformed(ActionEvent e) 
{ 
String selection = jta.getSelectedText(); 
StringSelection clipString = new StringSelection(selection); 
clipbd.setContents(clipString, clipString); 
jta.replaceRange("", jta.getSelectionStart(), 
jta.getSelectionEnd()); 
} 
}); 
toolBar.add(button4); 
//粘贴按钮
JButton button5 = new JButton(new ImageIcon("images/paste.gif")); 
button5.setToolTipText("粘贴剪切板的内容"); 
button5.addActionListener(new ActionListener() 
{ 
public void actionPerformed(ActionEvent e) 
{ 
Transferable clipData = clipbd.getContents(JNotepadTest.this); 
try 
{ 
String clipString = 
(String)clipData.getTransferData( 
DataFlavor.stringFlavor); 
jta.replaceRange(clipString, 
jta.getSelectionStart(), jta.getSelectionEnd()); 
} 
catch(Exception ex) 
{ 
} 
} 
}); 
toolBar.add(button5); 

} 


//关闭窗口
protected static final class appCloseL extends WindowAdapter 
{ 
public void windowClosing(WindowEvent e) 
{ 
System.exit(0); 
} 
} 
//主函数，程序入口
public static void main(String args[]) 
{ 
JFrame f = new JFrame(); 
JNotepadTest applet = new JNotepadTest(); 
f.setTitle("写字板"); 
f.setBackground(Color.lightGray); 
f.getContentPane().add(applet, BorderLayout.CENTER); 
f.addWindowListener(new appCloseL()); 
f.setSize(800, 500); 
f.setVisible(true); 
f.pack(); 
} 
} 