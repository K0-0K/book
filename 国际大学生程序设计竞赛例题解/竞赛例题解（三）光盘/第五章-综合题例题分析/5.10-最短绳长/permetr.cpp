#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include<assert.h>

const int MAXN = 100;
const double PI2 = acos(-1.0) * 2.0;
const double EPS = 1e-5;

struct REC{
	double x, y, r;
}list[MAXN];
int ptr[2*MAXN];
char flag[MAXN][MAXN];

inline double arctan(const double& dy, const double& dx)
{
	double t = atan2(dy, dx);
	if (t<0){
		t+=PI2;
	}
	return t;
}

void normalize(double &ang)
{
	if (ang<0){
		ang+=PI2;
	} else if (ang>PI2){
		ang-=PI2;
	}
	assert(ang>=0.0 && ang<=PI2);
}

double distance(REC& a, REC& b)
{
	return sqrt( (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) );
}

double calc(REC& a, REC& b, REC& c)
{
	double ret=0.0;
	double ang_in, ang_out;
	double d, ang, deltaR, l, theta;
	ang = arctan(a.y - b.y, a.x - b.x);
	d = distance(a, b);
	deltaR = a.r - b.r;
	assert(d>1e-8);
	theta = asin(deltaR/d);
	ang_in = ang + theta + PI2/4.0;
//	normalize(ang_in);

	ang = arctan(c.y - b.y, c.x - b.x);
	d = distance(b, c);
	deltaR = c.r - b.r;
	theta = asin(deltaR/d);
	ang_out = ang - theta - PI2/4.0;

	if (ang_out<ang_in){
		ang_out+=PI2;
	}
	assert(ang_out - ang_in < PI2+EPS);	//impossible wrap around
	ret = b.r * (ang_out - ang_in);
	l = sqrt( d*d - deltaR*deltaR );
	return ret+l;
}

int main()
{
	freopen("permetr.in", "r", stdin);
	freopen("permetr.out", "w", stdout);

	int N, i;
	scanf("%d", &N);
	// locate the starting point
	double ymin=1e50, xmin=1e50;
	int ori=-1;
	for (i=0; i<N; i++){
		scanf("%lf %lf %lf", &list[i].x, &list[i].y, &list[i].r);
		if (list[i].y-list[i].r < ymin){
			ymin = list[i].y - list[i].r;
			xmin = list[i].x;
			ori = i;
		} else if (list[i].y - list[i].r < ymin+EPS &&
				list[i].x < xmin){
			xmin = list[i].x;
			ori = i;
		}
	}

	int p;
	double last = -1.0;
	memset(flag, 0, sizeof(flag));
	ptr[0] = ori;
//	printf("%.3lf %.3lf %.3lf\n", list[ori].x, list[ori].y, list[ori].r);
	for (p = 1; ; p++){
		int sel = -1;
		double amin = 1.0e10, len = -1.0;
		double d, deltaR, ang, theta, l;
		//printf("%d\n", ptr[p-1]);
		for (i=0; i<N; i++){
			if (ptr[p-1]==i) continue;
			ang = arctan(list[i].y - list[ptr[p-1]].y, list[i].x - list[ptr[p-1]].x);
			d = distance(list[i], list[ptr[p-1]]);
			deltaR = list[i].r - list[ptr[p-1]].r;
			assert(d>1e-8);
			if (d<1e-8){
				printf("aoao\n");
			}
			theta = asin(deltaR/d);
			ang -= theta;
			normalize(ang);
			l = sqrt( d*d - deltaR*deltaR );
			if (ang<last){
				ang += PI2;
			}
			if (ang < amin - EPS){
				amin = ang;
				len = l;
				sel = i;
			} else if(ang < amin+EPS){
				if (l > len){
					len = l;
					sel = i;
				}
			}
		}
		assert(sel >= 0);
		last = amin;
//		printf("%.3lf %.3lf %.3lf\n", list[sel].x, list[sel].y, list[sel].r);
		ptr[p] = sel;
		if (flag[ptr[p-1]][ptr[p]]){
			break;
		}
		flag[ptr[p-1]][ptr[p]] = 1;
		assert(p<2*MAXN-1);///
	}
	N = p-1;
	assert(N>1);
	double acc;
	acc = calc(list[ptr[N-1]], list[ptr[0]], list[ptr[1]]);
	for (i=1; i<N; i++){
		acc += calc(list[ptr[i-1]], list[ptr[i]], list[ptr[i+1]]);
	}
	printf("%.4e\n", acc);
	return 0;
}

