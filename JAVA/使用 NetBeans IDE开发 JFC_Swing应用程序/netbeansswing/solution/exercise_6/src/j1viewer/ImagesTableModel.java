package j1viewer;
import java.awt.*;
import java.io.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.table.*;
/*
 * ImagesTableModel.java
 *
 * Created on March 15, 2005, 12:57 PM
 */

/**
 *
 * @author cmlee
 */
public class ImagesTableModel
        extends AbstractTableModel 
        implements TableCellRenderer {
    //implements TableModel {
    
    private static final int TABLE_COLS = 3;
    public static final int THUMBNAIL_SIZE = 90;
    
    protected String dirname;
    protected _ImageTablePanel[][] images;
    protected int currRow, currColumn;
    
    public class _ImageTablePanel extends JPanel {
        private File fileName;
        private int w;
        private int h;
        private boolean imageLoaded = false;
        
        public _ImageTablePanel(String f, Toolkit toolkit) {
            fileName = new File(f);
            setLayout(new BorderLayout());
            Image img = toolkit.createImage(f);
            
            synchronized (this) {
                imageLoaded = false;
                w = img.getWidth(this);  // we need to register this class as an ImageObserver
            }
            
            synchronized (this) {
                int t = 0;
                while (!imageLoaded) {
                    t++;
                    try {
                        wait(100);
                        if (t >= 10)
                            imageLoaded = true; // don't wait for than a second for an image
                    } catch (InterruptedException ie) {
                    }
                }
            }
            
            if (w >= h) {
                img = img.getScaledInstance(THUMBNAIL_SIZE, -1, Image.SCALE_FAST);
            } else {
                img = img.getScaledInstance(-1, THUMBNAIL_SIZE, Image.SCALE_FAST);
            }

            add(new JLabel(new ImageIcon(img)), BorderLayout.CENTER);
            f = f.toLowerCase();
            StringBuilder str = new StringBuilder();
            str.append("<html>");
            str.append("Filename: ").append(fileName.getName()).append("<br>");
            str.append("Type: ");
            if (f.endsWith(".jpg"))
                str.append("JPEG");
            else if (f.endsWith(".gif"))
                str.append("GIF");
            else if (f.endsWith(".tiff")) 
                str.append("TIFF");
            else if (f.endsWith(".bmp"))
                str.append("BMP");
            else if (f.endsWith(".png"))
                str.append("PNG");
            else
                str.append("Not supported");
            str.append("<br>");
            str.append("Size:" + w + "x" + h + " (" + fileName.length() + " bytes)");
            str.append("</html>");
                
            setToolTipText(str.toString());
        }
        
        public boolean imageUpdate(Image img, int infoflags, int x, int y, int width, int height) {
            if (width < 0 || height < 0) {
                return (true);
            } else {
                synchronized(this) {
                    w = width;
                    h = height;
                    imageLoaded = true;
                }
                return (false);
            }
        }
    }
    
    public class _ImageLoader extends Thread {
        
        public void run() {
            Toolkit toolkit = Toolkit.getDefaultToolkit();
            File f = new File(dirname);
            String[] fn = f.list();
            // Bubble sort the list of filenames
            for (int b = 0; b < (fn.length - 2); b++) {
                for (int c = 0; c < (fn.length - b - 1); c++) {
                    if (fn[c].compareToIgnoreCase(fn[c + 1]) > 0) {
                        String temp = fn[c + 1];
                        fn[c + 1] = fn[c];
                        fn[c] = temp;
                    }
                }
            }
            
            boolean lastRow = ((fn.length % TABLE_COLS) > 0);
            int row = (fn.length / TABLE_COLS) + (lastRow? 1: 0);
            int col;
            images = new _ImageTablePanel[row][TABLE_COLS];
            for (int i = 0; i < fn.length; i++) {
                col = i % TABLE_COLS;
                row = i / TABLE_COLS;
                images[row][col] = new _ImageTablePanel((dirname + File.separator + fn[i]), toolkit);
            }
            
            fireTableDataChanged();
        }
    }
        
    public ImagesTableModel(String n) {        
        dirname = n;

        _ImageLoader loader = new _ImageLoader();
        // leave multithreading for later
        // loader.start();
        loader.run();
    }
    
    public ImagesTableModel() {
        dirname = "";
        images = new _ImageTablePanel[0][0];
    }
    
    public Class getColumnClass(int c) {
        return (_ImageTablePanel.class);
    }
    
    public int getColumnCount() {
        return (TABLE_COLS);
    }
    
    public String getColumnName(int i) {
        return ("Image");
    }
    
    public int getRowCount() {
        
        return (images.length);
    }
    
    public _ImageTablePanel getValueAt(int r, int c) {
        return (images[r][c]);
    }
    
    public String getImageName(int r, int c) {
        if (images[r][c] == null)
            return ("");
        
        return (images[r][c].fileName.getAbsolutePath());
    }
    
    public boolean isCellEditable(int r, int c) {
        return (false);
    }
    
    public void setValueAt(Object o, int r, int c) {
    }

    public Component getTableCellRendererComponent(JTable jTable, Object obj, boolean selected, boolean hasFocus
                    , int row, int col) {
        
        return (images[row][col]);
    }    
}
