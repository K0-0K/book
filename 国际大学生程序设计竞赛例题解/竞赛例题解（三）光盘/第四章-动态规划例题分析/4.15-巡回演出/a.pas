{$i-,r-,x-,v-,s-,f-,b-,n+}
var
  day,i,i1,j,k,n:integer;
  f1,f2:text;
  cost:array[1..1000,1..10] of longint;
     {cost[n][i]表示第n天到达城市i的最少费用}
  d,cnt:array[1..10,1..10] of byte;
     {d[i][j]表示从i到j有多少航班}
  price:array[1..10,1..10,1..30] of longint;
     {price[i][j]记录从i到j的每一个航班的费用}

procedure init;    {指定输入输出}
begin
  assign(f1,'perform.dat');   {指定输入输出文件}
  reset(f1);
  assign(f2,'perform.out');
  rewrite(f2);
end;

procedure read_file;         {读数据文件}
begin
  readln(f1,n,k);
  if n>=2 then
    for i:=1 to n do    {读入航班表}
      for j:=1 to n do
        if i<>j then
          begin
            read(f1,d[i,j]);   {读入航班循环周期}
            for i1:=1 to d[i,j] do read(f1,price[i,j,i1]); {读入航班具体变化}
          end;
end;

procedure run; {主过程}
begin
  repeat
    read_file;     {读入数据}
    if n>=2 then
      begin
        fillchar(cost,sizeof(cost),0);  {cost[i][j]=0表示暂时没有到达j的路径}
        for i:=2 to n do cost[1,i]:=price[1,i,1];  {初始条件}
        day:=1;
        fillchar(cnt,sizeof(cnt),1);
        while day<k do  {按天数划分阶段，day不断增加直到k}
          begin
  {CNT数组记录了第DAY天时各航班表循环到哪一天，以下是数组CNT所有元素循环增1}
            for i:=1 to n do       
              for j:=1 to n do
                if i<>j then
                  begin
                    inc(cnt[i,j]);
                    if cnt[i,j]>d[i,j] then cnt[i,j]:=1;
                  end;
           {由COST[DAY,I]推出COST[DAY+1,J] (I,J=1,2,…,N)}
            for i:=1 to n do
              if cost[day,i]>0 then   {第day天能够到达i}
                for j:=1 to n do
                  if (i<>j) and (price[i,j,cnt[i,j]]>0) then
                                        {第day+1天有航班从i到j}
                    if (cost[day+1,j]=0)
                      or (cost[day,i]+price[i,j,cnt[i,j]]<cost[day+1,j]) then
               {如果是新的路径或者费用更少，那么就找到一个新的旅行计划}
                      cost[day+1,j]:=cost[day,i]+price[i,j,cnt[i,j]];
            inc(day);     {继续计算下一天的计划}
          end;
        writeln(f2,cost[k,n]);  {输出结果}
      end;
  until n=0;   {知道输入结束}
end;

procedure done; {关闭输入输出}
begin
  close(f1); {关闭文件}
  close(f2); {关闭文件}
end;

begin  {主程序}
  init;  {指定输入输出}
  run;   {主过程}
  done;  {关闭输入输出}
end.
