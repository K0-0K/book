{*
 * Judging program for "Random Number"
 *
 * Copyright Chen Mingrei (ZSUCPC 2001)
 * Date: May 10, 2001
 *
 * Comment: This program displays and checks the output of a program to see
 *          if it correct.  The following are checked:
 *
 *          1. no output
 *          2. extraneous output
 *          3. format error (invalid number)
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
	Write('Judging program for "Random Number"         Copyright Chen Mingrei (ZSUCPC 2001)');
	Writeln('Input: random.in                       � Output: random.out');
	Write('컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컵컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴');
	InStr := '';
	if NoOutput then OutStr := 'Fail to open "random.out"!'
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

procedure Judge;
var
	YourAns, StdAns: Integer;
	I, M, N: Integer;
begin
	Reset(InFile); Reset(StdFile);

	If NoOutput then Report('No output file');

	Reset(OutFile);
	if Eof(OutFile) then Report('No output');

	Readln(InFile, M, N);
	for I := 1 to N do
	begin
		Read(StdFIle, StdAns);
		{$I-}
		Read(OutFile, YourAns);
		{$I+}
		if IOResult <> 0 then Report('Format error: invalid number');
		if StdAns <> YourAns then Report('Wrong answer.');
	end;

	Readln(OutFile);
	if not Eof(OutFile) then Report('Extraneous output');

	Report('');
end;

begin
	Assign(InFile, 'random.in');
	Assign(OutFile, 'random.out');
	Assign(StdFile, 'random.std');

	Show;
	Judge;
end.