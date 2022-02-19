var x is (1 + 1) * 2;
print x;

type point is record { var x : integer; var y : integer; } end;
point.x := 1;
point.y := 2;

print point.x;

# var h is point.x + point.y;
var h is point.x + point.y + 1;
print h;

type tuple is array[3] integer;
tuple[1] := 1;
var res is tuple[1];
print res;

# Note: parser crashes with Seg fault
# as it tries to get the value for a and b
# which are not assigned yet.
# after fixing this issue replace 1 with a and 2 with b

routine max (a : integer, b : integer) : integer is
    if 1 > 2 then
        return 1;
    else
        return 2;
    end
end

# var mx is max(5, 4);
# print mx;
