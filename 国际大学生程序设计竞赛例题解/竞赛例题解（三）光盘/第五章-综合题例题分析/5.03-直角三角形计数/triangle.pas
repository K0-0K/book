program triangle;
uses
  SysUtils;
const
  max=100;
  fx:array[0..7,1..2] of shortint=((-1,0),(-1,1),(0,1),(1,1),(1,0),(1,-1),(0,-1),(-1,-1));    //8个方向
var
  mat:array[1..max,1..max] of char;
  count:array['A'..'Z'] of longint;			//统计每个字母所构成的等腰直角三角形的个数
  appear:array['A'..'Z'] of boolean;			//记录字母是否出现过
  total:longint;
  n:integer;
  f1,f2:text;

procedure readin;
var
  i,j:integer;
begin
  fillchar(mat,sizeof(mat),0);
  fillchar(count,sizeof(count),0);
  fillchar(appear,sizeof(appear),0);
  reset(f1);
  readln(f1,n);
  for i:=1 to n do
  begin
    for j:=1 to n do read(f1,mat[i,j]);
    readln(f1);
  end;
  close(f1);
end;

function btwn(a,b,c:integer):boolean;
begin
  btwn:=(a<=b) and (b<=c);
end;

function make(x,y,fx1,fx2:integer):integer;	//统计以(x,y)为顶点，直角边沿fx1,fx2
//方向有多少个三角形
var
  x0,y0,x1,y1,x2,y2,dx,dy:integer;
  m:integer;
  c:char;
begin
  m:=0;
  c:=mat[x,y];
  x1:=x+fx[fx1,1];
  y1:=y+fx[fx1,2];
  x2:=x+fx[fx2,1];
  y2:=y+fx[fx2,2];
  while btwn(1,x1,n) and btwn(1,y1,n) and btwn(1,x2,n) and btwn(1,y2,n) do
  begin
    if x2=x1 then dx:=0 else dx:=(x2-x1) div abs(x2-x1);
    if y2=y1 then dy:=0 else dy:=(y2-y1) div abs(y2-y1);
    x0:=x1;
    y0:=y1;
    while ((x0<>x2) or (y0<>y2)) and (mat[x0,y0]=c) do
    begin
      x0:=x0+dx;
      y0:=y0+dy;
    end;
    if mat[x0,y0]<>c then
      break
    else
      inc(m);
    x1:=x1+fx[fx1,1];
    y1:=y1+fx[fx1,2];
    x2:=x2+fx[fx2,1];
    y2:=y2+fx[fx2,2];
  end;
  make:=m;
end;

procedure main;
var
  i,j,k:integer;
begin
  for i:=1 to n do
    for j:=1 to n do
      for k:=0 to 7 do
      begin
        appear[mat[i,j]]:=true;
        inc(count[mat[i,j]],make(i,j,k,(k+2) mod 8));
      end;
end;

procedure output;
var
  c:char;
begin
  total:=0;
  for c:='A' to 'Z' do
    total:=total+count[c];
  rewrite(f2);
  writeln(f2,total);
  for c:='A' to 'Z' do
    if appear[c] then
      writeln(f2,c,' ',count[c]);
  writeln(f2);
  close(f2);
end;

begin
  assign(f1,'triangle.dat');
  assign(f2,'triangle.out');
  readin;
  main;
  output;
end.
