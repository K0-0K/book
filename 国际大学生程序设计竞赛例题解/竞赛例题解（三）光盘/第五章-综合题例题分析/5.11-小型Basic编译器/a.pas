{$A+,B-,D+,E+,F-,G-,I+,L+,N-,O-,P-,Q+,R+,S+,T-,V-,X+,Y+}
{$M 65520,0,655360}
program TinyBasicInterpreter;

const
     ipf='tbasic.dat';	{输入文件名}
     opf='tbasic.out';	{输出文件名}
     max=100;

type
    TExpression=record  {表达式类型}
                      etype:integer; {表达式类型}(* 1 ... 3 *)
                      constant:integer; {用到的常量}
                      var1,var2:char;   {用到的变量}
    end;

    TStatement=record   {语句类型}
                     stype:integer;  {语句类型}(* 1 ... 5 *)
                     var1:char;        {用到的变量}
                     expr:TExpression; {用到的表达式}
    end;

var f:text;
    prog:array[1..max] of TStatement;  {prog存储TBASIC程序}
    n:integer;                           {n记录程序有多少条语句}
    variable:array['A'..'Z'] of integer;  {variable存储变量的值}

function s2i(s:string):integer;	{ 字符串转换成数字 }
var i,code:integer;
begin
     val(s,i,code);  {val函数把字符串s转换成数字i，错误代码保存在code}
     s2i:=i;
end;

procedure convert2(s:string; var expr:TExpression);
                    {把字符串形式的表达式s存储到记录expr中}
begin
     if s[1]>'9' then begin
        if s[2]='+' then expr.etype:=2     {类型2：<变量>+<变量>}
                    else expr.etype:=3;    {类型3：<变量>><变量>}
        expr.var1:=s[1];
        expr.var2:=s[3];
     end else begin
         expr.etype:=1;                    {类型1：<常量>}
         expr.constant:=s2i(s);
     end;
end;

procedure convert(s:string; var sm:TStatement); 
                    {把字符串形式的语句s存储到记录sm中}
var s1:string;
begin
     while not (s[1] in ['A'..'Z']) do delete(s,1,1);  {删除行号}
     s1:=copy(s,1,pos(' ',s)-1);
     delete(s,1,pos(' ',s));
     while s[1]=' ' do delete(s,1,1);                  {删除多余空格}
     if s1='LET' then begin                   {类型1：LET语句}
        sm.stype:=1;
        sm.var1:=s[1];       {保存等号左边的赋值变量}
        delete(s,1,2);
        convert2(s,sm.expr);  {保存等号右边的表达式}
     end;
     if s1='PRINT' then begin                 {类型2：PRINT语句}
        sm.stype:=2;
        sm.var1:=s[1];        {保存需要打印的表达式}
     end;
     if s1='GOTO' then begin                  {类型3：GOOT语句}
        sm.stype:=3;
        convert2(s,sm.expr);   {保存跳转的表达式}
     end;
     if s1='IF' then begin                    {类型4：IF语句}
        sm.stype:=4;
        convert2(s,sm.expr);     {保存条件表达式}
     end;
     if s='STOP' then begin                   {类型5：STOP语句}
        sm.stype:=5;
     end;
end;

procedure initialize;	{程序初始化}
var s:string;
begin
     assign(f,ipf); reset(f);   {指定输入文件}
     n:=0;
     while not eof(f) do begin    {逐行读入和存储BASIC程序}
           readln(f,s);        {读入}
           inc(n);
           convert(s,prog[n]); {存储}
     end;
     close(f); {关闭文件}
     fillchar(variable,sizeof(variable),0);   {变量空间初始化}
end;

function result(expr:TExpression):integer;    {运算表达式}
begin
     if expr.etype=1 then result:=expr.constant;   {<常量>}
     if expr.etype=2 then result:=variable[expr.var1]+variable[expr.var2];
                                                         {<变量>+<变量>}
     if expr.etype=3 then begin   {<变量> > <变量>}
        if variable[expr.var1]>variable[expr.var2] then result:=1
                                                   else result:=0;
     end;
end;

procedure run;	{主程序运行}
var no:integer;       {no是程序运行指针}
    tmp:TStatement;
    over:boolean;
begin
     no:=1;           {从第一条语句开始}
     over:=false;
     assign(f,opf); rewrite(f);    {指定输出文件}
     while not over do begin         {如果程序没有结束则继续执行下去}
           tmp:=prog[no];               {取出当前语句，并解释执行}
           case tmp.stype of
       {类型1：LET语句}
                1: begin variable[tmp.var1]:=result(tmp.expr);
                         inc(no);
                   end;
       {类型2：PRINT语句}
                2: begin writeln(f,tmp.var1,'=',variable[tmp.var1]);
                         inc(no);
                   end;
       {类型3：GOOT语句}
                3: no:=result(tmp.expr);
       {类型4：IF语句}
                4: if result(tmp.expr)=0 then inc(no,2)
                                         else inc(no);
       {类型5：STOP语句}
                5: over:=true;
           end;
     end;
     close(f); {关闭文件}
end;

begin	{主程序}
     initialize;   {初始化}
     run;          {模拟程序运行}
end.
