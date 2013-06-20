//Test.java
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.io.*;
import java.net.URL;

public class Test extends JFrame {
	//变量声明
	JFileChooser   chooser = new JFileChooser();
	TextPreviewer  previewer = new TextPreviewer();
	PreviewPanel   previewPanel = new PreviewPanel();

        //制作预览面板
	class PreviewPanel extends JPanel {
		public PreviewPanel() {
			JLabel label = new JLabel("Text Previewer",
									   SwingConstants.CENTER);
			setPreferredSize(new Dimension(350,0));
			setBorder(BorderFactory.createEtchedBorder());

			setLayout(new BorderLayout());

			label.setBorder(BorderFactory.createEtchedBorder());
			add(label, BorderLayout.NORTH);
			add(previewer, BorderLayout.CENTER);
		}
	}
	//构造函数
	public Test() {
		super("Filtering Files");
		
		Container contentPane = getContentPane();
		JButton	button = new JButton("Select A File");

		contentPane.setLayout(new FlowLayout());
		contentPane.add(button);		

		button.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				int state = chooser.showOpenDialog(null);
				String s = "CANCELED";

				if(state == JFileChooser.APPROVE_OPTION) {
					s = "File Selected: " + 
						 chooser.getSelectedFile().getPath();
				}
				JOptionPane.showMessageDialog(null, s);
			}
		});

		chooser.setAccessory(previewPanel);
		chooser.addChoosableFileFilter(new TextFilter());
		chooser.addChoosableFileFilter(new JavaCodeFilter());

		chooser.addPropertyChangeListener(
									new PropertyChangeListener() {
			public void propertyChange(PropertyChangeEvent e) {
				if(e.getPropertyName().equals(
				   JFileChooser.SELECTED_FILE_CHANGED_PROPERTY)) {
					previewer.configure((File)e.getNewValue());
				}
			}
		});
	}
	//主函数
	public static void main(String a[]) {
		JFrame f = new Test();
		f.setBounds(300, 300, 300, 75);
		f.setVisible(true);

		f.setDefaultCloseOperation(
								WindowConstants.DISPOSE_ON_CLOSE);

		f.addWindowListener(new WindowAdapter() {
			public void windowClosed(WindowEvent e) {
				System.exit(0);
			}
		});
	}
}
//过滤文件名字
abstract class SuffixAwareFilter 
			   extends javax.swing.filechooser.FileFilter {
	public String getSuffix(File f) {
		String s = f.getPath(), suffix = null;
	    int i = s.lastIndexOf('.');

	    if(i > 0 &&  i < s.length() - 1)
	    	suffix = s.substring(i+1).toLowerCase();

		return suffix;
	}
	public boolean accept(File f) {
		return f.isDirectory();
	}
}
//过滤图像文件
class ImageFilter extends SuffixAwareFilter {
    public boolean accept(File f) {
		boolean accept = super.accept(f);

		if( ! accept) {
			String suffix = getSuffix(f);

			if(suffix != null)
				accept = super.accept(f) || suffix.equals("jpg")
				|| suffix.equals("gif");
		}
		return accept;
	}
	public String getDescription() {
		return "Java Source Code Files(*.java)";
	}
}
//过滤.java文件
class JavaCodeFilter extends SuffixAwareFilter {
    public boolean accept(File f) {
		boolean accept = super.accept(f);

		if( ! accept) {
			String suffix = getSuffix(f);

			if(suffix != null)
				accept = super.accept(f) || suffix.equals("java");
		}
		return accept;
	}
	public String getDescription() {
		return "Java Source Code Files(*.java)";
	}
}
//过滤.txt文件
class TextFilter extends SuffixAwareFilter {
    public boolean accept(File f) {
		String suffix = getSuffix(f);

		if(suffix != null)
			return super.accept(f) || suffix.equals("txt");

		return false;
	}
	public String getDescription() {
		return "Text Files(*.txt)";
	}
}
//显示文件内容
class TextPreviewer extends JComponent {
	private JTextArea textArea = new JTextArea();
	private JScrollPane scrollPane = new JScrollPane(textArea);

	public TextPreviewer() {
		textArea.setEditable(false);

		setBorder(BorderFactory.createEtchedBorder());
		setLayout(new BorderLayout());
		add(scrollPane, BorderLayout.CENTER);
	}
	public void configure(File file) {
		textArea.setText(contentsOfFile(file));

		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				JViewport vp = scrollPane.getViewport();

				vp.setViewPosition(new Point(0,0));
			}
		});
	}
	//读取文件内容
	static String contentsOfFile(File file) {
		String s = new String();
		char[] buff = new char[50000];
		InputStream is;
		InputStreamReader reader;
		URL url;

		try {
			reader = new FileReader(file);

			int nch;

			while ((
				nch = reader.read(buff, 0, buff.length)) != -1) {
				s = s + new String(buff, 0, nch);
			}
		} 
		catch (java.io.IOException ex) {
			s = "Could not load file";
		}
		return s;
	}
}
