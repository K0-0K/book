type str=string;
const max=10000;          {表示无穷大}
var input,output:text;
n,fk,i,len:byte;      {n是问题的数目，fk是编码表的规模}
                      {len是文本长度}
    map:array[0..255] of integer;
    form:array[1..200] of record        {form保存编码表}
                             s:str;       {s是字符串}
                             l:byte;      {l是s的编码}
                          end;
    temp,doc:str;


  procedure init;	{程序初始化并读入数据}
  begin
    assign(input,'compress.dat');   {指定输入输出文件}
    assign(output,'compress.out');
    reset(input);
    rewrite(output);
  end;

  function min(a,b:integer):integer;	{求两个数中的小数}
  begin
    if a<b then min:=a else min:=b;
  end;

  procedure workout;   {对文本进行压缩}
  var i,j:byte;
  begin
    for i:=1 to len do map[i]:=max; {开始假设不能压缩，设为无穷大}
    map[0]:=0;
    j:=0;                 {j指示当前压缩进行的位置}
    while length(doc)>0 do
    begin
      for i:=1 to fk do   {在当前位置尝试使用每一种编码}
      if pos(form[i].s,doc)=1 then
         map[j+length(form[i].s)]:=
               min(map[j+length(form[i].s)],map[j]+form[i].l);

      delete(doc,1,1);    {位置前移一位}
      inc(j);        
    end;
  end;

begin		{主程序}
  init;    {输入输出初始化}
  readln(input,n);
  readln(input,temp);     {读入文本}
  for i:=1 to n do        
  begin
    fk:=0;           {fk是编码表的规模}
    doc:=temp;
      readln(input,temp);
      while (temp[1]='(') and (temp<>'')   {读入编码表}
      do
        begin
          inc(fk);
          form[fk].s:=copy(temp,2,pos(',',temp)-2);    {保存字符串}
          form[fk].l:=length(temp)-pos(',',temp)-1;    {计算编码的长度}
          readln(input,temp);   {读入编码表的下一项}
        end;
    len:=length(doc);    {求文本长度}
    workout;             {压缩文本}
    if map[len]=max then writeln(output,0)   {输出0表示不能压缩文本}
    else writeln(output,map[len]);   {输出结果}
  end;
  close(output); {关闭文件}
  close(input); {关闭文件}
end.
