Program Random_Numbers;
const
  Digits = 30; {长整数的最大长度}
  O = 10000; {长整数权值}
  MaxN = 200;    {N最大值为200}
  MaxM = 200;    {M最大值为200}

type
  Long = record {自定义长整数类型}
           D: Integer; {长整数长度（以log10O位为单位）}
           Num: array[0..Digits] of Word; {存储长整数（每位权为O）}
         end;

function Max(a, b: Integer): Integer;   {返回a和b两者的最大值}
begin
   if a < b then Max := b else Max := a
end;

{ 自定义长整数运算 } 
procedure SetInt(var a: Long; x: Integer); {设置长整数a的值为x（x<O）}
begin
   a.D := 0; a.Num[0] := x;   {x<=65535，直接保存到最低位即可}
end;

procedure Add(var x, y, res: Long); {长整数加法：res=x+y}
var
  carry: Word; {进位标志}
  i: Integer;
begin
  carry := 0; i := 0;  {进位标志carry置零}
  repeat
    if i <= x.D then carry:=carry+x.Num[i];   {对应位相加}
    if i <= y.D then carry:=carry+y.Num[i];
    res.Num[i] := carry mod O;   {计算第i位数字}
    carry := carry div O;        {计算进位}
    i:=i+1;      {计算更高位}
  until (carry = 0) and (i > x.D) and (i > y.D);   {从低到高，逐位相加}
  res.D := i - 1;    {设结果的位数}
end;


procedure Sub(var x, y, res: Long); {长整数加法：res=x-y（x≥y）}
var
  carry: Word; {借位标志}
  i: Integer;
begin
  carry := 1; i := 0;   {进位标志carry置1}
  repeat
    if i <= x.D then carry:=carry+x.Num[i];              {对应位相减}
    if i <= y.D then carry:=carry+(O - 1 - y.Num[i])
                else carry:=carry+(O - 1);
    res.Num[i] := carry mod O;
    carry := carry div O;        {借位}
    i:=i+1;        {计算更高位}
  until (carry = 1) and (i > x.D) and (i > y.D);   {从低到高，逐位相减}
  i:=i-1;
  while (i > 0) and (res.Num[i] = 0) do i:=i-1; {调整掉前面多余的零}
  res.D := i;      {设结果的位数}
end;

procedure Div2(var a, res: Long); {长整数除2：res=a div 2}
var
  i: Integer;
  carry: Word; 
begin
  carry := 0;   {高位除2的余数}
  for i := a.D downto 0 do   {从高到低，逐位试商}
  begin
    carry := carry * O + a.Num[i];
    res.Num[i] := carry div 2;   {上商}
    carry := carry mod 2        {计算当前余数}
  end;
  i := a.D;
  while (i > 0) and (res.Num[i] = 0) do i:=i-1; {调整掉前面多余的零}
  res.D := i;     {设结果的位数}
end;
function Cmp(var x, y: Long): Integer;             { 1 若x>y}
var                          {长整数比较函数。Cmp =  0 若x=y}
i: Integer;                                     { -1 若x<y}
begin
  if x.D > y.D then Cmp := 1 {若x的长度比y长则x>y}
  else if x.D < y.D then Cmp := -1 {若x的长度比y短则x<y}
  else begin {否则按字典序比较}
         for i := x.D downto 0 do {从高位到底位，逐一比较}
           if x.Num[i] > y.Num[i] then begin Cmp := 1; exit end
           else if x.Num[i] < y.Num[i] then begin Cmp := -1; exit end;
         Cmp := 0;  {数字都相同，则两个数相等}
  end;
end;

var
  Bellman: array[1..MaxM] of Long; {存储Bellman数组}
  M, N: Integer;
  Long1, A, B, T, Number, Sum: Long; {Number=G(1,T,0.b1b2…bp)}
  cur, i, j: Integer;
  c: char;

begin
Assign(Input, 'random.in'); Reset(Input);    {指定输入输出文件}
  Assign(Output, 'random.out'); Rewrite(Output);
  Readln(M, N);

  SetInt(Long1, 1); {设置常数1}

  for j := N to M do SetInt(Bellman[j], 1); {顺推Bellman数组}
  for i := N - 1 downto 1 do
  begin
    SetInt(Bellman[i], 0);
    for j := M - 1 downto i do {Bellman[M]恒等于1}
      Add(Bellman[j], Bellman[j + 1], Bellman[j]);
         {计算递推式：Bellman[i-1,j] := Bellman[i,j] + Bellman[i,j+1]}
  end;
  SetInt(T, 0); 
  for j := 1 to M do Add(T, Bellman[j], T); {通过Bellman数组计算T}

  SetInt(A, 1); {计算G(1,T,0.b1b2…bp)}
  SetInt(B, 0); Add(B, T, B); {A=1,B=T}
  repeat Read(c); until c = '.'; Read(c); {跳过小数点}
  while ((c = '0') or (c = '1')) and (Cmp(A, B) <> 0) do {模拟二分法}
  begin   {c是当前的小数位}
if c = '0' then begin Add(A, B, B); Div2(B, B) end
                      {得到区间 (A, (A+B) div 2) }
               else begin Add(A, B, A); Div2(A, A); Add(A, Long1, A) end;
                          {得到区间 ((A+B) div 2+1, B) }
    Read(c)
  end;
  SetInt(Number, 0); Add(Number, A, Number); {Number=G(1,T,0.b1b2…bp)}
                                             {Number就是最后的下标}
  {下面求Number对应的u序列}
  cur := 1;
  for i := 1 to N do
  begin
    SetInt(Sum, 0); {累加和初始化}
    cur:=cur-1;
    while Cmp(Number, Sum) = 1 do {求第i位的数字}
    begin
      cur:=cur+1;
      Add(Sum, Bellman[cur], Sum) {累加Bellman数组}
	end;
	if i > 1 then Write(' '); {输出u序列的第i位数字cur}
	Write(cur);
	Sub(Sum, Bellman[cur], Sum); {求下一个数字的位置}
	Sub(Number, Sum, Number);
	{逆推Bellman数组，恢复到上一阶段的Bellman数组}
	for j := i to M - 1 do Sub(Bellman[j], Bellman[j + 1], Bellman[j]);
end;

Writeln;
  Close(Input); Close(Output); {关闭文件}
end.
