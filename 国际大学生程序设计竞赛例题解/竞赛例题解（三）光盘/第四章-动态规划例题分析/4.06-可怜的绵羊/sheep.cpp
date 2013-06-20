#define maxn 101
#include <stdio.h>
#include <math.h>
#include <string.h>

struct point {
	long x,y;
};
long n,m=0;
point vertex[maxn],insd[maxn];
int out[maxn][maxn],online[maxn][maxn];

long area(point a,point b,point c)
{
	return labs(a.x*(b.y-c.y)+b.x*(c.y-a.y)+c.x*(a.y-b.y));
}

long multiple(point a,point b,point o)
{
	return (a.x-o.x)*(b.y-o.y)-(a.y-o.y)*(b.x-o.x);
}

void prepare()
{
	long i,j,k,tmp;
	memset(out,0,sizeof(out));
	memset(online,0,sizeof(online));
	for (i=1;i<=n;i++)
	for (j=1;j<=n;j++)
	if (i!=j)
		for (k=1;k<=m;k++)
		{
			tmp=multiple(insd[k],vertex[j],vertex[i]);
			if (tmp>=0) out[i][j]++;
			if (tmp==0) online[i][j]=1;
		}
}

void dp()
{
	long u,i,j;
	long a[maxn],tmp,max=0;
	for (u=1;u<=n-2;u++)
	{
		memset(a,0,sizeof(a));
		for (i=u+1;i<=n-1;i++)
		for (j=i+1;j<=n;j++)
		if (out[u][i]+out[i][j]+out[j][u]==m) {
			tmp=area(vertex[u],vertex[i],vertex[j]);
			if (tmp>a[j]) a[j]=tmp;
			if (online[u][i]==0) {
				tmp+=a[i];
				if (tmp>a[j]) a[j]=tmp;
			}
		}
		for (j=u+2;j<=n;j++)
			if (a[j]>max) max=a[j];
	}
	if (max==0) printf("die\n");
		else printf("%.2lf\n",max/2.);
}

void main()
{
	freopen("sheep.in","r",stdin);
	freopen("sheep.out","w",stdout);
	scanf("%ld",&n);
	long i,j,tm,s1=0,s2;
	for (i=1;i<=n;i++)
		scanf("%ld%ld",&vertex[i].x,&vertex[i].y);
	for (i=2;i<n;i++)
		s1+=area(vertex[1],vertex[i],vertex[i+1]);
	scanf("%ld",&tm);
	for (i=1;i<=tm;i++)
	{
		scanf("%ld%ld",&insd[0].x,&insd[0].y);
		s2=0;
		for (j=1;j<=n;j++)
			s2+=area(insd[0],vertex[j],vertex[j%n+1]);
		if (s1==s2) insd[++m]=insd[0];
	}
	prepare();
	dp();
	fclose(stdout);
}