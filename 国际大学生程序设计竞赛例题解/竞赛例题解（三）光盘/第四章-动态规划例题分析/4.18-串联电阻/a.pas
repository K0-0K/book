{$A+,B-,D+,E+,F-,G-,I+,L+,N-,O-,P-,Q-,R+,S+,T-,V+,X+,Y+}
{$M 65520,0,655360}
type waynum=string[29];     {用字符串形式存放大整数}
var 
f1,f2:text;   {文件变量}
    ways:array[0..1999] of waynum;   {ways[i]表示得到阻值为i的电阻的方案数}
    v:array[1..20] of integer;         {v存放每盒电阻的阻值}
    rmake:array[1..1999] of integer;   {rmake存放需要计算的阻值}
    b,n,t,nmax,temp,i,j:integer;
    h,m,s,ms:word;
test1,test2:waynum;

procedure add(var a:waynum;b:waynum); {高精度加法}
var 
l,l1,l2,code,temp,i,j:integer;
    ia,ib:longint;
begin
l1:=length(a);    {求a和b的位数}
l2:=length(b);
if (l1<=9) and (l2<=9) then {如果a，b皆在longint的表示范围则直接计算}
begin
val(a,ia,code);   {把a转化为longint并存放到ia}
val(b,ib,code);   {把b转化为longint并存放到ib}
ia:=ia+ib;
str(ia,a);        {把ia转化为字符串a}
end
else begin
if l1>=l2 then {a的位数不小于b的位数，则把b加在a上}
begin
a:='0'+a;  {a加上前导0，方便进位}
inc(l1);
j:=l1;
for i:=l2 downto 1 do {从尾数开始逐位相加}
begin
temp:=(ord(a[j])+ord(b[i])-96);
a[j-1]:=chr(ord(a[j-1])+temp div 10);   {进位}
a[j]:=chr(temp mod 10+48);          {计算第j位的数字}
j:=j-1;
end;
for i:=l1-l2 downto 2 do {处理最高位进位的问题}
begin
if a[i]>'9' then   {大于9则需要进位}
begin
a[i-1]:=chr(ord(a[i-1])+1);   {进位}
a[i]:=chr(ord(a[i])-10);      {低位保留一个数字}
end 
else break;
end;
if a[1]='0' then delete(a,1,1);    {把前导0删除}
end
else begin {a的位数小于b的位数，则把a加在b上，过程与上面基本类型}
b:='0'+b;
inc(l2);
j:=l2;
for i:=l1 downto 1 do{从尾数开始逐位相加}
begin
temp:=(ord(a[i])+ord(b[j])-96);
b[j-1]:=chr(ord(b[j-1])+temp div 10);  {进位}
b[j]:=chr(temp mod 10+48);
j:=j-1;
end;
for i:=l2-l1 downto 2 do{处理最高位进位的问题}
begin
if b[i]>'9' then 
begin 
b[i-1]:=chr(ord(b[i-1])+1);  {进位}
b[i]:=chr(ord(b[i])-10);   {低位保留一个数字}
end else break;
end;
if b[1]='0' then delete(b,1,1);
a:=b;   {最后把结果保存到a}
end;
end;
end;

begin
assign(f1,'Resist.in');  reset(f1);    {指定输入输出文件}
assign(f2,'Resist.out');  rewrite(f2);
readln(f1,b); {读入盒子数b}
while (b<>0) do
begin
for i:=1 to b do read(f1,v[i]); {读入每盒电阻的阻值}
readln(f1);
readln(f1,n); 
nmax:=n;      {nmax是最大需要计算的阻值}
t:=0;           {t是需要计算的阻值数量}
while (n<>0) do  {读入所有要计算的个阻值n}
begin
inc(t);
rmake[t]:=n;   {把需要计算的阻值保存到rmake中}
if n>nmax then nmax:=n;
readln(f1,n);
end;
for i:=0 to nmax do ways[i]:='1'; {初始化每种阻值的获得途径数为1}
for i:=2 to b do {按上面题解的状态转移方程求解}
for j:=v[i] to nmax do add(ways[j],ways[j-v[i]]); 
for i:=1 to t do writeln(f2,ways[rmake[i]]); {输出结果}
readln(f1,b); {读入盒子数b}
  	end;
close(f1); {关闭文件}
close(f2); {关闭文件}
end.
