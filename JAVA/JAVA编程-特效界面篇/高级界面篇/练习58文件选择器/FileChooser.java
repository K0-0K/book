//FileChooser.java
import java.awt.*;
import java.awt.event.*;
import java.io.File;
import javax.swing.*;

public class FileChooser extends JFrame {
	JFileChooser chooser = new JFileChooser();
	JDialog dialog;
	JButton button = new JButton("显示文件选择。。。");

	public FileChooser() {
		super("文件选择器");
		Container contentPane = getContentPane();

		contentPane.setLayout(new FlowLayout());
		contentPane.add(button);		

		button.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				String title = chooser.getDialogTitle();

				if(title == null)
					chooser.
							getUI().
									getDialogTitle(chooser);

        		dialog = new JDialog((Frame)null, 
        								title, 
        								false);
				
        		Container dialogContentPane = 
								dialog.getContentPane();

        		dialogContentPane.setLayout(new BorderLayout());
        		dialogContentPane.add(chooser, 
								      BorderLayout.CENTER);
 
				dialog.setTitle("请选择要显示的文件");

        		dialog.pack();
        		dialog.setLocationRelativeTo(FileChooser.this);
 
        		dialog.setVisible(true);			
			}
		});
		chooser.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				String state = (String)e.getActionCommand();
				// 确定按钮被激活
				if(state.equals(JFileChooser.APPROVE_SELECTION)) {
					File file = chooser.getSelectedFile();

					JOptionPane.showMessageDialog(
											null, file.getPath());
				}
				// 取消按钮被激活
				else if(
					state.equals(JFileChooser.CANCEL_SELECTION)) {
					JOptionPane.showMessageDialog(
													null,
													"取消"
												  );
				}
				
				dialog.setVisible(false);

			}
		});
	}
	public static void main(String args[]) {
		JFrame f = new FileChooser();
		f.setBounds(300,300,350,100);
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
