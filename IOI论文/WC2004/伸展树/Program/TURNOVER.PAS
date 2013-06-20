Program Turnover_SplayTree_FP;
Const
    inf='turnover.in';
    outf='turnover.out';
    maxn=32767;

Type
    Tp=record
           data:longint;
           father,left,right:integer;
       end;

Var
    n,now,root,prev,next,same:integer;
    min:longint;
    ans:extended;
    tree:array[0..maxn]of Tp;

procedure init;
begin
    fillchar(tree,sizeof(tree),0);
    read(n);
    read(tree[1].data);
    root:=1;
    ans:=tree[1].data;
end;

procedure leftrotate(x:integer);
var y:integer;
begin
    y:=tree[x].father;

    tree[y].right:=tree[x].left;
    if tree[x].left<>0 then tree[tree[x].left].father:=y;

    tree[x].father:=tree[y].father;
    if tree[y].father<>0 then begin
        if y=tree[tree[y].father].left
          then tree[tree[y].father].left:=x
          else tree[tree[y].father].right:=x;
    end;
    tree[y].father:=x;
    tree[x].left:=y;
end;

procedure rightrotate(x:integer);
var y:integer;
begin
    y:=tree[x].father;

    tree[y].left:=tree[x].right;
    if tree[x].right<>0 then tree[tree[x].right].father:=y;

    tree[x].father:=tree[y].father;
    if tree[y].father<>0 then begin
        if y=tree[tree[y].father].left
          then tree[tree[y].father].left:=x
          else tree[tree[y].father].right:=x;
    end;

    tree[y].father:=x;
    tree[x].right:=y;
end;

procedure splay(now:integer);
var t:integer;
begin
    while tree[now].father<>0 do
      begin
          t:=tree[now].father;

          if tree[t].father=0 then begin
              if now=tree[t].left
{ZIG}           then rightrotate(now)
{ZAG}           else leftrotate(now);
              break;
          end;

          if now=tree[t].left then begin
              if t=tree[tree[t].father].left then begin
{ZIG-ZIG}         rightrotate(t);
                  rightrotate(now);
              end
              else begin
{ZIG-ZAG}         rightrotate(now);
                  leftrotate(now);
              end;
          end
          else begin
              if t=tree[tree[t].father].right then begin
{ZAG-ZAG}         leftrotate(t);
                  leftrotate(now);
              end
              else begin
{ZAG-ZIG}         leftrotate(now);
                  rightrotate(now);
              end;
          end;
      end;

    root:=now;
end;

procedure insert;
var t:integer;
begin
    t:=root; same:=now;
    while true do
      begin
          if tree[t].data=tree[now].data
            then begin min:=0; same:=t; exit; end;
          if tree[t].data>tree[now].data then begin
              if tree[t].left=0 then begin
                  tree[now].father:=t;
                  tree[t].left:=now;
                  exit;
              end
              else t:=tree[t].left;
          end
          else begin
              if tree[t].right=0 then begin
                 tree[now].father:=t;
                 tree[t].right:=now;
                 exit;
              end
              else t:=tree[t].right;
          end;
      end;
end;

procedure findprev;
begin
    prev:=tree[now].left;
    while prev<>0 do
      begin
          if tree[prev].right=0 then exit;
          prev:=tree[prev].right;
      end;
end;

procedure findnext;
begin
    next:=tree[now].right;
    while next<>0 do
      begin
          if tree[next].left=0 then exit;
          next:=tree[next].left;
      end;
end;

procedure work;
begin
    read(tree[now].data);
    min:=maxlongint;

    insert;
    splay(same);
    if min>0 then begin
      findprev;
      findnext;
      if (next<>0) and ((prev=0) or
         (tree[next].data-tree[now].data<tree[now].data-tree[prev].data))
      then min:=tree[next].data-tree[now].data
      else min:=tree[now].data-tree[prev].data;
    end;

    ans:=ans+min;
end;

procedure main;
begin
    assign(input,inf); reset(input);
    assign(output,outf); rewrite(output);
      init;
      for now:=2 to n do work;
      writeln(ans:0:0);
    close(input);
    close(output);
end;

Begin
    main;
End.
