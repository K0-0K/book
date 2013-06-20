//Threader.java
import java.awt.Graphics;
import java.awt.Color;

public class Threader extends java.awt.Canvas implements Runnable {
  int myPosition =0;
  String myName;
  int numberofSteps=600;
  boolean keepRunning = true;
  //构造函数
  public Threader (String inName){
    myName=new String (inName);
  }

  public synchronized void paint(Graphics g){
    //为线程竞赛画一条直线
    g.setColor (Color.black);
    g.drawLine (0,getSize().height/2,getSize().width,getSize().height/2);

    //画竞赛者.
    g.setColor (Color.yellow);
    g.fillOval((myPosition*getSize().width/numberofSteps),0,15,getSize().height);
  }

  public void stop(){
    keepRunning = false;
  }

  public void run(){
    //循环直到竞赛停止
    while ((myPosition <numberofSteps)&& keepRunning){
      myPosition++;
      repaint();

      //将当前线程睡眠，画屏函数工作.
      try{
        Thread.currentThread().sleep(10);
      }catch (Exception e){System.out.println("Exception on sleep");}
    }
    System.out.println("Threader:"+myName+" has finished the race");
  }


}//end class Threader. 
