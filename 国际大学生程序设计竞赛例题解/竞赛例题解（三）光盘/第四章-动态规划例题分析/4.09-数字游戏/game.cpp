#include<stdio.h>
#include<stdlib.h>
#include<string.h>
struct Tnum
{
	int a,b;
};
Tnum num[210];
int sum[210][210];
int mark[210][210];
int n,m;

void init()
{
	int i;
	scanf("%d%d",&n,&m);
	for (i=0; i<n; i++)
		scanf("%d",&num[i].a);
	for (i=0; i<n; i++)
		scanf("%d",&num[i].b);
}

int cmp(const void *a,const void *b)
{
	Tnum *aa=(Tnum *)a;
	Tnum *bb=(Tnum *)b;
	return bb->b-aa->b;
}

void work()
{
	int i,j,k,t,ans;
	qsort(num,n,sizeof(num[0]),cmp);
	memset(mark,0,sizeof(mark));
	for (i=0; i<n; i++)
	{
		mark[i][1]=1;
		sum[i][1]=num[i].a;
		for (j=0; j<i; j++)
			for (k=1; k<m; k++)
				if (mark[j][k] && (mark[i][k+1]==0 || sum[j][k]+num[i].a-num[i].b*k>sum[i][k+1]))
				{
					mark[i][k+1]=1;
					sum[i][k+1]=sum[j][k]+num[i].a-num[i].b*k;
				}
	}
	t=0;
	for (i=0; i<n; i++)
		if (mark[i][m])
			if (t==0 || ans<sum[i][m])
			{
				t=1;
				ans=sum[i][m];
			}
	printf("%d\n",ans);
}

int main()
{
	freopen("game.in","r",stdin);
	freopen("game.out","w",stdout);
	init();
	work();
	return 0;
}