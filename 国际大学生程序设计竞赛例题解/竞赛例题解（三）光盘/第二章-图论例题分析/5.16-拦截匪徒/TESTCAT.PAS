const
     inf1   =   'catch.out';
     inf2   =   'catch.std';
     outf   =   'result.txt';
var
   t1,t2        :text;
   c            :char;
   s1,s2        :string;

begin
  assign(t1,inf1);
  reset(t1);
  assign(t2,inf2);
  reset(t2);
  assign(output,outf);
  append(output);
  while not eof(t1) and not eof(t2) do
   begin
     c:=' ';
     while (((c>'9') or (c<'0')) and (c<>'N') and (c<>'o')) and not eof(t1) do read(t1,c);
     s1:='';
     while ((c<='9') and (c>='0') or (c='o') or (c='N')) and not eof(t1) do
      begin
        s1:=s1+c;
        read(t1,c);
      end;
     c:=' ';
     while (((c>'9') or (c<'0')) and (c<>'N') and (c<>'o')) and not eof(t2) do
      read(t2,c);
     s2:='';
     while ((c<='9') and (c>='0') or (c='o') or (c='N')) and not eof(t2) do
      begin
        s2:=s2+c;
        read(t2,c);
      end;
     if s1<>s2 then begin writeln('No');close(output);exit;end;
   end;
  while not eof(t1) do
   begin
     readln(t1,s1);
     if s1<>'' then begin writeln('No');close(output);exit;end;
   end;
  while not eof(t2) do
   begin
     readln(t2,s1);
     if s1<>'' then begin writeln('No');close(output);exit;end;
   end;
  writeln('Yes');
  close(t1);
  close(t2);
end.