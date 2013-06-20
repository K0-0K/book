program bridge;
var
  fin,fout:text;   {文件变量}
  n,load,temp,len:integer; 
     {load是桥的载重，len是桥的长度}
  spd,wei:array[1..1001] of integer;
{spd存放车辆的最大速度，wei存放车辆的重量}
  da:array[1..1001] of double;
    {计算结果da[i]表示前i辆车全通过小桥的最短时间}
  i,j,k,tot,least:integer;
begin
  assign(fin,'bridge.in'); reset(fin);      {指定输入输出文件}
  assign(fout,'bridge.out'); rewrite(fout);
  readln(fin,load,len,n);
  fillchar(da,sizeof(da),0);
  for i:=1 to n do begin
    readln(fin,wei[i],spd[i]);        {读入第i部车的数据}      
    if i=1 then da[1]:=len/spd[1]    {边界条件：只有一辆车}     
    else begin
      {计算前i部车过桥的最少用时}
      j:=i;
      tot:=0;
      least:=30000;
      while j>=1 do begin           {j是分组的位置}
        inc(tot,wei[j]);             {tot是当前组的重量}
        if tot>load then break;
        if least>spd[j] then least:=spd[j];       {least是最慢的车速}
        if tot<=load then
          {计算da[j]+T(T的意义见思路描述)并与当前da[i]比较，得出最小值}
          if (da[i]=0) or (da[j-1]+len/least<da[i]) then
            da[i]:=da[j-1]+len/least         
          else
        else break;
        dec(j)                {尝试另一个分组位置}
      end;
    end;
  end;
  writeln(fout,da[n]*60:0:1);   {输出结果}
  close(fin); {关闭文件}
  close(fout); {关闭文件}
end.
