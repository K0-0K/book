program street;

const
  MaxNode = 1000; {最大路口数}
  MaxDeg = 4; {每个路口最多连接的街道数；这是一个典型的稀疏图}
  MaxEdge = MaxNode * MaxDeg div 2; {最大街道数}

var
  G: array[1 .. MaxNode, 1 .. MaxDeg] of Integer;
  {图G采用邻接表存储}
  Deg: array[1 .. MaxNode] of Integer;
  {每个顶点的度数（对应路口连接的街道数）}

  Dfs: array[1 .. MaxNode] of Integer;
  {dfs[v]=c}
  Back: array[1 .. MaxNode] of Integer;
  {back[v]=顶点v或v的后代所能追溯到的最早的祖先点的深度优先遍历数}

  V_Stack, W_Stack: array[1 .. MaxEdge] of Integer;
  {v_stack,w_stack：记录深度优先遍历所经过的边的栈}
  N_Stack: Integer;
  Dfn: Integer; {当前深度优先遍历数}

{求X和Y之中的较小者}
function Min(X, Y: Integer): Integer;
begin
  if X < Y then Min := X else Min := Y;
end;

{深度优先，求图G的强连通分支}
procedure Bicomponent(V, Pred: Integer); {v是深度优先遍历的根；pred是上一个顶点}
var
  I, W, Count: Integer;
begin
  Inc(Dfn);  {搜索进入一个新的顶点，深度优先遍历数加一}
  Dfs[V] := Dfn;  {记下该顶点的深度优先遍历数}
  Back[V] := Dfn;  {一开始v只能追溯到v自己，没有路返回更高的祖先}

  for I := 1 to Deg[V] do {检查顶点V的所有邻接顶点}
  begin
    W := G[V, I];
    if (Dfs[W] < Dfs[V]) and (W <> Pred) then
	begin
	  {反向边或未检查的边}
	  V_Stack[N_Stack] := V;	{将边<v.w>入栈}
	  W_Stack[N_Stack] := W;
	  Inc(N_Stack);
	end;
	if Dfs[W] = 0 then	{w尚未访问过，是前向边}

	begin
	  Bicomponent(W, V); {以w为根继续深度优先搜索}
	  if Back[W] >= Dfs[V] then
	  begin
	    {找到新的强连通分支}
		Count := 0;	{count强连通分支包含的顶点数}
		{输出强连通分支}
		while (V_Stack[N_Stack - 1] <> V) or (W_Stack[N_Stack - 1] <> W) do
		begin
          Writeln(V_Stack[N_Stack-1], ' ', W_Stack[N_Stack - 1]);
		  Dec(N_Stack);	{出栈}
		  Inc(Count);
		end;
		Writeln(V_Stack[N_Stack - 1], ' ', W_Stack[N_Stack - 1]);
		if Count = 0 then
		  {找到一个只含一条边的强连通分支}
		  Writeln(W_Stack[N_Stack - 1], ' ', V_Stack[N_Stack - 1]);
		Dec(N_Stack);
	  end
	  else
	    Back[V] := Min(Back[V], Back[W]);
	end
	else
	  {w已经检查过了}
	  Back[V] := Min(Back[V], Dfs[W]);
  end;
end;

var
  N, M, I, J, K: Integer;

begin
  Assign(Input, 'street.in');   {指定输入文件}
  Reset(Input);
  Assign(Output, 'street.out'); {指定输出文件}
  Rewrite(Output);

  {读入数据}
  Readln(N, M);
  for I := 1 to N do Deg[I] := 0;
  for K := 1 to M do
  begin
    Readln(I, J);
    Inc(Deg[I]);
    Inc(Deg[J]);
    G[I, Deg[I]] := J;
    G[J, Deg[J]] := I;
  end;

  {初始化调用的参数}
  N_Stack := 1;
  Dfn := 0;
  for I := 1 to N do Dfs[I] := 0;

  {深度优先求出连通分支，同时输出结果}
  Bicomponent(1, -1);

  Close(Input);   {关闭输入文件}
  Close(Output);  {关闭输出文件}
end.
