//ColorPanel.java
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;

public class ColorPanel extends JApplet {
	private JComboBox comboBox = 
		new JComboBox(new Object[] {
			new Item(new Object[] {Color.gray, "gray"}),
			new Item(new Object[] {Color.orange, "orange"}),
			new Item(new Object[] {Color.red, "red"}),
			new Item(new Object[] {Color.blue, "blue"}),
			new Item(new Object[] {Color.yellow, "yellow"}),
			new Item(new Object[] {Color.magenta, "magenta"}),
			new Item(new Object[] {Color.black, "black"}),
			new Item(new Object[] {Color.green, "green"}),
			new Item(new Object[] {Color.lightGray, "lightGray"}),
			new Item(new Object[] {Color.white, "white"}),
		});
        //初始化小程序
	public void init() {
		Container contentPane = getContentPane();

		comboBox.setRenderer(new ColorRendererer());
		comboBox.setEditor(new ColorComboBoxEditor());
		comboBox.setEditable(true);

		contentPane.setLayout(new FlowLayout());
		contentPane.add(comboBox);
	}
}
//Item类
class Item {
	private Color color;
	private String string;

	public Item(Object[] array) {
		color = (Color)array[0];
		string = (String)array[1];
	}
	public Color getColor() { return color; }
	public String toString() { return string; }
}
//ColorComboBoxEditor类
class ColorComboBoxEditor extends AbstractComboBoxEditor {
	ColorIcon editorIcon = new ColorIcon();
	JLabel editorLabel = new JLabel(editorIcon);

	Item item;

	JColorChooser colorChooser = new JColorChooser();
	ActionListener okListener = new OKListener();
	Dialog dialog = JColorChooser.createDialog(
					  	null, 	 // 上一级组件
					  	"Choose A Color",// 标题
					  	true, 			 // 模式
					  	colorChooser,
					  	okListener, 
					  	null); 			 //取消监听器

	public ColorComboBoxEditor() {
		editorLabel.setBorder(BorderFactory.createEtchedBorder());

		editorLabel.addMouseListener(new MouseAdapter() {
			public void mousePressed(MouseEvent e) {
				dialog.setVisible(true);
			}
		});
	}
	class OKListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			item = 
				new Item(new Object[] {
							 colorChooser.getColor(), null});

			fireActionPerformed(e);
		}
	}
	public Component getEditorComponent() {
		return editorLabel;
	}
	public Object getItem() {
		return item;
	}
	public void setItem(Object itemToSet) {
		item = (Item)itemToSet;
		
		editorIcon.setColor(item.getColor());
		editorLabel.setText(item.toString());
	}
	public void selectAll() {
	}
}
//AbstractComboBoxEditor类
abstract class AbstractComboBoxEditor implements ComboBoxEditor {
	EventListenerList listenerList = new EventListenerList();

	public void addActionListener(ActionListener listener) {
		listenerList.add(ActionListener.class, listener);
	}
	public void removeActionListener(ActionListener listener) {
		listenerList.remove(ActionListener.class, listener);
	}
	protected void fireActionPerformed(ActionEvent e) {
		//数组定义
		Object[] listeners = listenerList.getListenerList();

                //监听器
		for (int i = listeners.length-2; i>=0; i-=2) {
		 	if (listeners[i]==ActionListener.class) {
				((ActionListener)
					listeners[i+1]).actionPerformed(e);
		 	}	       
		}
	}	
}
//ColorRendererer类
class ColorRendererer extends JLabel implements ListCellRenderer {
	private ColorIcon icon = new ColorIcon();

	public ColorRendererer() {
		setOpaque(true);
		setIcon(icon);
	}
	public Component getListCellRendererComponent(
									JList list,
									Object value,
									int index,
									boolean isSelected,
									boolean cellHasFocus) {
		Item item = (Item)value;

		icon.setColor(item.getColor());
		setText(item.toString());

		if(isSelected) {
			setForeground(list.getSelectionForeground());
			setBackground(list.getSelectionBackground());
		}
		else {
			setForeground(list.getForeground());
			setBackground(list.getBackground());
		}
		return this;
	}
}
//ColorIcon类
class ColorIcon implements Icon {
	private Color color;
	private int w, h;

	public ColorIcon() {
		this(Color.gray, 50, 15);
	}
	public ColorIcon(Color color, int w, int h) {
		this.color = color;
		this.w = w;
		this.h = h;
	}
	public void paintIcon(Component c, Graphics g, int x, int y) {
		g.setColor(Color.black);
		g.drawRect(x, y, w-1, h-1);
		g.setColor(color);
		g.fillRect(x+1, y+1, w-2, h-2);
	}
	public Color getColor() {
		return color;
	}
	public void setColor(Color color) {
		this.color = color;
	}
	public int getIconWidth() {
		return w;
	}
	public int getIconHeight() {
		return h;
	}
}
