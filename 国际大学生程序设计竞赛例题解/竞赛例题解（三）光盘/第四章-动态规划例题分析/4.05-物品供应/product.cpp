#include<stdio.h>
#include<iostream.h>
long n;
long p[30],s[30],d[30],r[30];
long a[21][510];
void process(){
	cin>>n;
	long i,j;
	for(i=1;i<=n;i++)
		for(j=0;j<=500;j++)
			a[i][j]=0;

	for(i=1;i<=n;i++)
		cin>>p[i]>>s[i]>>d[i]>>r[i];

	for(j=0;j<=500;j++)
		a[1][j]=r[1]+(j+d[1])*s[1];

	if(d[1]==0)
		a[1][0]=0;

	long t,m,min;
	for(t=2;t<=n;t++)
	for(j=0;j<=500;j++)
	{
		min=-1;
		for(m=0;m<=500;m++)
		{
			i=a[t-1][m]+m*p[t-1];
			if(j+d[t]-m>0)
				i=i+r[t]+s[t]*(j+d[t]-m);
			if(min<0||i<min)
				min=i;
		}
		a[t][j]=min;
	}
	min=a[n][0];
	for(i=0;i<=500;i++)
		if(a[n][i]<min)
			min=a[n][i];
		cout<<min<<endl;
}
long main(){
	long t;
	freopen("product.in","r",stdin);
	freopen("product.out","w",stdout);
	cin>>t;
	while(t--)
	{
	process();
	}
	return 0;
}