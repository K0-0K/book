#include <cstdio>
#include <string>
#include <algorithm>
using namespace std;
#define MAXN 1000010
#define MIN(a,b) ((a)<(b)?(a):(b))
int map[MAXN*2],index[MAXN+1],k[MAXN+1],stack[MAXN],ptr[MAXN];
int a[MAXN],b[MAXN];
char flag[MAXN+1];
int n,p1,p2;

int dfs(int v){
    int i,j,p;
    memset(flag,0,sizeof(flag));
    stack[0]=v;
    ptr[0]=0;
    flag[v]=1;
    for(p=0; p>=0;){
        i=stack[p];
        if(ptr[p]<k[i]){
            j=map[index[i]+ptr[p]];
            ptr[p]++;
            if(p==0||j!=stack[p-1]){
                if(flag[j]){p1=i; p2=j; return 1;}
                else{
                    p++;
                    stack[p]=j;
                    ptr[p]=0;
                    flag[j]=1;
                }
            }
        }else p--;
    }
    return 0;
}

int work(int v){
    int i,j,p;
    stack[0]=v;
    ptr[0]=0;
    a[0]=1;
    b[0]=0;
    for(p=0; p>=0;){
        i=stack[p];
        if(ptr[p]<k[i]){
            j=map[index[i]+ptr[p]];
            ptr[p]++;
            if((i!=p1||j!=p2)&&(i!=p2||j!=p1)&&(p==0||j!=stack[p-1])){
                p++;
                stack[p]=j;
                ptr[p]=0;
                a[p]=1;
                b[p]=0;
            }
        }else{
            if(p>0){
                a[p-1]+=MIN(a[p],b[p]);
                b[p-1]+=a[p];
            }
            p--;
        }
    }
    return a[0];
}

int main()
{
    int i,j,t,c1,c2;
    freopen("traffic.in","r",stdin);
    freopen("traffic.out","w",stdout);
    scanf("%d",&n);
    for(i=1,t=0; i<=n; i++){
        index[i]=t;
        scanf("%d",&k[i]);
        for(j=0; j<k[i]; j++) scanf("%d",&map[t++]);
    }
//    printf("%d\n",t);
    dfs(1);
    c1=work(p1);
    c2=work(p2);
//    printf("%d %d %d %d\n",p1,p2,c1,c2);
    printf("%d\n",MIN(c1,c2));
    return 0;
}
