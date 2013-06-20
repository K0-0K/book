#include<fstream.h>
ifstream filein("guess.in");
ofstream fileout("guess.out");
int n,m,g[18][210];
int main(){
  filein>>n;
  int i,j,t;
  for(i=2,m=1;i<n;i*=2,m++);
  for(i=1;i<=m;i++)for(j=0;j<=n;j++)g[i][j]=0;
  for(i=1;i<=n;i++){
    t=i-1;
    for(j=1;j<=m;j++){
      if(t%2==1)g[j][i]=1;
      t/=2;
    }
  }
  fileout<<m<<endl;
  for(i=1;i<=m;i++){
    t=0;
    for(j=1;j<=n;j++)if(g[i][j]==1)t++;
    fileout<<t;
    for(j=1;j<=n;j++)if(g[i][j]==1)fileout<<' '<<j;
    fileout<<endl;
  }
  return 0;
}