{$M 65520,0,655360}
{$r-,i-,n+}
program package;
type
  lightnode=array[1..4] of integer;  {表示状态，需要四种灯泡的个数}

  packagetype=record       {表示可选购套装的属性}
    la:integer;          {标号}
    va:real;             {价格}
    nu:lightnode;        {购买方式}
  end;

  node=array[0..999] of real;    {保存状态的空间}

const   {输入输出文件}
  inputname='package.in';
  outputname='package.out';

var
  pa:array[1..50] of packagetype;{保存所有可选购套装的属性}
  ne:array[1..50] of integer;    {在输出结果时保存购买方案} 
  r:lightnode;                   {顾客的购买需求}
  w:array[1..4] of longint;      {计算某一状态在数组中的位置时用的参数}
  min:array[0..999] of ^node;    {保存所有状态的最小费用}
  n:byte;                        {可选购套装总数}
  temp:lightnode;
  i,j:integer;

  procedure readdata;   {读入数据}
  var
    f:text;
    s,s1:string;
    i,j,k:integer;
  begin
    assign(f,inputname);  {指定输入文件}
    reset(f);

    readln(f,n);
    for i:=1 to n do              {读入每个优惠套装}
    begin
      read(f,pa[i].la,pa[i].va);   {读入标号和价格}
      readln(f,s);                 {读入购买方式}
      s:=s+' ';
      for j:=1 to 4 do
        if pos(chr(96+j),s)>0 then   {寻找灯泡类型：abcd}
        begin
          k:=pos(chr(96+j),s)+2;      {寻找对应数量}
          s1:='';                        {把表示数量的字符串转化为数字}
          while s[k]<>' ' do begin s1:=s1+s[k]; inc(k); end;
          val(s1,pa[i].nu[j],k);
        end
        else pa[i].nu[j]:=0;          {如果没有改类型的灯泡则表示不需要购买}
    end;

    readln(f,s);                   {读入顾客要买的灯泡型号和数量}
    s:=s+' ';
    for j:=1 to 4 do      {寻找灯泡类型：abcd}
      if pos(chr(96+j),s)>0 then
      begin
        k:=pos(chr(96+j),s)+2;
        s1:='';                {把表示数量的字符串转化为数字}
        while s[k]<>' ' do begin s1:=s1+s[k]; inc(k); end;
        val(s1,r[j],k);
      end
      else r[j]:=0;

    close(f); {关闭文件}
  end;

  procedure getready; {初始化}
  var
    i,j:integer;
    tot:longint;
    temp:packagetype;
  begin
for i:=1 to n-1 do   {把套装按标号从小到大排序}
      for j:=i+1 to n do if pa[i].la>pa[j].la then
      begin
        temp:=pa[i];
        pa[i]:=pa[j];
        pa[j]:=temp;
      end;

    tot:=1;         {统计总数tot以及计算存储的位置数组w}
    for i:=1 to 4 do
    begin
      w[i]:=tot;
      tot:=tot*(1+r[i]);
    end;

    {给存储任一灯泡状态最优值的数组分配空间，并赋初值}
    tot:=tot div 1000;
    for i:=0 to tot do 
    begin
      new(min[i]);
      fillchar(min[i]^,sizeof(min[i]^),0);
    end;
    min[0]^[0]:=1;
  end;

  procedure change(s:lightnode; var la1,la2:integer);
  {计算某一状态在数组中的位置}
  var
    tot:longint;
    i:byte;
  begin
    tot:=0;
    for i:=1 to 4 do tot:=tot+w[i]*s[i];
    la1:=tot div 1000;
    la2:=tot-la1*1000;
  end;

  procedure getmin(s:lightnode; la1,la2:integer);
  {计算灯泡状态为s时的最优值}
  var
    i,j:byte;
    nla1,nla2:integer;
  begin
    min[la1]^[la2]:=1e30;

    for i:=1 to n do  {尝试使用每一种优惠套装}
      if ((s[1]>0) and (pa[i].nu[1]>0)) or ((s[2]>0) and (pa[i].nu[2]>0)) or
         ((s[3]>0) and (pa[i].nu[3]>0)) or ((s[4]>0) and (pa[i].nu[4]>0)) then
      {要状态有变化才操作}
      begin
        for j:=1 to 4 do
        begin
          temp[j]:=s[j]-pa[i].nu[j];
          if temp[j]<0 then temp[j]:=0; 
          {因为只要满足顾客要求即可，所以购买数可多于需求数}
        end;
        change(temp,nla1,nla2); {计算存储位置n1a1,n1a2}

        if min[nla1]^[nla2]=0 then getmin(temp,nla1,nla2);
                  {计算使用了第i种优惠套装之后剩下的还需要多少花费}
                  {如果以前曾经计算过，则不需要再次重复}

        if min[la1]^[la2]>min[nla1]^[nla2]+pa[i].va then
          min[la1]^[la2]:=min[nla1]^[nla2]+pa[i].va;
      end;
  end;

  procedure getsolve(s:lightnode; la1,la2:integer);
  {求最小费用的购买方案}
  var
    i,j:byte;
    nla1,nla2:integer;
  begin
    for i:=1 to n do   {尝试使用每一种优惠套装}
    begin
      for j:=1 to 4 do
      begin
        temp[j]:=s[j]-pa[i].nu[j];
        if temp[j]<0 then temp[j]:=0;
      end;
      change(temp,nla1,nla2); {计算存储位置n1a1,n1a2}

      if abs(min[la1]^[la2]-min[nla1]^[nla2]-pa[i].va)<0.001 then
                       {判断使用该优惠能否达到整体的最优值}
      begin
        inc(ne[i]);
        getsolve(temp,nla1,nla2);
        break;
      end;
    end;
  end;

  procedure print(re:lightnode;la1,la2:integer);  {输出结果}
  var
    f:text;
    i:byte;
  begin
    fillchar(ne,sizeof(ne),0);
    getsolve(re,la1,la2);   {求最小费用的购买方案}

    assign(f,outputname);  {指定输出文件}
    rewrite(f);
    writeln(f,min[la1]^[la2]-1:0:2);    {输出花费}
    for i:=1 to n do if ne[i]>0 then     {输出方案}
    begin
      write(f,pa[i].la);
      if ne[i]>1 then write(f,'(',ne[i],') ')
      else write(f,' ');
    end;
    close(f); {关闭文件}
  end;

begin   {主程序}
  readdata;    {读入数据}
  getready;    {初始化}
  change(r,i,j);   {计算状态r的位置}
  getmin(r,i,j);   {计算状态r的最优值}
  print(r,i,j);   {输出结果}
end.
