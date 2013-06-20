program division;
const
  ifn='division.in';
  ofn='division.out';

const
  maxn=1000;

var
  know:array[1..maxn,1..3] of integer;
     {know用邻接表，记录每个人认识其他哪些人}
  du,now:array[1..maxn] of integer;
     {du记录每个人认识的人数（也就是图中顶点的度）}
     {now记录当前分队方案中每个人认识的同队人的数量}
  n,m:longint;
     {n是人数}
  ans:array[1..maxn] of boolean;
     {ans记录分队情况，ans[i]=true表示i在第一队}

procedure read_data;   {读入数据}
var
  i,x,y:longint;
begin
  assign(input,ifn);   {指定输入文件}
  reset(input);

    fillchar(know,sizeof(know),0);  {know和du数组置零，初始化邻接表}
    fillchar(du,sizeof(du),0);
    readln(n,m);
    for i:=1 to m do begin         {读入选手之间认识的情况，建立图的邻接表}
      readln(x,y);
      inc(du[x]);  know[x,du[x]]:=y;
      inc(du[y]);  know[y,du[y]]:=x;
    end;

  close(input);    {输入结束，关闭文件}
end;

procedure mend_du1(x:longint);
                {计算x换队之后，原来与x同队的人在图中顶点度的情况}
var
  i:longint;
begin
  {如果know[x,i]与x既认识又同队，那么换队后就不是同队认识了}
  for i:=1 to du[x] do 
    if ans[know[x,i]]=ans[x] then dec(now[know[x,i]]);
end;

function mend_du2(x:longint):longint; 
                {计算x换队之后，原来与x不同队的人在图中顶点度的情况}
var
  i,y:longint;
begin
  y:=0;
  for i:=1 to du[x] do          {寻找x换到另一队后与其认识的同队的人y}
    if ans[know[x,i]]=ans[x] then begin
      y:=know[x,i];
      break;
    end;
  if y>0 then inc(now[y]);   {修改这个人的认识人数（多认识了x）}
  now[x]:=du[x]-now[x];      {x因为换队，认识和不认识的人刚好相反}
  mend_du2:=y;     {返回这个认识的人，如果没有则返回0}
end;

procedure change(x:longint); {调整过程}
var
  y:longint;
begin
  repeat
    mend_du1(x);          {原来与x同队的人在图中顶点度的情况}
    ans[x]:=not ans[x];   {x换到另一队}
    y:=mend_du2(x);       {原来与x不同队的人在图中顶点度的情况}
    if y=0 then break;         {因为x换队可能导致y同队认识人数增加}
    if now[y]<=1 then break;   {如果y不需要调整怎退出}
    x:=y;                      {否则，继续循环，调整y}
  until false;
end;

procedure divi;  {分队}
var
  i:longint;
begin
  {一开始假设所有人都在第一队，第二队没有人}
  fillchar(ans,sizeof(ans),true); 
  now:=du;
  {调整分队情况}
  for i:=1 to n do  {发现有人在同队里认识多于一个人，就进行调整}
    if now[i]>1 then change(i);
end;

procedure write_data;    {输出结果}
var
  i,k:longint;
begin
  assign(output,ofn);    {指定输出文件}
  rewrite(output);

    k:=0;                {统计第一队人数k}
    for i:=1 to n do
      if ans[i] then inc(k);
    write(k);
    for i:=1 to n do      {打印第一队成员}
      if ans[i] then write(' ',i);
    writeln;
    write(n-k);          {其余的人都归第二队}
    for i:=1 to n do
      if not ans[i] then write(' ',i);

  close(output);        {输出完毕，关闭输出文件}
end;

begin     {主程序}
  read_data;    {读入数据}
  divi;       {分队}
  write_data;   {输出结果}
end.
