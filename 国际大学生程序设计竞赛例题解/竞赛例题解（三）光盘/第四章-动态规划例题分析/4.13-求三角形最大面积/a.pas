var
  f1,f2:text;
  n,max:integer;
  a:array[1..2,1..100,1..200]of boolean;  {a存放每一块子三角的好坏情况}

procedure main;		{主过程}
var
  i,j,h,l,k:integer;   {i,k是用于枚举三角形的坐标}
begin
  h:=0;   {h是标记，当找到全白三角形就令h=1}
  for i:=1 to n-1  do           {计算n=2时顶角朝上的三角形}
    for j:=1 to (n+1-i-1) do
      begin
        k:=2*j-1;
        a[2,i,k]:=(a[1,i,k])and(a[1,i,k+2])and(a[1,i,k+1])and(a[1,i+1,k]);
                  {判断包含的四个子三角形是否都是全白的}
        if a[2,i,k] then h:=1;
      end;
  for i:=1 to n-1 do            {计算n=2时顶角朝下的三角形}
    for j:=2 to (n-i-1) do
      begin
        k:=2*j;
        a[2,i,k]:=(a[1,i+1,k])and(a[1,i+1,k-1])and(a[1,i,k-2])and(a[1,i,k]);
                  {判断包含的四个子三角形是否都是全白的}
        if a[2,i,k] then h:=1;
      end;
  l:=2;
  while h<>0 do   {循环寻找更大的三角形}
    begin
      inc(max); inc(l);       {max是当前阶段三角形的大小}
      h:=0;                   {h标记是否有边长为max的好三角形}
      a[1]:=a[2];             {把已有结果存放到a[1]，准备下一阶段递推}
      for i:=1 to n-l+1 do
        begin
          for j:=1 to (n+1-i-l+1) do    {计算顶角朝上的三角形}
            begin
              k:=2*j-1;
              a[2,i,k]:=(a[1,i,k])and(a[1,i,k+2])and(a[1,i+1,k]);
                  {判断B,C,D三部分是否都是全白的}
              if a[2,i,k] then h:=1;
            end;
          for j:=l to (n+1-i-l)do       {计算顶角朝下的三角形}
            begin
              k:=2*j;
              a[2,i,k]:=(a[1,i,k])and(a[1,i+1,k-2])and(a[1,i+1,k]);
                  {判断B,C,D三部分是否都是全白的}
              if a[2,i,k] then h:=1;
            end;
        end;
    end;
  writeln(f2,max*max);   {输出最大三角形的面积}
end;

procedure init;	{程序初始化}
var
  ch:char;
  i,j,k:integer;
  s:string;
begin
  assign(f1,'triangle.dat'); reset(f1);   {指定输入输出文件}
  assign(f2,'triangle.out'); rewrite(f2);
  readln(f1,n);    {读入三角形大小n}
  while n<>0 do
    begin
      max:=0; j:=n;
      for i:=1 to n do        {读入三角形的每一行}
        begin
          readln(f1,s); k:=0;
          while s[1]=' ' do delete(s,1,1);  {删除每一行前面的空格}
          while k<2*(n+1-i)-1 do
            begin
              inc(k);
              if s[1]='#' then a[1,i,k]:=false else
                if s[1]='-' then begin a[1,i,k]:=true; max:=1; end;
                               {记下每个小三角形的好坏情况，作为初始条件}
              delete(s,1,1);
            end;
        end;
       main;             {求解}
       readln(f1,n);     {准备读入下一个三角形}
     end;
  close(f1); close(f2); {关闭文件}
end;

begin   {主程序}
  init;   {程序初始化}
end.
