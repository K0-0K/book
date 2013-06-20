{$N+}
program Sum;

type P=record	{多项式类型}
         l:integer;
         x:extended;
         p:array [0..30] of extended;
       end;

var fin,fout:text;
    n:integer;
    c:array [1..30,0..30] of extended; {记录各组合数的值}
    e:array [0..30] of p;	{Sk(n)的值}


procedure makeC;		{生成组合数C(n,m)}
  var i,j:integer;
  begin
    fillchar(c,sizeof(c),0);
    c[1,0]:=1;c[1,1]:=1;
    for i:=2 to 22 do
      begin
        c[i,0]:=1;
        c[i,i]:=1;
        for j:=1 to i-1 do c[i,j]:=c[i-1,j-1]+c[i-1,j];
      end;
  end;


function nzc(a,b:extended):boolean;{判断b是否不能整除a}
  var temp:extended;
  begin
    nzc:=false;
    temp:=int(a/b);  {a除以b的商取整}
    if abs(a-temp*b)<1e-5 then exit;
    if abs(a-(temp+1)*b)<1e-5 then exit;
    if abs(a-(temp-1)*b)<1e-5 then exit;
    nzc:=true;       {b不能整除a}
  end;


procedure reduce(var a:P);	{约分}
  var i,j:integer;
      flag:boolean;
  begin
    for i:=21 downto 2 do
      begin
        if nzc(a.x,i) then     {判断i能否整除分母}
continue;
        flag:=false;
        for j:=0 to a.l do
        if nzc(a.p[j],i) then  {判断i能否整除所有分子}
          begin
            flag:=true;
            break;
          end;

        if flag then       {flag=true表示i不是分子和分母的公约数}
continue;
        for j:=0 to a.l do   {否则，消除公约数i}
        a.p[j]:=a.p[j]/i;
        a.x:=a.x/i;
      end;
  end;


procedure add(a,b:P;var c:p);	{多项式相加}
  var i,j,l:integer;
  begin
    fillchar(c,sizeof(c),0);
    if a.l>b.l then c.l:=a.l else c.l:=b.l;   {取最高系数}
    c.x:=a.x*b.x;                             {通分}
    for i:=0 to a.l do a.p[i]:=a.p[i]*b.x;    
    for i:=0 to b.l do b.p[i]:=b.p[i]*a.x;
    for i:=0 to c.l do c.p[i]:=a.p[i]+b.p[i];  {分子相加}
    reduce(c);                                {约分}
  end;


procedure main; {主程序}
  var i,j,k:integer;
      temp:p;
      sn:string;

  begin
    assign(fin,'sum.dat');   {指定输入文件，读入数据}
    reset(fin);
    readln(fin,n);
    close(fin); {关闭文件}

fillchar(e,sizeof(e),0);
{边界条件：k=1}
    with e[0] do
      begin
        x:=1;
        l:=1;
        p[1]:=1;p[0]:=0;
      end;

{递推求Sk(n)的值}
    for k:=2 to n+1 do
      begin
        with e[k-1] do
          begin
            l:=k;
            x:=1;
            p[l]:=1;
          end;
       
 for i:=2 to k do  {利用递推公式求解}
          begin
            temp:=e[k-i];
            with temp do
              begin
                if i mod 2=0 then
                  for j:=0 to l do p[j]:=p[j]*c[k,i]
                else
                 for j:=0 to l do p[j]:=-p[j]*c[k,i];
              end;
            add(e[k-1],temp,e[k-1]);
          end;
        e[k-1].x:=e[k-1].x*k;
        reduce(e[k-1]);  {最后约分，求得e[k-1]的最简式}
      end;
	
{输出结果}
    assign(fout,'sum.out'); {指定输出文件}
    rewrite(fout);
    write(fout,e[n].x:0:0);   {输出分母}
    with e[n] do
    for i:=l downto 0 do write(fout,' ',p[i]:0:0); {输出各系数分子}
    close(fout); {关闭文件}
  end;

begin  {主过程}
  makeC;   {计算组合数}
  main;    {计算Sk(n)}
end.
