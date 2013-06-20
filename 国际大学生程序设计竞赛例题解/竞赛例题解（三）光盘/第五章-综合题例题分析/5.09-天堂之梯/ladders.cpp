#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

struct line{
	short x,y;
	short dx,dy;
}l[2500];

double a[2500];
long n,k,inversion;
short order[2500],TR[2500],c[2500];

int cmp(short *t1,short *t2){
	line *p1=&l[*t1],*p2=&l[*t2];
	double t=(double)(p2->dy)/p2->dx-(double)(p1->dy)/(p1->dx);
	if (t>1e-6) return 1;
	else if (t<-1e-6) return -1;
	else return 0;
}

int cmp2(short *p1,short *p2){
	double t=a[*p1]-a[*p2];
	if (t>1e-9) return 1;
	else if (t<-1e-9) return -1;
	else return 0;
}

void merge(short p[],short q[],long m,long n,long t)
{
	long i=m,j=n+1,k;
	for(k=m;i<=n&&j<=t;k++)
	{
		if (p[i]>=p[j]){
			q[k]=p[j++];
		}else{
			q[k]=p[i++];
			inversion+=j-n-1;
		}
	}
	if (i<=n) for(;i<=n;i++){
		q[k++]=p[i];
		inversion+=j-n-1;
	}
	if (j<=t) for(;j<=t;j++) q[k++]=p[j];
}

void mergepass(short SR[],short t[],long n,long l)
{
	long i,p=0;
	while(p+2*l<=n)
	{
		merge(SR,t,p,p+l-1,p+2*l-1);
		p+=2*l;
	}
	if (p+l<n) merge(SR,t,p,p+l-1,n-1);
	else for(i=p;i<n;i++) t[i]=SR[i];
}

void MSort(short SR[],long n)
{
	long l=1;
	while(l<n){
		mergepass(SR,TR,n,l);
		l*=2;
		mergepass(TR,SR,n,l);
		l*=2;
	}
}

long compute(double lambda)
{
	long i;
	for(i=0;i<n;i++){
		order[i]=i;
		a[i]=(double)l[c[i]].dy*(lambda-l[c[i]].x)/l[c[i]].dx+l[c[i]].y;
	}
	inversion=0;
	qsort(order,n,sizeof(short),(int(*)(const void *,const void *))cmp2);
	MSort(order,n);
	return inversion;
}

void solve()
{
	long i;
	double p=-1000000,q=1000000,t;

	while(p<q-1e-9){
		t=(p+q)/2;
		i=compute(t);
		if (i<k) p=t;
		else q=t;
	}

	for(i=0;i<n;i++){
		order[i]=i;
		a[i]=(double)(l[c[i]].dy)*(t-l[c[i]].x)/l[c[i]].dx+l[c[i]].y;
	}
	qsort(order,n,sizeof(short),(int(*)(const void *,const void *))cmp2);
	for(i=0;i<n-1;i++){
		if (fabs(a[order[i]]-a[order[i+1]])<1e-5) break;
	}

	order[i]=c[order[i]];
	order[i+1]=c[order[i+1]];

	if (i==n-1) printf("Data Error\n");
	else printf("%d %d\n",(order[i]<order[i+1]?order[i]:order[i+1])+1,(order[i]>order[i+1]?order[i]:order[i+1])+1);
}

int main()
{
	freopen("ladders.in","r",stdin);
	freopen("ladders.out","w",stdout);

	long i,x1,x2,y1,y2;

	scanf("%ld%ld",&n,&k);
	for(i=0;i<n;i++){
		scanf("%ld%ld%ld%ld",&x1,&y1,&x2,&y2);
		l[i].x=x1;
		l[i].y=y1;
		l[i].dx=x2-x1;
		l[i].dy=y2-y1;
		c[i]=i;
	}

	qsort(c,n,sizeof(short),(int(*)(const void *,const void *))cmp);

	solve();

	return 0;
}
