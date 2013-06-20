package j1viewer;
import java.awt.*;
import java.io.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.table.*;
/*
 * ImageNamesTableModel.java
 *
 * Created on March 15, 2005, 12:57 PM
 */

/**
 *
 * @author cmlee
 */
public class ImageNamesTableModel 
    extends AbstractTableModel 
    implements TableCellRenderer {
    //implements TableModel {
    
    public static final int TABLE_COLS = 1;
    
    protected String dirname;
    protected ImageIcon[] images;
    protected String[] imagesName;
    protected TableImagePanel[] tableImagePanel;
    protected int currRow, currColumn;
    
    public class TableImagePanel extends JPanel {
        
        public File imageFileName;
        public JLabel text;
        
        public TableImagePanel(File f) {
            imageFileName = f;
            setLayout(new FlowLayout(FlowLayout.LEFT));
            text = new JLabel(f.getName());
            add(text);         
            setToolTipText("<html><img src=\"file://" + f.getAbsolutePath()
                    + "\" width=\"148\" height=\"100\""
                    + " vspace=\"3\" hspace=\"3\">" + "</html>");
        }
        
        public void setSelected(boolean b) {
            if (b)
                text.setForeground(Color.RED);
            else
                text.setForeground(Color.BLACK);
        }
    }
        
    public class _ImageLoader
        extends Thread {
        
        public void run() {
            File f = new File(dirname);
            String[] fn = f.list();
            tableImagePanel = new TableImagePanel[fn.length];
            
            for (int i = 0; i < fn.length; i++) 
                tableImagePanel[i] = new TableImagePanel(new File(dirname + File.separator + fn[i]));            
            
            fireTableDataChanged();         
        }
    }
    
    /** Creates a new instance of ImageNamesTableModel */
    public ImageNamesTableModel(String n) {
        dirname = n;
        _ImageLoader loader = new _ImageLoader();
        //loader.start();
        loader.run();
    }       
    public ImageNamesTableModel() {
	 	dirname = "";
	 }

    public Class getColumnClass(int c) {
        return (TableImagePanel.class);
    }
    
    public int getColumnCount() {
        return (TABLE_COLS);
    }
    
    public String getColumnName(int i) {
        return ("Image name");
    }
    
    public int getRowCount() {               
        
        return ((tableImagePanel != null)? tableImagePanel.length: 0);
    }
    
    public TableImagePanel getValueAt(int r, int c) {
        return (tableImagePanel[r]);
    }
    
    public String getImageName(int r, int c) {
        return (tableImagePanel[r].imageFileName.getAbsolutePath());
    }
    
    public boolean isCellEditable(int r, int c) {
        return (false);
    }
    
    public void setValueAt(Object o, int r, int c) {       
    }

    public Component getTableCellRendererComponent(JTable jTable, Object obj
                    , boolean selected, boolean focus, int row, int col) {
        
        tableImagePanel[row].setSelected(selected);
        
        return (tableImagePanel[row]);
    }
}
