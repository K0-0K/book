#include<fstream.h>
ifstream in("coding.in");
ofstream out("coding.out");

#define NMAX 50
#define MMAX 20
int     n;
int     num[NMAX];
int     st[NMAX];
long    ans[MMAX];

void cal()
{
  int  i,j;
  long temp,advance;

  for(ans[0]=st[0],i=1;i<MMAX;i++)  ans[i]=0;
  for(i=n-1;i>=2;i--){
      for(advance=st[n-i],j=0;j<MMAX;j++){
	     temp=ans[j]*i+advance;
		 ans[j]=temp%10000;
		 advance=temp/10000;
	  }
  }
  for(advance=1,j=0;j<MMAX;j++){
     temp=ans[j]+advance;
	 if(temp>=10000){ ans[j]=temp-10000; advance=1;} else{ ans[j]=temp; break;}
  }
}

void print(int flag)
{
  int  i;
  if(!flag) out<<',';
  for(i=MMAX-1;i>0;i--) if(ans[i]) break;
  out<<ans[i];
  for(i--;i>=0;i--) out<<(ans[i]/1000)<<(ans[i]%1000)/100<<(ans[i]%100)/10<<ans[i]%10;
}

int  main()
{
  int  i,j;
  int  IsFirstCase;
  char ch;
  IsFirstCase=1;
  while(in>>ch){
	  if(ch=='-') {in>>ch;break;}
	  //输入数据
	  in>>n>>ch>>ch;
	  for(i=0;i<n;i++) in>>num[i]>>ch;
	  in>>ch;
	  //预处理
	  for(i=0;i<n;i++) 
		  for(st[i]=0,j=i+1;j<n;j++) if(num[j]<num[i]) st[i]++;
	  cal();
	  print(IsFirstCase);
	  IsFirstCase=0;
  }
  return 0;
}
