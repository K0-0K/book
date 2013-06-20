program Tournament_Seeding;
var
seed: array[0..127] of Integer;
  {seed[Index] = 竞赛树中位置为Index的选手的种子排名}
place: array[1..128] of Integer;
  {place[Index] = 种子编号Index的选手在竞赛树中的位置}
i, j, k, r, n, m, place1, place2, cur, min: Integer;
  {在有n位选手参加的锦标赛中，实力为m的竞赛最早在第min轮出现}
begin
  {预处理，建立具有128人的竞赛树}
seed[0]:= 1; {已知条件}
r := 64; k := 2; {r=2i-1为第i轮所有比赛的两个最佳选手的位置距离；k=28-i}
for i := 7 downto 1 do
begin
	j := 0; {从已知条件出发推导其它位置的种子编号}
	while j < 128 do
	begin
seed[j + r]:= k + 1 - seed[j]; {第i轮所有比赛的最佳实力都是28-i+1}
j := j + 2 * r;
	end;
	r := r div 2; k := k * 2;
end;
{建立竞赛树的反查表，得到任一种子编号的选手在竞赛树中的位置表}
for i := 0 to 127 do place[seed[i]]:= i;
Read(n, m); {读入数据}

r := 0; k := n - 1; {总共有r=?log2n?轮竞赛}
while k > 0 do
begin
k := k div 2; r:=r+1;
end;

min := 8; {初始化min}
for i := 1 to (m - 1) div 2 do {枚举两个种子编号i和m-i}
if (i <= n) and (m - i <= n) then {求两位选手在第几轮竞赛中相遇}
	begin
cur := 0; {从第一轮开始判断}
	  place1 := place[i]; {取出两位选手在竞赛树中的位置}
	  place2 := place[m - i];
	  repeat
        cur:=cur+1;
	    place1 := place1 div 2; {走到父结点的位置，相当选手晋级}
		place2 := place2 div 2;
	  until place1 = place2; {若位置相同，则在第cur轮相遇}
	  cur := cur - (7 - r);
  if (cur < min) then min := cur; {若cur比min更早，则更新min}
	end;
Writeln(min); {输出结果}
end.
