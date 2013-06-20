{*
 * Judging program for "Street Directions"
 *
 * Copyright Chen Mingrei (ZSUCPC 2001)
 * Date: May 10, 2001
 *
 * Comment: This program displays and checks the output of a program to see
 *          if it correct.  The following are checked:
 *
 *          1. out of range vertex numbers
 *          2. duplicate edges in output
 *          3. incorrect vertex degree
 *          4. solution contains extra streets not in the original map
 *          5. some streets have no direction assigned
 *          6. incorrect number of two-way streets
 *          7. reachability from any intersection to any other intersections
 *          8. format error (invalid number)
 *
 *}

var
	InFile, OutFile, StdFile: Text;
	NoOutput: Boolean;

function Left(var S: string; N: Integer): string;
var
	Temp: string;
begin
	Temp := Copy(S, 1, N);
	while Length(Temp) < N do Temp := Temp + ' ';
	Delete(S, 1, N);
	Left := Temp;
end;

procedure Show;
var
	InStr, OutStr: string;
begin
	Reset(InFile);
	{$I-}
	Reset(OutFile);
	{$I+}
	NoOutput := IOResult <> 0;
	Write('Judging program for "Street Directions"     Copyright Chen Mingrei (ZSUCPC 2001)');
	Writeln('Input: street.in                       � Output: street.out');
	Write('컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컵컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴');
	InStr := '';
	if NoOutput then OutStr := 'Fail to open "street.out"!'
	else OutStr := '';
	repeat
		If not Eof(InFile) and (InStr = '') then Readln(InFile, InStr);
		If not NoOutput and not Eof(OutFile) and (OutStr = '') then Readln(OutFile, OutStr);
		Writeln(Left(InStr, 38) + ' � ' + Left(OutStr, 38));
	until Eof(InFile) and (NoOutput or Eof(OutFile)) and (InStr = '') and (OutStr = '');
end;

procedure Report(S: string);
begin
	if S = '' then Writeln('Judgement: Yes! ^_^')
	else Writeln('Judgement: No - ', S, '.');
	Close(InFile);
	if not NoOutput then Close(OutFile);
	Close(StdFile);
	Readln;
	Halt;
end;

function MyStr(I: Integer): string;
var
	S: string;
begin
	Str(I, S);
  MyStr := S;
end;

const
	MAX_N = 1000;
	MAX_DEG = 4;
	MAX_M = MAX_DEG * MAX_N div 2;

type
	Tdeg = array[1 .. MAX_N] of Integer;
	Tadj = array[1 .. MAX_N, 0 .. MAX_DEG - 1] of Integer;
	Tvisited = array[1 .. MAX_N] of Boolean;

var
	n, m: Integer;
	deg1, deg2: Tdeg;
	adj1, adj2: Tadj;
	dfs: array[1 .. MAX_N] of Integer;
	back:array [1 .. MAX_N] of Integer;
	n_stack, dfn: Integer;
	v_stack: array[0 .. MAX_M - 1] of Integer;
	w_stack: array[0 .. MAX_M - 1] of Integer;
	TwoWay1, TwoWay2: Integer;


function Min(x, y: Integer): Integer;
begin
	if x < y then Min := x else Min := y
end;

function DFSearch(var deg: Tdeg; var adj: Tadj; src, prev: Integer; var visited: Tvisited): Boolean;
var
	i, w: Integer;
begin
	if visited[src] then
		begin	DFSearch := False; Exit end;

	if (1 <= src) and  (src <= prev) then
		begin DFSearch := True; Exit end;

	visited[src] := True;
	for i := 0 to deg[src] - 1 do
	begin
		w := adj[src, i];
		if DFSearch(deg, adj, w, prev, visited) then
			begin DFSearch := True; Exit end;
	end;

	DFSearch := False;
end;

function CheckReachability(var deg: Tdeg; var adj: Tadj): Boolean;
var
	visited: Tvisited;
	i, j: Integer;
begin
	for i := 1 to n do visited[i] := False;

	DFSearch(deg, adj, 1, 0, visited);

	for i :=1 to n do
	if not visited[i] then
		begin CheckReachability := False; Exit end;

	for i := 2 to n do
	begin
		for j := 1 to n do visited[j] := False;

		if not DFSearch(deg, adj, i, i-1, visited) then
			begin CheckReachability := False; Exit end
	end;

	CheckReachability := True;
end;

procedure ReadData(var deg: Tdeg; var adj: Tadj);
var
	i, u, v: Integer;
begin
	Read(InFile, n, m);
	for i := 1 to n do deg[i] := 0;
	for i := 0 to m - 1 do
	begin
		Read(InFile, u, v);
		adj[u, deg[u]] := v; Inc(deg[u]);
		adj[v, deg[v]] := u; Inc(deg[v]);
	end;
end;

procedure ReadSoln(var deg: Tdeg; var adj: Tadj);
var
	i, u, v, t: Integer;
begin
	for i := 1 to n do deg[i] := 0;

	while not eof(OutFile) do
	begin
		{$I-}
		Read(OutFile, u, v);
		{$I+}
		if IOResult <> 0 then Report('Format error: invalid number');

		if (u < 1) or (u > n) or (v < 1) or (v > n) or (u = v) then
			Report('Vertices out of range');

		for t := 0 to deg[u] - 1 do
			if adj[u][t] = v then Report('Duplicate edge');

		adj[u][deg[u]] := v; Inc(deg[u]);
		if (deg[u] < 0) or (deg[u] > 4) then Report('Wrong degree');

		if not Eoln(OutFile) then
			Report('Format error: not end of line after an edge')
		else
			Readln(OutFile);
	end;
end;

function CheckEdge(var deg: Tdeg; var adj: Tadj; u, v: Integer): Boolean;
var
	i: Integer;
begin
	for i := 0 to deg[u] - 1 do
		if adj[u, i] = v then
			begin CheckEdge := True; Exit end;
	CheckEdge := False;
end;

procedure CheckEdges(var deg1: Tdeg; var adj1: Tadj; var deg2: Tdeg; var adj2: Tadj);
var
	u, i: Integer;
begin
	{ check that all edges in graph 2 are edges in graph 1 }
	for u := 1 to n do
	for i := 0 to deg2[u] - 1 do
		if not CheckEdge(deg1, adj1, u, adj2[u, i]) then { extra edges in graph 2 }
			Report('Solution graph contains extra edges');

	{ check that edges in graph 1 has been assigned a direction in graph 2 }
	for u := 1 to n do
	for i := 0 to deg1[u] - 1 do
		if not CheckEdge(deg2, adj2, u, adj1[u, i]) and
		not CheckEdge(deg2, adj2, adj1[u, i], u) then { unassigned edges }
		Report('Unassigned edge');
end;

procedure Bicomp(var deg: Tdeg; var adj: Tadj; v, pred: Integer);
var
	i, w, count: Integer;
begin
	Inc(dfn);
	dfs[v] := dfn; back[v] := dfn;
	for i := 0 to deg[v] - 1 do
	begin
		w := adj[v, i];
		if (dfs[w] < dfs[v]) and (w <> pred) then
		begin
			{ back edge or unexamined forward edge }
			v_stack[n_stack] := v;
			w_stack[n_stack] := w;
			Inc(n_stack);
		end;
		if dfs[w] = 0 then
		begin
			bicomp(deg, adj, w, v);
			{ back up from recursion }
			if back[w] >= dfs[v] then
			begin
				{ new bicomponent }
				count := 0;
				while (v_stack[n_stack-1] <> v) or (w_stack[n_stack-1] <> w) do
				begin
{		      assert(n_stack > 0); }
					Dec(n_stack);
					Inc(count);
				end;
				if count = 0 then
				{ we have a bicomponent containing only one edge }
					Inc(TwoWay1);
				Dec(n_stack);
			end
			else
				back[v] := min(back[v], back[w]);
		end
		else
			{ w has been examined already }
			back[v] := min(back[v], dfs[w]);
	end;
end;

function CountTwoWay(var deg: Tdeg; var adj: Tadj): Integer;
var
	res, u, i: Integer;
begin
	res := 0;

	for u := 1 to n do
	for i := 0 to deg[u] - 1 do
		if CheckEdge(deg, adj, adj[u, i], u) then Inc(res);

{  assert(res % 2 == 0); }
	CountTwoWay := res div 2;
end;

procedure Judge;
var
	i: Integer;
begin
	Reset(InFile); Reset(StdFile);

	If NoOutput then Report('No output file');

	Reset(OutFile);
	if Eof(OutFile) then Report('No output');

	ReadData(deg1, adj1);
	ReadSoln(deg2, adj2);
	CheckEdges(deg1, adj1, deg2, adj2);

	{ compute number of two-way streets }
	TwoWay1 := 0; n_stack := 0; dfn := 0;
	for i:= 1 to n do dfs[i] := 0;
	Bicomp(deg1, adj1, 1, -1);
	TwoWay2 := CountTwoWay(deg2, adj2);
	if TwoWay1 <> TwoWay2 then { incorrect number of two-way streets }
		Report('Incorrect number of two-way streets');

	if not CheckReachability(deg2, adj2) then { not reachable }
		Report('Not reachable');

	Report('');
end;

begin
	Assign(InFile, 'street.in');
	Assign(OutFile, 'street.out');
	Assign(StdFile, 'street.std');

	Show;
	Judge;
end.