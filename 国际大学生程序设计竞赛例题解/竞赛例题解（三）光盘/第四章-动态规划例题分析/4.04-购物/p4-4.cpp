#include<fstream.h>
#include<string.h>
ifstream filein("shopping.in");
ofstream fileout("shopping.out");
int k,s[1001];
struct node{
  node(int vv=0,node* nn=0){v=vv;next=nn;}
  int v;
  node* next;
} * g[1001];
int tag[1001];
long funf[1001],fung[1001];
void search(int v){
  tag[v]=1;
  for(node* loop=g[v];loop;loop=loop->next)
    if(tag[loop->v]==0){
      node *pre=0,*temp;
      for(temp=g[loop->v];temp;temp=temp->next){
	if(temp->v==v)break;
	pre=temp;
      }
      if(temp){
	if(pre==0){
	  g[loop->v]=g[loop->v]->next;
	}else{
	  pre->next=temp->next;
	}
      }
      search(loop->v);
    }
}
long getg(int);
long getf(int v){
  if(funf[v]>=0)return funf[v];
  funf[v]=s[v];
  for(node* loop=g[v];loop;loop=loop->next)
    funf[v]+=getg(loop->v);
  return funf[v];
}
long getg(int v){
  if(fung[v]>=0)return fung[v];
  fung[v]=0;
  for(node* loop=g[v];loop;loop=loop->next)
    fung[v]+=(getf(loop->v)>getg(loop->v)?
      getf(loop->v):getg(loop->v));
  return fung[v];
}
void main(){
  int m,i,v1,v2;
  long ans=0;
  memset(g,0,sizeof(g));
  memset(tag,0,sizeof(tag));
  for(i=1;i<=1000;i++)funf[i]=fung[i]=-1;
  filein>>k>>m;
  for(i=1;i<=k;i++)filein>>s[i];
  for(i=1;i<=m;i++){
    filein>>v1>>v2;
    g[v1]=new node(v2,g[v1]);
    g[v2]=new node(v1,g[v2]);
  }
  for(i=1;i<=k;i++)if(tag[i]==0){
    search(i);
    ans+=(getf(i)>getg(i)?getf(i):getg(i));
  }
  fileout<<ans;
}
