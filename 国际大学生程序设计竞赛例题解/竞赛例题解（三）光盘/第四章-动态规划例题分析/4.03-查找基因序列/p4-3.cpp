#include<stdio.h>
#include<string.h>
const int num=101;
long score[num][num];
char a[101],         //给定目标序列
     b[101],         //数据库序列
     seq[101];       //得到的最相似序列
int dp()
{
	long i,j,m,n;
	m=strlen(a);
	n=strlen(b);
	//初始化
	for(i=0;i<=m;i++)score[i][n]=(m-i)*(-7);
	for(j=0;j<=n;j++)score[m][j]=(n-j)*(-7);
	//由后向前动态规划
	for(i=m-1;i>=0;i--){
		for(j=n-1;j>=0;j--){
			if(a[i]!=b[j])score[i][j]=score[i+1][j+1]-4;
			else score[i][j]=score[i+1][j+1]+5;
			if(score[i+1][j]-7>score[i][j])
				score[i][j]=score[i+1][j]-7;
			if(score[i][j+1]-7>score[i][j])
				score[i][j]=score[i][j+1]-7;
		}
	}
	return score[0][0];
}
int main()
{
	freopen("sequence.in","r",stdin);
	freopen("sequence.out","w",stdout);
	long i,n,max,p;
	scanf("%s %ld",a,&n);
	for(i=0;i<n;i++){
		scanf("%s",b);
		p=dp();
		if(i==0||
			p>max||
			(p==max&&strcmp(b,seq)<0)){
			strcpy(seq,b);
			max=p;
		}
	}
	printf("%ld\n%s\n",max,seq);
	return 0;
}
