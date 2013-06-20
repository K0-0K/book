#include <stdio.h>
#include <string.h>

int m,n;
int a[10][10];
char f[10][10][2010];

// 计算从左上角走到(x, y)的时候所取的数字和能否为d
bool Solve(int x,int y,int d)
{
  // 把结果记录在f[x][y][d+1000], 因为d可能为负数
  char &res=f[x][y][d+1000];

  // 若已经求解, 则直接返回结果
  if(res!=-1) return res;
  // 递归求解结果
  if(!x && !y) return res=(d==a[0][0]);
  if(x && Solve(x-1,y,d-a[x][y])) return res=1;
  if(y && Solve(x,y-1,d-a[x][y])) return res=1;
  // 记录结果并返回
  return res=0;
}

int main()
{
  while(scanf("%d%d",&m,&n)==2) {
    int i;
    for(i=0;i<m;i++)
      for(int j=0;j<n;j++) scanf("%d",&a[i][j]);
    // 全部置为-1, 表示没有被求解
    memset(f,255,sizeof(f));
    int ans=-1;
    // 求最小的可达的正整数
    for(i=1;i<=m*n*20;i++) if(Solve(m-1,n-1,i)) {
      ans=i;break;
    }
    // 输出结果
    printf("%d\n",ans);
  }

  return 0;
}
