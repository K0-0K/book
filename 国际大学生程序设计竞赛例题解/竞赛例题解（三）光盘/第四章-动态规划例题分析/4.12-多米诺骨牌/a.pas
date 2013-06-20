program Dominoes;

const ifn='domino.dat';		{输入数据文件名}
      ofn='domino.out';		{输出答案文件名}
      maxn=1000;            {骨牌的最大数量}
      maxdot=6;             {骨牌最大点数}

type gaptype=array[-maxdot*maxn..maxdot*maxn] of integer;

var domino:array[1..maxn] of integer;
        {domino记录每块骨牌的两个数的差值}
    gap,addgap,g:^gaptype;
    n:integer;

procedure read_data;	{读入数据}
var i,a,b:integer;
begin
  assign(input,ifn);  {指定输入文件}
  reset(input);
    readln(n);        {读入骨牌数量}
    for i:=1 to n do begin
      readln(a,b);
      domino[i]:=a-b;  {只保留两个数的差就足够了，具体什么数字没有关系}
    end;
  close(input); {关闭文件}
end;

procedure solve;	{利用动态规划思想寻找最优解}
var min,max:integer;
    i,j,k:integer;
begin
  new(gap);  new(addgap);   {申请内存空间}
  for i:=-maxdot*maxn to maxdot*maxn do begin   {初始化}
    gap^[i]:=maxn;
    addgap^[i]:=maxn;
  end;
  gap^[0]:=0;    {开始假设没有骨牌，不需要翻转就可以做到上下层差值为0}
  min:=0;  max:=0;
  for i:=1 to n do begin   {从左到右逐一加入骨牌}
    for j:=min to max do
      if gap^[j]<maxn then begin
        k:=j+domino[i];     {第i块骨牌不动}
        if gap^[j]<addgap^[k] then addgap^[k]:=gap^[j];
        k:=j-domino[i];     {第i块骨牌翻转一次}
        if gap^[j]+1<addgap^[k] then addgap^[k]:=gap^[j]+1;
        gap^[j]:=maxn;
      end;
    min:=min-abs(domino[i]);   {计算可能达到的最小和最大的上下层差值}
    max:=max+abs(domino[i]);
    g:=gap;                    {交换运算数组，准备加入下一块骨牌}
    gap:=addgap;
    addgap:=g;
  end;
end;

procedure write_data;	{输出答案}
var min,k:integer;
begin
  assign(output,ofn);   {指定输出文件}
  rewrite(output);
    min:=maxn;  k:=-1;
    repeat                  {寻找最小的差值}
      inc(k);
      if gap^[k]<min then min:=gap^[k];
      if gap^[-k]<min then min:=gap^[-k];
    until min<maxn;
    writeln(min);           {输出差值}
  close(output); {关闭文件}
end;

begin		{主程序}
  read_data;    {输入数据}
  solve;        {动态规划过程}
  write_data;   {输出结果}
end.
