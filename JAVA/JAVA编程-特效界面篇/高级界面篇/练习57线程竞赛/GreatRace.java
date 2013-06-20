//GreatRace.java
import java.awt.Graphics;
import java.awt.GridLayout;
import java.awt.Frame;

public class GreatRace extends java.applet.Applet implements Runnable{
  Threader theRacers[];
  static int racerCount = 3;
  Thread    theThreads[];
  Thread    thisThread;
  static boolean inApplet=true;
  int    numberofThreadsAtStart;

  public void init(){
    //计算工作线程的个数
    numberofThreadsAtStart = Thread.activeCount();
    //确定界面的显示风格
    setLayout(new GridLayout(racerCount,1));
    //确定竞赛者的数量
    theRacers = new Threader [racerCount];
    theThreads = new Thread[racerCount];
    //为每一个竞赛者创建一个线程
    for (int x=0;x<racerCount;x++){
      theRacers[x]=new Threader ("Racer #"+x);
      theRacers[x].setSize(getSize().width,getSize().height/racerCount);
      add (theRacers[x]);
      theThreads[x]=new Thread(theRacers[x]);

    }
  }

  public void start(){
    //启动所有的线程
    for (int x=0;x<racerCount;x++)
      theThreads[x].start();
    //创建一个对照线程
    thisThread= new Thread (this);
    thisThread.start();
  }

  public void stop(){    
    for (int x= 0;x<theRacers.length;x++){
      theRacers[x].stop();
    }
  }
  
  public void run(){
    //循环直到结束
    while(Thread.activeCount()>numberofThreadsAtStart+2){
      try{
        thisThread.sleep(100);
      }  catch (InterruptedException e){
        System.out.println("thisThread was interrupted");
      }
    }

    //停止竞赛
    if (inApplet){
      stop();
      destroy();
    }
    else
      System.exit(0);
  }


  public static void main (String argv[]){
    inApplet=false;

    //监测在线竞赛者的数量.
    if (argv.length>0)
      racerCount = Integer.parseInt(argv[0]);

    //创建一个新的界面
    Frame theFrame = new Frame("The Great Thread Race");
    GreatRace theRace = new GreatRace();
    theFrame.setSize(400,200);
    theFrame.add ("Center",theRace);
    theFrame.show();
    theRace.init();
    theFrame.pack();
    theRace.start();
  }

}//end class GreatRace. 
