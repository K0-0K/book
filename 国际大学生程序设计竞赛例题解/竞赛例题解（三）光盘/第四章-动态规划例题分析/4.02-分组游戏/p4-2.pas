program GDOI2002_Group;
const
  IPF='group.in';
  OPF='group.std';
  MAXN=100;
var
  n,t:integer;
  rec:array[1..MAXN,1..MAXN] of boolean;
  selected:array[1..MAXN] of boolean;
  mark:array[1..MAXN] of integer;
  tot:array[-MAXN..MAXN] of integer;
  exp:array[0..8000,1..3] of integer;
  ans:array[1..MAXN] of integer;

procedure init;
var
  i,j,x:integer;
begin
  fillchar(rec,sizeof(rec),false);
  readln(n);
  for i:=1 to n do
  begin
    read(x);
    while x<>0 do
    begin
      rec[i,x]:=true;
      read(x);
    end;
  end;
  for i:=1 to n do
    for j:=1 to n do
      if rec[i,j] and not rec[j,i] then
         rec[i,j]:=false;
end;

function unused:integer;
var
  i:integer;
begin
  for i:=1 to n do
    if not selected[i] then
    begin
      unused:=i;
      exit;
    end;
  unused:=-1;
end;

function success:boolean;
var
  k,i,j,p1,p2:integer;
  r:array[1..MAXN,1..2] of integer;
  open,closed:integer;
begin
  success:=false;
  fillchar(selected,sizeof(selected),false);
  fillchar(mark,sizeof(mark),0);
  fillchar(tot,sizeof(tot),0);
  t:=0; k:=unused;
  while k>0 do
  begin
    inc(t);
    fillchar(r,sizeof(r),0);
    r[1,1]:=k; r[1,2]:=1; selected[k]:=true;
    open:=1; closed:=0;
    while closed<open do
    begin
      inc(closed);
      p1:=r[closed,1]; p2:=r[closed,2];
      mark[p1]:=p2*t; inc(tot[p2*t]);
      for i:=1 to n do
        if not selected[i] and not rec[p1,i] then
        begin
          for j:=1 to open do
            if p2<>r[j,2] then
               if not rec[i,r[j,1]] then exit;
          inc(open);
          r[open,1]:=i; r[open,2]:=-p2;
          selected[i]:=true;
        end;
    end;
    k:=unused;
  end;
  success:=true;
end;

procedure calc(k:integer);
var
  x,y,xx,yy,father,level,flag,i:integer;
begin
  fillchar(ans,sizeof(ans),0);
  level:=t;
  while level>0 do
  begin
    father:=exp[k,3];
    x:=exp[k,1]; y:=exp[k,2];
    xx:=exp[father,1]; yy:=exp[father,2];
    if x=xx+tot[level] then flag:=1
       else flag:=-1;
    for i:=1 to n do
    begin
      if mark[i]=flag*level then ans[i]:=1;
      if mark[i]=-flag*level then ans[i]:=2;
    end;
    k:=father;
    dec(level);
  end;
end;

procedure print(k:integer);
var
  i,j:integer;
begin
  for i:=1 to 2 do
  begin
    write(exp[k,i]);
    for j:=1 to n do
      if ans[j]=i then write(' ',j);
    writeln;
  end;
end;

procedure done;
var
  level,head,tail,oldtail,i,j,k,xx,yy,x,y:integer;
  min:integer;
  old:array[0..MAXN,0..MAXN] of boolean;

  procedure deal(x,y,z:integer);
  begin
    if old[x,y] then exit;
    inc(tail);
    exp[tail,1]:=x;
    exp[tail,2]:=y;
    exp[tail,3]:=z;
    old[x,y]:=true;
  end;

begin
  level:=0;
  fillchar(exp,sizeof(exp),0);
  exp[0,3]:=-1;
  head:=0; tail:=0;
  fillchar(old,sizeof(old),false);
  for level:=1 to t do
  begin
    oldtail:=tail;
    for j:=head to oldtail do
    begin
      xx:=exp[j,1]; yy:=exp[j,2];
      x:=xx+tot[level]; y:=yy+tot[-level];
      deal(x,y,j);
      x:=xx+tot[-level]; y:=yy+tot[level];
      deal(x,y,j)
    end;
    head:=oldtail+1;
  end;

  min:=abs(exp[head,1]-exp[head,2]); k:=head;
  for i:=head+1 to tail do
    if min>abs(exp[i,1]-exp[i,2]) then
    begin
      k:=i;
      min:=abs(exp[i,1]-exp[i,2]);
    end;

  calc(k);
  print(k);
end;

begin
  assign(input,IPF); reset(input);
  assign(output,OPF); rewrite(output);
  init;
  if not success then writeln('No solution')
     else done;
  close(input); close(output);
end.
