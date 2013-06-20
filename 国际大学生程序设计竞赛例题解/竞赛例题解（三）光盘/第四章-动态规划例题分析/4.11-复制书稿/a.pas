program copy_books;  const ifn='books.dat';	 {输入文件名}       ofn='books.out';	{输出文件名}       maxsize=500;		{书的最大本数}

type datatype=array[1..maxsize] of longint;

var dt:array[1..maxsize] of ^datatype;	
{记录j本书分配给i个人的个人最少页数}				 way:array[1..maxsize] of ^datatype;
{记录j本书分配给i个人，第i个人分配到的书的本数} sum:array[1..maxsize] of ^datatype;
{记录从第i本书至第j本书的页数总和} book:array[1..maxsize] of longint;
{记录第i本书的页数}     m,k:longint;  function max(x,y:longint):longint;	{求两个数中的大数} begin   if x>y then max:=x          else max:=y; end;  procedure read_data;		{读入数据} var f:text;     i,j:longint; begin   assign(f,ifn);     {指定输入文件}   reset(f);
    readln(f,m,k);
    for i:=1 to k do begin    {申请内存空间}
      new(dt[i]);
      new(way[i]);
    end;
    for i:=1 to m do read(f,book[i]);      {读入每本书的页数}
    for i:=1 to m do begin     {计算页数总和sum[i][j]}
      new(sum[i]);
      sum[i]^[i]:=book[i];
      for j:=i+1 to m do sum[i]^[j]:=sum[i]^[j-1]+book[j]; 
    end;
  close(f); {关闭文件}
end;

procedure solve;	{递推过程}
var i,j,l,x:longint;
begin
	{初始化数据}   dt[1]^[1]:=book[m];               {把书都分配给最后一个人}   for i:=2 to m do dt[1]^[i]:=dt[1]^[i-1]+book[m-i+1];
	{递推}		   for i:=2 to k do               {后i个人}    for j:=i to m do begin          {分配前j本书}      dt[i]^[j]:=maxlongint;      for l:=i-1 to j-1 do begin        {后i-1个人总共分配l本书}        x:=sum[m-j+1]^[m-l];                {计算倒数第i个人分配的页数}
       if max(x,dt[i-1]^[l])<=dt[i]^[j] then  {得到一个好的分配方案}
        begin
          dt[i]^[j]:=max(x,dt[i-1]^[l]);
          way[i]^[j]:=l;            {记录下此时开始一个人分配了多少本书}
        end;
     end;
   end;
end;

procedure write_data;	{输出结果}
var f:text;
    i,a,b:longint;
begin
  assign(f,ofn);  {指定输出文件}
  rewrite(f);
    a:=1;  b:=m;        {a是起始编号，b是剩下的书数量}
    for i:=k downto 2 do begin   {输出每一个人的分配方案}
      writeln(f,a,' ',m-way[i]^[b]);  {第i个人分配的起始书和终止书}
      a:=m-way[i]^[b]+1;
      b:=way[i]^[b];
    end;
    writeln(f,a,' ',m);    {最后一个人分配剩下的书}
  close(f); {关闭文件}
end;

begin		{主程序}
  read_data;    {读入数据}
  solve;        {动态规划求解}
  write_data;   {输出结果}
end.
