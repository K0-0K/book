#include <stdio.h>
#include <string.h>
#include <time.h>

long n,c,k,best;
long b[500];
unsigned char a[200][200];

long min(long a,long b){
	return a<b?a:b;
}

inline long f(long x){
	return x<n?x:x-n;
}

unsigned char dp(long s,long t)
{
	long i,j;
	if (a[s][t]<255) return a[s][t];
	a[s][t]=250;
	if (s<t) j=t-s+1;
	else j=t+n-s+1;
	if (j<=k){
		if (b[s]==b[t]) a[s][t]=dp(s,f(t+n-1));
		else{
			for(i=0;i<j;i++){
				if (b[s]==b[f(s+i)]) a[s][t]=min(a[s][t],dp(s,f(s+i))+dp(f(s+i+1),t));
			}
		}
	}else{
		for(i=0;i<k;i++){
			if (b[s]==b[f(s+i)]) a[s][t]=min(a[s][t],dp(s,f(s+i))+dp(f(s+i+1),t));
		}
	}
	return a[s][t];
}

int main()
{
	freopen("spring.in","r",stdin);
	freopen("spring.out","w",stdout);

	long i,j;
	double t1,t2;

	t1=clock();

	while(scanf("%d%d%d",&n,&c,&k)==3){
		for(i=0;i<n;i++) scanf("%d",&b[i]);

		best=n;
		memset(a,0xff,sizeof(a));
		for(i=0;i<n;i++) a[i][i]=1;
		for(i=0;i<n;i++){
			j=dp(i,f(i+n-1));
			best=min(best,j);
		}

		printf("%d\n",best);
	}

	t2=clock();
//	printf("%.3lf\n",(t2-t1)/CLK_TCK);

	return 0;
}
