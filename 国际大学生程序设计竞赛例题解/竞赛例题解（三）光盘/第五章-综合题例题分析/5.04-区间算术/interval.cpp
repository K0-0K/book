//Interval arithmetic
#include<stdio.h>
#include<math.h>
const double eps=1e-7;
const long num=100;       
typedef struct node{           
	double min;       //记录区间下界
	double max;       //记录区间上界
} interval;
interval data[num];  //数据栈
char stack[num],     //运算符栈
     deg[128];       //以ASCII码为下标,记录各运算符优先级
long record[num],    //运算符栈中相应元素的优先级,
     top,            //指明运算符栈的栈顶位置
	 n,              //数据栈中区间个数
	 good;           //good=0表明出现了除数为0的情况
void add()                        //对加号的处理
{
   interval a,b;
   a.min=data[n-2].min;
   a.max=data[n-2].max;
   b.min=data[n-1].min;
	 b.max=data[n-1].max;
   n-=2;
   data[n].min=a.min+b.min;
   data[n].max=a.max+b.max;
   n++;
}
void minus()
{
	interval a,b;
	if(record[top]==1){              //如果为减号，则做减法运算
	   a.min=data[n-2].min;
       a.max=data[n-2].max;
       b.min=data[n-1].min;
			 b.max=data[n-1].max;
       n-=2;
	   data[n].min=a.min-b.max;
	   data[n].max=a.max-b.min;
	   n++;
	}
	else{                           //如果为负号，则做取负运算
		n--;
		a.min=data[n].min;
		a.max=data[n].max;
		data[n].max=-a.min;
		data[n].min=-a.max;
		n++;
	}
}
void multi()                        //对乘号的处理
{
	interval a,b;
	double v[4];
	long i;
	a.min=data[n-2].min;
    a.max=data[n-2].max;
    b.min=data[n-1].min;
    b.max=data[n-1].max;
    n-=2;
	v[0]=a.min*b.min;
	v[1]=a.min*b.max;
	v[2]=a.max*b.min;
	v[3]=a.max*b.max;
	data[n].min=data[n].max=v[0];
	for(i=1;i<4;i++){
		if(v[i]<data[n].min)
			data[n].min=v[i];
		if(v[i]>data[n].max)
			data[n].max=v[i];
	}
	n++;
}
void divide()                       //对除号的处理
{
	interval a,b;
	double v[4];long i;
	a.min=data[n-2].min;
    a.max=data[n-2].max;
    b.min=data[n-1].min;
    b.max=data[n-1].max;
		n-=2;
	if(b.min<=0&&b.max>=0){         //处理除数区间包含0的情况
		good=0;
		printf("Division by zero\n");
		return;
	}
	v[0]=a.min/b.min;
	v[1]=a.min/b.max;
	v[2]=a.max/b.min;
	v[3]=a.max/b.max;
	data[n].min=data[n].max=v[0];
	for(i=1;i<4;i++){
		if(v[i]<data[n].min)
			data[n].min=v[i];
		if(v[i]>data[n].max)
			data[n].max=v[i];
	}
	n++;
}
int main()
{
	freopen("interval.in","r",stdin);
	freopen("interval.out","w",stdout);
	deg['+']=deg['-']=1;      //设置各运算符的优先级,数值越大,优先级越高
	deg['*']=deg['/']=2;      
	deg['(']=4;               //3缺省为负号的优先级
	long sum,prev;           //用于区分读入的'-'是负号和减号,
                //如果读入'-'时prev=0,表明'-'跟在一个运算符后面,是负号
                //如果读入'-'时prev=1,表明'-'跟在一个运算数后面,是减号
	char ch;
	do{
		sum=0;n=0;good=1;
		top=-1;prev=0;
		do{
			if(scanf("%c",&ch)!=1||ch=='\n'){  //判断是否读完一行数据
				if(good){
				   while(good&&top>=0){
					     if(stack[top]=='+')
						    add();
							 if(stack[top]=='-')
								minus();
							 if(stack[top]=='*')
								multi();
							 if(stack[top]=='/')
								divide();
							 top--;
					 }
				}
				break;
			}
			sum++;
			if(ch=='['){
				scanf("%lf%*c%lf%*c",&data[n].min,&data[n].max);
				n++;
				prev=1;
			}
			else if(good){
				if(ch=='+'||ch=='*'||ch=='/'){
					while(good&&top>=0&&record[top]>=deg[ch]){
							if(stack[top]=='(')break;
							if(stack[top]=='+')
								 add();
								if(stack[top]=='-')
								 minus();
								if(stack[top]=='*')
								 multi();
								if(stack[top]=='/')
								 divide();
								top--;
					}
					stack[++top]=ch;
					record[top]=deg[ch];
					prev=0;
				}
				if(ch=='-'){
					if(prev==0){                  //负号的入栈处理
						while(good&&top>=0&&record[top]>=3){
							if(stack[top]=='(')break;
							if(stack[top]=='+')
									 add();
									if(stack[top]=='-')
									 minus();
									if(stack[top]=='*')
									 multi();
									if(stack[top]=='/')
									 divide();
									top--;
						}
						stack[++top]=ch;
						record[top]=3;
						prev=1;
					}
					else{                         //减号的入栈处理
						while(good&&top>=0&&record[top]>=1){
							if(stack[top]=='(')break;
							if(stack[top]=='+')
									 add();
									if(stack[top]=='-')
									 minus();
									if(stack[top]=='*')
									 multi();
									if(stack[top]=='/')
									 divide();
									top--;
						}
						stack[++top]=ch;
						record[top]=1;
						prev=0;
					}
				}
				if(ch=='('){
					stack[++top]=ch;
					record[top]=4;
					prev=0;
				}
				if(ch==')'){
					while(good&&top>=0){
						if(stack[top]=='('){
							 top--;
							 break;
						}
						if(stack[top]=='+')
							 add();
							if(stack[top]=='-')
							 minus();
							if(stack[top]=='*')
							 multi();
							if(stack[top]=='/')
							 divide();
							top--;
					}
					prev=1;
				}
			}
		}while(1);
		if(!sum)return 0;
		if(!good)continue;
		if(fabs(data[0].min)<eps)data[0].min=0.0;
		if(fabs(data[0].max)<eps)data[0].max=0.0;
		printf("[%.3lf,%.3lf]\n",data[0].min,data[0].max);
	}while(1);
}
