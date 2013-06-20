var
  f1,f2:text;
  s:string;
  n:integer;

function test(s:string):integer;
var
  x,y,i,j,k,h:integer;
  s1,s2:string;
begin
  if s='' then
    begin
      test:=0;
      exit;
    end;

  if s[1] in['A'..'Z','a'..'z'] then
    begin
      if length(s)=1 then test:=1 else test:=0;
    end else if s[1]='(' then
    begin
      if s[length(s)]=')' then
        begin
          h:=0; k:=0;
          for i:=2 to length(s)-1 do
            begin
              if s[i]='(' then inc(h)
                else if s[i]=')' then dec(h);
              if (h=0)and(s[i]=',')then
                begin
                  k:=i;
                  break;
                end;
            end;
          if k=0 then test:=0 else
            begin
              s1:=copy(s,2,k-2);
              s2:=copy(s,k+1,length(s)-k-1);
              x:=test(s1);
              y:=test(s2);
              if (x=1)and(y=1)then test:=1 else test:=0;
            end;
        end else test:=0;
    end else test:=0;
end;

begin
  assign(f1,'sexp.in');
  assign(f2,'sexp.std');
  reset(f1);rewrite(f2);

  readln(f1,n);
  writeln(f2,n);
  while n>0 do
    begin
      dec(n);
      readln(f1,s);
      while (s<>'')and(s[1]=' ')do delete(s,1,1);
      while (s<>'')and(s[length(s)]=' ') do delete(s,length(s),1);
      if test(s)=1 then writeln(f2,'Yes')
        else writeln(f2,'No');
    end;

  close(f1);close(f2);
end.
