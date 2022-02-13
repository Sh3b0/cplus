var x is (1 + 1) * 2;
print x;

type point is record { var x : integer; var y : integer; } end;
point.x := 1;
point.y := 2;

print point.x;

var h is point.x + point.y;
# var h is point.x + point.y + 1;
print h;

type tuple is array[3] integer;
tuple[1] := 1;
var res is tuple[1];
print res;


routine max (a : integer, b : integer) : integer is
    if a > b then
        return a;
    else
        return b;
end

var mx is max(5, 4);
print mx;
