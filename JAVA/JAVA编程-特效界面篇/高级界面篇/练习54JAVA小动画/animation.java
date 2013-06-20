//animation.java
import java.awt.*;
import java.applet.Applet;

public class animation extends Applet implements Runnable
 {
  Image hImages[];
  int hFrame=0;
  Thread hThread;
  public void init(){
   //准备每一帧图像
   hImages=new Image[6]; 
   for(	int i=1;
   		i<=6;
   		i++
      )
     hImages[i-1]=getImage(getCodeBase(),"frame"+i+".jpg");
  }
 public void start(){
  //创建线程
  if(hThread==null)
     hThread=new Thread(this); 
  //启动线程
   hThread.start();
 }
public void run(){
  //线程run方法
  while(true) {
   repaint(); 
   try{
    //线程睡眠时间
    hThread.sleep(500);
   }
   catch(InterruptedException e)
    {
    }
   if(hFrame==5) 
   	   hFrame=0;
   else
   	   hFrame++;
  }
 }
 public void paint(Graphics g){
 		update(g); 
 }
 public void update(Graphics g){ 
  g.drawImage(hImages[hFrame],0,0,this); 
 }

  public void stop() 
  {
   hThread= null; 
  } 
}
