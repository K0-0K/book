//ClipboardTest2.java
import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;

//主类声明，用到了接口ClipboardOwner
public class ClipboardTest2 extends    Applet 
                           implements ClipboardOwner {
	//变量定义                           	
	private Clipboard   clipboard;
	private ImageCanvas copyFrom = new ImageCanvas();
	private ImageCanvas copyTo   = new ImageCanvas();
	private Button copy  = new Button("复制");
	private Button paste = new Button("粘贴");

	//初始化函数
	public void init() {
		clipboard = new Clipboard("image clipboard");
		//得到要复制的图片
		copyFrom.setImage(getImage(getCodeBase(),"skelly.gif"));
		add(copyFrom);
		add(copyTo);
		add(copy);
		add(paste);
		//添加事件监听
		copy.addActionListener (new CopyListener());
		paste.addActionListener(new PasteListener());
	}
	//拷贝类
	class CopyListener implements ActionListener {
		public void actionPerformed(ActionEvent event) {
			////实例化ImageSelection类
			ImageSelection contents = 
						new ImageSelection(copyFrom.getImage());
	
			clipboard.setContents(contents, ClipboardTest2.this);
		}
	}
	//粘贴类
	class PasteListener implements ActionListener {
		public void actionPerformed(ActionEvent event) {
			Transferable contents = clipboard.getContents(this);

			if(contents != null && 
				contents.isDataFlavorSupported(
					ImageSelection.ImageFlavor)) {
				try {
					Image image;
					image = (Image) contents.getTransferData(
							 ImageSelection.ImageFlavor);
					copyTo.setImage(image);
				}
				catch(Exception e) {
					e.printStackTrace();
				}
			}
		}
	}
	public void lostOwnership(Clipboard    clip, 
	                          Transferable transferable) {
		System.out.println("Lost ownership");
	}
}
//画布类
class ImageCanvas extends Panel {
    private Image image;
	//不带参数的构造函数
	public ImageCanvas() {
		this(null);
	}
	//带参数的构造函数
    public ImageCanvas(Image image) {
		if(image != null)
			setImage(image);
    }
    //画屏函数
    public void paint(Graphics g) {
		g.setColor(Color.lightGray);

		g.draw3DRect(0,0,getSize().width-1,
		                 getSize().height-1,true);

		if(image != null) {
        	g.drawImage(image, 1, 1, this);
		}
    }
    //更新函数
    public void update(Graphics g) {
        paint(g);
    }
    //设置图片函数
	public void setImage(Image image) {
		this.image = image;
		try {
			MediaTracker tracker = new MediaTracker(this);
			tracker.addImage(image, 0);
        	tracker.waitForID(0);
		}
		catch(Exception e) { e.printStackTrace(); }

		if(isShowing()) {
			repaint();
		}
	}
	//得到图片函数
	public Image getImage() {
		return image;
	}
	public Dimension getPreferredSize() {
		return new Dimension(100,100);
	}
}
