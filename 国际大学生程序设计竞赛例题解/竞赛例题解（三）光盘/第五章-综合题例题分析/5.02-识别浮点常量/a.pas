Program Identifying_Real_Constant;

var
	LastChar: Char; {一个字符的缓冲区}
	Buffered: Boolean; {缓冲区标志}

function getchar: Char; {从输入流读字符}
begin
	if not Buffered then Read(LastChar);  {如果缓冲区没有字符则读入一个}
	Buffered := False;
	GetChar := LastChar;             {取缓冲区字符}
end;

procedure ungetc(c: char); {向输入流回放字符}
begin
	LastChar := c;
	Buffered := True;
end;

function ReadThrough(match: string): Integer;
var                  {将输入流与字符集合match进行匹配，返回匹配的字符个数}
	c: Char;         {c是当前输入流的字符}
	num: Integer;    {匹配的字符个数}
begin
	num := 0;
	while true do
	begin
		c := getchar; {从输入流读字符c}
		if Pos(c, match) = 0 then {匹配结束}
		begin
			ungetc(c);   {c没有匹配，重新放入缓冲区}
			break;
		end;
		Inc(num); {匹配成功，匹配字符数加一}
	end;
	ReadThrough := num;
end;

function Valid: Boolean; {识别输入流是否浮点常量}
var
	hasdec, hasexp: Boolean;  {hasdec表示是否有小数, hasexp表示是否有指数}
	n: Integer;
begin
	Valid := False; Buffered := False;
	hasdec := False; hasexp := False; {标志初始化}

	ReadThrough(' '); {跳过输入流的前导空格}
	if ReadThrough('-+') > 1 then Exit; {多个正负号}
	if ReadThrough('0123456789') = 0 then Exit; {浮点常量缺无符号整数}
	n := ReadThrough('.');
	if n > 1 then Exit; {多个小数点}
	if n = 1 then {字符串中存在小数部分}
	begin
		 hasdec := True;   {置小数标记}
		 if ReadThrough('0123456789') = 0 then Exit; {小数部分缺无符号整数}
	end;
	n := ReadThrough('eE');
	if n > 1 then Exit; {多个e或E}
	if n = 1 then {字符串中存在指数部分}
	begin
		 hasexp := True; {置指数标记}
		 if ReadThrough('+-') > 1 then Exit; {多个正负号}
		 if ReadThrough('0123456789') = 0 then Exit; {指数部分缺无符号整数}
	end;
	ReadThrough(' '); {跳过输入流的后续空格}
	if Pos(getchar, #13#10) = 0 then Exit; {输入流不是回车}
	if (not hasexp) and (not hasdec) then Exit; {既无小数部分又无指数部分}

	Valid := True;   {没有找到错误，常量表示合法}
end;

const
	 res: array[Boolean] of string[3] = ('NO', 'YES');
begin
	 writeln(res[Valid]); {根据Valid函数的值输出YES或NO}
end.
