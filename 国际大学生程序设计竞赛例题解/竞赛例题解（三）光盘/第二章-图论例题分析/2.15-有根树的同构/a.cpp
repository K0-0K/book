#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int n,k,root[100]; // root记录每棵树的根
int *list[100][50],deg[100][50];
     // list存放所有的树，deg存放每棵树每个节点的度

//读数据
void read_data()
{
	char flag[100];    // flag标记每个节点是否有入度
	int v1[100],v2[100],i,j;  // (v1[i],v2[i])是树的一条边
	scanf("%d%d",&k,&n);
	for(i=0;i<k;i++){                    // 逐行读入每一棵树
		memset(deg[i],0,sizeof(deg[i]));
		memset(flag,0,sizeof(flag));
		for(j=0;j<n-1;j++){              // 读入树的n-1条边
			scanf("%d%d",&v1[j],&v2[j]);
			v1[j]--,v2[j]--;
			deg[i][v1[j]]++;      // 统计：deg[i][v]表示第i棵树中节点v的度
			flag[v2[j]]=1;
		}
		for(j=0;j<n;j++) if (!flag[j]) root[i]=j;   // 寻找该树的根
		for(j=0;j<n;j++) list[i][j]=new int[deg[i][j]];   // 申请空间存放树
		memset(deg[i],0,sizeof(deg[i]));
		for(j=0;j<n-1;j++) list[i][v1[j]][deg[i][v1[j]]++]=v2[j];
         // 把树以邻接表形式寸在deg中
	}
}

//交换两个数
void swap(int &a,int &b){
	int c;
	c=a,a=b,b=c;
}

//比较编号为t1的树中以p1为根的子树与编号为t2的树中以p2为根的子树的大小
//如果前者大则返回1，后者大返回-1，相等返回0
int compare(int t1,int p1,int t2,int p2)
{
	int i,j;
	if (deg[t1][p1]>deg[t2][p2]) return 1;       // 先比较度的大小
	else if (deg[t1][p1]<deg[t2][p2]) return -1;
	else{                         // 度相同的情况下递归地比较每棵子树
		if (deg[t1][p1]==0) return 0;
		for(i=0;i<deg[t1][p1];i++){
			j=compare(t1,list[t1][p1][i],t2,list[t2][p2][i]);
			if (j!=0) return j;
		}
		return 0;    // 所有子树都相同，那么这两棵树也是相同的
	}
}

//把编号为t的树中以p为根的子树排序，求出其最小表示
void sort(int t,int p)
{
	int i,j;
     // 先对每棵子树排序，取得各自最小的表示
	for(i=0;i<deg[t][p];i++) sort(t,list[t][p][i]); 
     // 再对所有子树排序
	for(i=0;i<deg[t][p];i++)
		for(j=i+1;j<deg[t][p];j++){
			if (compare(t,list[t][p][i],t,list[t][p][j])==1) swap(list[t][p][i],list[t][p][j]);
		}
}

void solve()   // 主过程
{
	int i,j;
	char flag[100];
	
	//排序
	for(i=0;i<k;i++) sort(i,root[i]);
	
	//扫描
	memset(flag,0,sizeof(flag));  // flag标记树是否已经打印
	for(i=0;i<k;i++) if (!flag[i]){   // 逐一判断每棵还没打印的树
		flag[i]=1;
		printf("%d",i+1);
        // 寻找和i相同的树
		for(j=1;j<k;j++) if (!flag[j]&&compare(i,root[i],j,root[j])==0){
			flag[j]=1;
			printf("=%d",j+1);
		}
		printf("\n");
	}
}

// 主程序
int main()
{
	freopen("Trees.in","r",stdin);    //指定输入输出文件
	freopen("Trees.out","w",stdout);
	read_data();    //读入数据
	solve();        //主过程
	fclose(stdin); //{关闭文件}
	fclose(stdout); //{关闭文件}
	return 0;
}
