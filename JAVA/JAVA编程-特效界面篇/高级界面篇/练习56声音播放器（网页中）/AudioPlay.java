//AudioPlay.java 
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.applet.*;
public class AudioPlay extends JApplet
implements ItemListener, ActionListener
  {
  private AudioClip audio1, audio2, audio;  
  private JButton play, loop, stop;
  private String item[] = {"music", "bird"};
  private JComboBox comboBox = new JComboBox(item);
  
  //界面设计和程序的初始化
  public void init()
    {
    //实例化容器类
    Container c = getContentPane();
    c.setLayout(new FlowLayout(FlowLayout.CENTER));
    comboBox.addItemListener(this);
    c.add(comboBox);
    c.setBackground(Color.black);
    play = new JButton("播放");
    play.addActionListener(this);
    c.add(play);
    loop = new JButton("循环");
    loop.addActionListener(this);
    c.add(loop);
    stop = new JButton("停止");
    stop.addActionListener(this);
    c.add(stop);
    audio1 = getAudioClip(getDocumentBase(), "music.au");
    audio2 = getAudioClip(getDocumentBase(), "bird.au");
    audio = audio1;
    }
  //停止播放
  public void stop()
    {
    audio.stop();
    }
  //选择下拉框时产生的事件
  public void itemStateChanged(ItemEvent e)
    {
    audio.stop();
    audio = comboBox.getSelectedIndex()==0 ? audio1:audio2;
	}
  //响应按钮事件
  public void actionPerformed(ActionEvent e)
    {
    if (e.getSource()==play)
      audio.play();
    else if (e.getSource()==loop)
      audio.loop();
    else if (e.getSource()==stop)
      audio.stop();
    }
  }