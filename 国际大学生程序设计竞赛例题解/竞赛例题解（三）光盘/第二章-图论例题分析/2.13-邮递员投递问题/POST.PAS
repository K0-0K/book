{$R-,S-}
program post;
uses crt;
type pset=set of 1..26;
var
  ch:char;
  found:boolean;
  f:text;
  chg:string;
  totc:byte;
  ph:array[1..1000] of byte;
  g:array[1..26,1..26] of integer;
  n:array[1..26,1..26] of byte;
  lt:word;
  odn1,odn,m:byte;
  t1:word;
  tot:longint;

procedure init;
var
  s:string;
  a,b:byte;
  c:byte;
begin
  clrscr;tot:=0;totc:=0;
  fillchar(n,sizeof(n),0);fillchar(g,sizeof(g),0);
  assign(f,'post.dat');reset(f);
  chg:='';
  readln(f,s);
  while s<>'deadend' do
    begin
      c:=length(s);
      inc(tot,c);
      a:=pos(s[1],chg);
      if a=0 then begin chg:=chg+s[1];a:=length(chg) end;
      b:=pos(s[length(s)],chg);
      if b=0 then begin chg:=chg+s[length(s)];b:=length(chg) end;
      g[a,b]:=c;g[b,a]:=c;inc(n[a,b]);inc(n[b,a]);
      readln(f,s);
    end;
  close(f);
  m:=length(chg);
end;

procedure quickadd;
var
  od:array[1..26] of byte;
  s:array[1..26,1..26] of pset;
  i,j,k,a,i1,j1:byte;
  min:longint;

function findodd:byte;
var
  i,odn,j:byte;
  a:word;
begin
  odn:=0;
  for i:=1 to m do
    begin
      a:=0;
      for j:=1 to m do inc(a,n[i,j]);
      if odd(a) then begin inc(odn);od[odn]:=i end;
    end;
  findodd:=odn
end;

procedure addpath(a,b:byte);
var
  s2:pset;
  i:byte;
begin
  s2:=s[a,b];
  while (s2<>[b]) and (s2<>[]) do
    begin
      for i:=1 to m do if (i in s2) and (n[a,i]>0) then
        begin
          inc(n[a,i]);inc(n[i,a]);
          s2:=s2-[a];
          a:=i;
          break;
        end;
    end;
end;

begin
  for i:=1 to m do for j:=1 to m do
    if g[i,j]<>0 then s[i,j]:=[i]+[j] else
      begin s[i,j]:=[];g[i,j]:=maxint div 2 end;
  for i:=1 to m do for j:=1 to m do for k:=1 to m do
    if (g[i,j]+g[i,k]<g[j,k]) and (n[j,k]=0) then
      begin
        g[j,k]:=g[i,j]+g[i,k];g[k,j]:=g[i,j]+g[i,k];
        s[j,k]:=s[i,j]+s[i,k];s[k,j]:=s[i,j]+s[i,k]
      end;
  odn1:=findodd;
  repeat
    odn:=findodd;
    if odn=0 then
      begin
        for i:=1 to m do for j:=1 to m do if n[i,j]>0 then
          if odd(n[i,j]) then n[i,j]:=1 else n[i,j]:=2;
        exit;
      end;
    min:=maxlongint;
    for i:=1 to odn-1 do for j:=i+1 to odn do
      if g[od[i],od[j]]<min then
        begin
          min:=g[od[i],od[j]];
          i1:=od[i];j1:=od[j];
        end;
    addpath(i1,j1);
  until false;
end;

procedure find;
var
  w:array[1..1000] of byte;
  i,j,k:byte;
  re:boolean;
  s,d:longint;
begin
  clrscr;
  tot:=0;
  for i:=1 to m-1 do for j:=i+1 to m do
    if n[i,j]<>0 then inc(tot,g[i,j]*(n[i,j] and $7f));
  write('Total=',tot);clreol;writeln;
  fillchar(w,sizeof(w),0);i:=1;
  repeat
    repeat
      inc(w[i]);re:=false;
      if w[i]>m then
        begin
          w[i]:=0;re:=true;
          dec(i);if i=0 then exit;
          if i>1 then
            begin
              n[w[i],w[i-1]]:=n[w[i],w[i-1]] and $7f;
              n[w[i-1],w[i]]:=n[w[i-1],w[i]] and $7f;
            end;
        end;
      if (not re) and (i>1) and ((n[w[i],w[i-1]] and $7f=0) or
         (n[w[i],w[i-1]] and $80<>0)) then re:=true;
    until not re;
    if i>1 then
      begin
        n[w[i],w[i-1]]:=n[w[i],w[i-1]] or $80;
        n[w[i-1],w[i]]:=n[w[i-1],w[i]] or $80;
      end;
    if (i>2) and (w[i]=w[1]) then
      begin
        s:=0;d:=0;
        for j:=2 to i do
          begin
            inc(s,g[w[j],w[j-1]]);
            if (n[w[j],w[j-1]] and $7f)=2 then inc(d,g[w[j],w[j-1]])
          end;
        if d*2>s then
          begin
            for j:=2 to i do
              begin
                k:=n[w[j],w[j-1]] and $7f;
                n[w[j],w[j-1]]:=3-k;
                n[w[j-1],w[j]]:=3-k;
              end;
            found:=true;exit;
          end;
      end
    else inc(i);
  until false;
end;

procedure circle(b:byte);
var i,j,k,b1:word;
begin
  j:=ph[b];b1:=b;
  repeat
    i:=1;
    if i=j then inc(i);
    while (i<=m) and (n[j,i]=0) do
      begin
        inc(i);
        if i=j then inc(i);
      end;
    dec(n[j,j],2);
    dec(n[j,i]);dec(n[i,j]);
    j:=i;
    inc(t1);inc(b1);
    for k:=t1 downto b1+1 do ph[k]:=ph[k-1];
    ph[b1]:=j;
  until ph[b]=j;
end;

procedure onepen;
var
  i,j:word;
  f:text;
begin
  for i:=1 to m do for j:=1 to m do n[i,j]:=n[i,j] and $7f;
  for i:=1 to m do for j:=1 to m do
    if (i<>j) then inc(n[i,i],n[i,j]);
  tot:=0;
  for i:=1 to m-1 do for j:=i+1 to m do
    if n[i,j]<>0 then inc(tot,g[i,j]*n[i,j]);
  writeln('Total=',tot);clreol;
  t1:=1;ph[1]:=1;
  repeat
    i:=1;
    while (i<=t1) and (n[ph[i],ph[i]]=0) do inc(i);
    if i<=t1 then circle(i);
  until i>t1;
  for j:=1 to t1-1 do write(chg[ph[j]],'-');
  write(chg[ph[t1]]);
  assign(f,'post.out');rewrite(f);
  writeln(f,tot);
  for j:=1 to t1 do write(f,chg[ph[j]],' ');
  close(f);
end;

begin
  init;
  quickadd;
  if odn1>2 then
    repeat
      found:=false;
      find;
    until not found;
  clrscr;
  onepen;
end.