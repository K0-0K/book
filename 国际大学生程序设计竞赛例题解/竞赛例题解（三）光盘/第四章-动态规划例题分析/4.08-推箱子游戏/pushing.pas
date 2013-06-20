{ Sample program for "Pushing Boxes"(ZSUCPC2001) }
const
  Max=20; {仓库长和宽的最大维数}
  Dx: array[1..4] of ShortInt = (-1, 0, 1, 0); {X方向偏移量}
  Dy: array[1..4] of ShortInt = (0, 1, 0, -1); {Y方向偏移量}
  DName: array[1..4] of Char = 'nesw'; {方向名称}
type
  TNode = record {状态结点类型}
            X, Y, D: ShortInt; {(X,Y)=箱子坐标，D=从起点到(X,Y)的最小代价}
            From: Word; {从那个结点扩展得来}
          end;
  TMaze = array[1..Max, 1..Max] of Char; {仓库地图类型}
var
  r, c, SX, SY, BX, BY, TX, TY: ShortInt; {r,c=仓库的长和宽；(SX,SY)=人起始坐标，(BX,BY)=箱子起始坐标，(TX,TY)=箱子目标坐标}
  Closed, Open: Word; {Closed表和Open表指针}
  Maze, G: TMaze; {Maze=仓库地图，G=临时地图}
{Maze[i, j] = '.'表示仓库地图中坐标(i, j)处是空格}
{Maze[i, j] = '#'表示仓库地图中坐标(i, j)处是墙壁}
  Mark: array[1..Max, 1..Max, 1..4] of Boolean; {标志数组，Mark[x,y,k]=True表示可以在(x,y)坐标从k方向将箱子推到邻近的方格中}
  List: array[1..Max*Max*4] of TNode; {状态数组}

function Valid(X, Y: ShortInt):Boolean; {判坐标(X,Y)是否出界}
begin
  Valid := (X >= 1) and (X <= r) and (Y >= 1) and (Y <= c);
end;

procedure InputMaze; {输入仓库地图并进行预处理}
var
  i, j, k:Byte;
begin
  Readln(r, c); {读入仓库的长和宽}
  for i := 1 to r do
  begin
    for j := 1 to c do
    begin
      Read(Maze[i, j]);
      case Maze[i, j] of {确定（人和箱子的）起始和目标坐标}
        'S': begin SX := i; SY := j end;
        'B': begin BX := i; BY := j end;
        'T': begin TX := i; TY := j end;
      end;
      {如果Maze[i, j]不是墙壁，则令Maze[i, j]是空格}
      if Maze[i, j] <> '#' then Maze[i, j] := '.';
    end;
    Readln;
  end;
  for i := 1 to r do {预处理，生成Mark标志数组}
  for j := 1 to c do
    if Maze[i, j] = '#' then FillChar(Mark[i, j], SizeOf(Mark[i, j]), False)
    else for k := 1 to 2 do
      if Valid(i-Dx[k], j-Dy[k]) and Valid(i+Dx[k], j+Dy[k]) then
        begin Mark[i, j, k] := True; Mark[i, j, k+2] := True end
      else
        begin Mark[i, j, k] := False; Mark[i, j, k+2] := False end;
end;

procedure DFS(i, j: ShortInt); {深度优先搜索}
var
  k:Byte;
begin
  G[i, j] := 'Y'; {标记“访问过”}
  for k := 1 to 4 do {若邻接点未访问过，则访问之}
    if Valid(i+Dx[k], j+Dy[k]) and (G[i+Dx[k], j+Dy[k]]='.') then
      DFS(i+Dx[k], j+Dy[k]);
end;

procedure Expand(SX, SY, BX, BY: ShortInt); {深度优先搜索，判断人可达的点}
begin
  G := Maze; {复制临时仓库地图}
  G[BX, BY] := 'B'; {标记箱子位置}
  DFS(SX, SY); {从人的当前位置开始深度优先搜索，判断人可达的点}
end;

{寻找从(X1,Y1)到(X2,Y2)人走的最短路，箱子位置在(BX,BY)}
procedure PrintWalk(X1, Y1, X2, Y2, BX, BY: ShortInt);
var
  Q: array[1..Max*Max] of TNode; {广度优先搜索的队列}
  Head, Tail: Word; {队列头尾指针}
  k: Byte;
begin
  if (X1 = X2) and (Y1 = Y2) then Exit; {起点终点相同，无需走}
  Head := 1; Tail := 1; G := Maze; G[BX, BY] := 'B'; {初始化队列指针，复制临时仓库地图，标记箱子位置}
  with Q[1] do {设置队列头结点}
    begin X := X2; Y := Y2; From := 0 end;
  while Head <= Tail do {广度优先搜索从(X2,Y2)到(X1,Y1)的最短路}
  with Q[Head] do
  begin
    for k := 1 to 4 do
      {(X-Dx[k], Y-Dy[k])不出界和G[X-Dx[k], Y-Dy[k]]是空格且未被扩展}
      if Valid(X-Dx[k], Y-Dy[k]) and (G[X-Dx[k], Y-Dy[k]]='.') then
      begin {扩展结点}
        G[X-Dx[k], Y-Dy[k]] := 'Y'; {标记G[X-Dx[k], Y-Dy[k]]已经被扩展}
        Inc(Tail);
        Q[Tail].X := X-Dx[k]; Q[Tail].Y := Y-Dy[k];
        Q[Tail].D := k; Q[Tail].From := Head;
        if (Q[Tail].X = X1) and (Q[Tail].Y = Y1) then {到达起点，输出路径}
        begin
          while Q[Tail].From <> 0 do
          begin
            Write(DName[Q[Tail].D]); {输出Q[Tail].D对应的“走”的方向}
            Tail := Q[Tail].From; {转到父结点}
          end;
          Exit;
        end;
      end;
    Inc(Head); {Q[Head]扩展完毕，扩展下一个结点}
  end;
end;

{从List[Node]结点逆推，输出“走”和“推”的序列}
procedure PrintSolution(Node: Word);
begin
  if List[Node].From=0 then {“推”序列第一个元素}
    with List[Node] do {输出人从起点到“推”序列第一个元素人的位置“走”的路径}
      PrintWalk(SX, SY, X-Dx[D], Y-Dy[D], X, Y)
  else
    with List[Node] do
    begin
	  {递归输出到达“推”序列前一个元素的“走”和“推”的序列}
      PrintSolution(From);
{输出人在两个“推”序列之间人的位置“走”的路径}
      PrintWalk(List[From].X, List[From].Y, X-Dx[D], Y-Dy[D], X, Y);
    end;
  Write(Upcase(DName[List[Node].D])); {输出List[Node].D对应的“推”的方向}
end;

procedure Find; {寻找从(BX,BY)到(TX,TY)箱子推的最短路}
var
  k: Byte;
begin
  Closed := 1;Open := 0; {初始化队列指针}
  Expand(SX, SY, BX, BY); {找出箱子在(BX,BY)时，人从(SX,SY)可达的结点}
  for k := 1 to 4 do
{如果可以在(BX, BY)坐标从k方向将箱子推到邻近的方格中且(BX, BY, k)未“生成过”，和(BX-Dx[k], BY-Dy[k])从(SX,SY)可达}
if Mark[BX, BY, k] and (G[BX-Dx[k], BY-Dy[k]] = 'Y') then
begin {生成队列头元素}
      Inc(Open);
      with List[Open] do
        begin X := BX; Y := BY; D := k; From := 0 end;
      Mark[BX, BY, k] := False; {标记(BX, BY, k)已经“生成过”}
    end;
  while Closed<=Open do {广度优先搜索从(BX,BY)到(TX,TY)的最短路}
  begin
    with List[Closed] do
    begin
      BX := X+Dx[D]; BY := Y+Dy[D]; {新的箱子位置}
      if (BX = TX) and (BY = TY) then {到达目标坐标，输出“走”和“推”的序列}
        begin
{从List[Closed]结点逆推，输出“走”和“推”的序列}
PrintSolution(Closed);
Writeln;
Exit
end;
      Expand(X, Y, BX, BY); {找出箱子在(BX,BY)时，人从(X,Y)可达的结点}
    end;
for k := 1 to 4 do
{如果可以在(BX, BY)坐标从k方向将箱子推到邻近的方格中且(BX, BY, k)未“生成过”，和(BX-Dx[k], BY-Dy[k])从(SX,SY)可达}
      if Mark[BX, BY, k] and (G[BX-Dx[k], BY-Dy[k]] = 'Y') then
      begin {扩展结点}
        Inc(Open);
        with List[Open] do
          begin X := BX; Y := BY; D := k; From := Closed end;
        Mark[BX, BY, k] := False; {标记(BX, BY, k)已经“生成过”}
      end;
    Inc(Closed); {List[Closed]扩展完毕，扩展下一个结点}
  end;
  Writeln('Impossible.'); {队列空，不可能将箱子从起点推到终点}
end;

begin
  Assign(Input, 'pushing.in'); Reset(Input);
  Assign(Output, 'pushing.out'); Rewrite(Output);
  InputMaze; {输入仓库地图}
  Find; {广度优先搜索推箱子游戏的最优解}
  Close(Input);
  Close(Output);
end.
