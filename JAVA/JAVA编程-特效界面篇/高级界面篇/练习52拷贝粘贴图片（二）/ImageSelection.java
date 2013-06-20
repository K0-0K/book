//ImageSelection.java
import java.awt.*;
import java.awt.datatransfer.*;
import java.io.*;

public class ImageSelection implements Transferable,
                                       ClipboardOwner {
    //变量定义                                       	
	static public DataFlavor ImageFlavor;

	private DataFlavor[] flavors = {ImageFlavor}; 
	private Image image;
	//静态函数
	static {
		try {
			ImageFlavor = new DataFlavor(
				Class.forName("java.awt.Image"), "AWT Image");
		}
		catch(ClassNotFoundException e) {
			e.printStackTrace();
		}
	}
	//选择图片的构造函数
	public ImageSelection(Image image) {
		this.image = image;
	}
	//传递数据
	public synchronized DataFlavor[] getTransferDataFlavors() {
		return flavors;
	}
	//是否支持当前数据格式
	public boolean isDataFlavorSupported(DataFlavor flavor) {
		return flavor.equals(ImageFlavor);
	}
	//得到当前数据
	public synchronized Object getTransferData(DataFlavor flavor) 
		throws UnsupportedFlavorException, IOException {
		//如果支持当前数据格式	
		if(flavor.equals(ImageFlavor)) {
			return image;
		}
		//如果不支持当前数据格式
		else {
			throw new UnsupportedFlavorException(flavor);
		}
	}
	public void lostOwnership(Clipboard c, Transferable t) {
	}
}
