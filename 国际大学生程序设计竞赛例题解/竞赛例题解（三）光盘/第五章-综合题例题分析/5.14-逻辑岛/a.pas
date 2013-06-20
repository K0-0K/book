program island;

const
  MaxInhabitant = 'E'; {最大居民数}
MaxStatement = 100; {最大对话数}

type
  {枚举用到的值：天使，魔鬼，人类，白天，晚上，说谎，不知道，不确定}
  TEnum = (Divine, Evil, Human, Day, Night, Lie, Unknown, Undeducible);

  {记录状态的数据结构}
  {函数KIND(state, speaker)可用state.kind实现；函数TIME(state)可用state.time实现}
  TState = record
    Kind: array['A' .. MaxInhabitant] of TEnum;  {每个居民的身份}
    Time: TEnum;                                 {时间}
  end;

  {形式化记录对话的数据结构}
  TStatement = record
    Spk: Char; {说话者}
    Sub: Char; {主语，为#0的时候，对话内容是关于时间的}
    Obj: TEnum; {宾语}
    Neg: Boolean; {是否出现否定}
  end;

var
  N: Integer; {对话数}
  Statement: array[1 .. MaxStatement] of TStatement; {所有的对话}
  State, Fact: TState; {当前状态和能够推导出来的事实}

{判断一个身份为kind的居民在时间为time时是否说谎}
function Lying(Kind, Time: TEnum): Boolean;
begin
  Lying := (Kind = Evil) or ((Kind = Human) and (Time = Night));
            {魔鬼，或者人在晚上，都在说谎}
end;

{判断对话statement是否符合条件组合state}
function Truth(var Statement: TStatement; var State: TState): Boolean;
var
  Res: Boolean;
begin
  if Statement.Sub = #0 then Res := Statement.Obj = State.Time {关于时间的}
  else if Statement.Obj = Lie then Res := Lying(State.Kind[Statement.Sub], State.Time) {关于说谎的}
  else Res := Statement.Obj = State.Kind[Statement.Sub]; {关于居民类型的}
  if Statement.Neg then Res := not Res; {出现否定吗？}
  Truth := Res;
end;

{分析对话有无矛盾，若全部无矛盾则分析可推导的事实}
procedure Analysis;
var
  I: Integer;
  J: Char;
begin
  for I := 1 to N do   {逐一分析每一句话}
if Lying(State.Kind[Statement[I].Spk], State.Time) xor Truth(Statement[I], State) then
{对话没有矛盾}
    else Exit; {有矛盾则退出}
  {对话全部无矛盾，分析可推导的事实}
  if Fact.Time = Unknown then {第一个可行的状态，直接记录之}
    Fact := State
  else begin
    {若与已经存在的事实不符，则是无法确定的事实}
    for J := 'A' to MaxInhabitant do
      if Fact.Kind[J] <> State.Kind[J] then Fact.Kind[J] := Undeducible;
                   {无法确定的居民身份}
if Fact.Time <> State.Time then Fact.Time := Undeducible;
               {时间也可能无法确定}
  end;
end;

{尝试所有可能的居民身份}
procedure TryAll(Now: Char);
begin
  if Now > MaxInhabitant then Analysis  {所有居民都赋予身份，则可以进行分析}
  else begin
    State.Kind[Now] := Divine;   {假设居民Now为天使}
    TryAll(Succ(Now));
    State.Kind[Now] := Evil;     {假设居民Now为魔鬼}
    TryAll(Succ(Now));
    State.Kind[Now] := Human;    {假设居民Now为人类}
    TryAll(Succ(Now));
  end;
end;

{求解逻辑岛问题，并输出结果}
procedure Solve;
const
  KindStr: array[TEnum] of string   {TEnum每个元素对应的字符串表示输出}
 = ('divine', 'evil', 'human', 'day', 'night', '', '', '');

var
  NoFacts: Boolean;
  I: Char;
begin
  {初始化事实}
  for I:='A' to MaxInhabitant do Fact.Kind[I]:=Unknown;
  Fact.Time:=Unknown;
  {枚举所有的状态}
  State.Time := Day;      {假设时间为白天}
  TryAll('A');
  State.Time := Night;    {假设时间为晚上}
  TryAll('A');
  {判断可以推导的事实}
  if Fact.Time = Unknown then {对话存在悖论}
Writeln('This is impossible.')
  else begin
    {输出可以推导的事实}
    NoFacts := True;
    for I := 'A' to MaxInhabitant do   {输出每个确定的居民身份}
      if Fact.Kind[I] <> Undeducible then
      begin
        Writeln(I, ' is ', KindStr[Fact.Kind[I]], '.');
        NoFacts := False;
      end;
    if Fact.Time <> Undeducible then   {时间确定，也输出}
    begin
      Writeln('It is ', KindStr[Fact.Time], '.');
      NoFacts := False;
    end;
    {没有推导出任何事实？}
    if NoFacts then Writeln('No facts are deducible.')
  end;
end;
{读入所有的对话，并形式话}
procedure ReadStatements;
var
  I: Integer;
  S: string;
begin
  Readln(N);  {读入语句数量}
  for I := 1 to N do  {逐一读入和分析每一条语句}
  begin
    Readln(S);      {读入语句}
    Statement[I].Spk := S[1]; {取出说话者}
    if Copy(S, 4, 2) = 'It' then {关于时间的：“It is ( day | night )”}
    begin
      Statement[I].Sub := #0;
      if Pos('day', S) > 0 then
        Statement[I].Obj := Day
      else
        Statement[I].Obj := Night;
    end
    else begin   {关于身份的：“I am [not]……”或者“X is [not]……”}
      if S[4] = 'I' then {取出主语}
        Statement[I].Sub := S[1]
      else
        Statement[I].Sub := S[4];
     {判断宾语}
      if Pos('divine', S) > 0 then     {是天使}
        Statement[I].Obj := Divine
      else if Pos('evil', S) > 0 then     {是魔鬼}
        Statement[I].Obj := Evil
      else if Pos('human', S) > 0 then   {是人类}
        Statement[I].Obj := Human
      else if Pos('lying', S) > 0 then    {在说谎}
        Statement[I].Obj := Lie;
    end;
    {判断否定的情况}
    Statement[I].Neg := Pos('not', S) > 0;
  end;
end;
begin
  Assign(Input, 'island.in');    {指定输入输出文件}
  Reset(Input);
  Assign(Output, 'island.out');
  Rewrite(Output);
  ReadStatements; {读入数据}
  Solve; {求解并输出结果}
  Close(Input); {关闭文件}
  Close(Output); {关闭文件}
end.
