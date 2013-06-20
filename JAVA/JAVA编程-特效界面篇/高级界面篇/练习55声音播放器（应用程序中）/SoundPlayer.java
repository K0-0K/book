//SoundPlayer.java
import sun.audio.*;
import java.awt.*;
import java.io.*;
import java.awt.event.*;

public class SoundPlayer extends Frame
							implements FilenameFilter,ActionListener
{
	Button openButton = new Button("Open");
	Button playButton = new Button("Play");
	Button loopButton = new Button("Loop");
	Button stopButton = new Button("Stop");
	
	Label filename = new Label("                  ");
	File theFile = null;
	AudioData theData = null;
	InputStream nowPlaying = null;
	
	public SoundPlayer(){
		//设置标题栏文字
		super("SoundPlayer");
		//关闭窗口时退出程序
		addWindowListener(new WindowAdapter() {	public void windowClosing(WindowEvent e) 
												{ System.exit(0);
												}
										      }
						  );
		//设置窗口大小						  
		setSize(300,200);
		//设置窗口背景色
		setBackground(Color.orange);
		//设置窗口上方面板控件内容
		Panel north = new Panel();
		north.setLayout(new FlowLayout(FlowLayout.LEFT));
		north.add(new Label("File:    "));
		north.add("NORTH",filename);
		add("North",north);
		//设置窗口下方面板控件内容
		Panel south = new Panel();
		south.add(openButton);
		south.add(playButton);
		south.add(loopButton);
		south.add(stopButton);
		add("South",south);
		
		//为按钮添加事件监听
		openButton.addActionListener(this);
		playButton.addActionListener(this);
		loopButton.addActionListener(this);
		stopButton.addActionListener(this);		
	}
	//主函数，应用程序入口处
	public static void main(String[] args){
		SoundPlayer sp = new SoundPlayer();
		sp.show();
	}		
	//打开文件
	public void open(){
		FileDialog fd = new FileDialog(this,"please select a file");
		fd.setFilenameFilter(this);
		fd.show();
		try{
			theFile = new File(fd.getDirectory()+"/"+fd.getFile());
			if(theFile != null){
				filename.setText(theFile.getName());
				FileInputStream fis = new FileInputStream(theFile);
				AudioStream as = new AudioStream(fis);
				theData = as.getData();
			}
		}
		catch(IOException e){
			System.err.println(e);
		}	
		catch(SecurityException e){
			System.out.println("security exception");
		}		
	}
	//开始播放
	public void play(){
		stop();
		if(theData == null) open();
		if(theData != null){
			AudioDataStream ads = new AudioDataStream(theData);
			AudioPlayer.player.start(ads);
			nowPlaying = ads;
		}
	}
	//停止播放
	public void stop(){
		if(nowPlaying != null){
			AudioPlayer.player.stop(nowPlaying);
			nowPlaying = null;
		}
	}		
	//循环播放
	public void loop(){
		stop();
		if(theData == null) open();
		if(theData != null){
			ContinuousAudioDataStream cads 
									  = new ContinuousAudioDataStream(theData);
			AudioPlayer.player.start(cads);
			nowPlaying = cads;
		}	
	}
	//事件响应
	public void actionPerformed(ActionEvent e){
		if(e.getSource() == playButton){
			play();
		}
		if(e.getSource() == openButton){
			open();
		}
		if(e.getSource() == loopButton){
			loop();
		}		
		if(e.getSource() == stopButton){
			stop();
		}		
	}
	public boolean accept(File dir,String name){
		return true;
	}
}							
