var x is 5;
type arr is array[3] integer;
type rec is record { var x: integer; } end;
routine max (a : integer, b : integer) : integer is
    if 1 > 2 then
        return 1;
    else
        return 2;
    end
end

# test 1: a is not a user type
# a.y := 1;

# test 2: x is not a user type
# x.y := 4;

# test 3: arr is not a record
# arr.x := 1;

# test 4: y is not a member of rec
# rec.y := 1;

# test 5: rec is not an array
# rec[1] := 1;

# test 6: array index out of bounds
# arr[5] := 1; 

# test 7: arr is not a declared variable
# arr := 3;

# test 8: x is not callable
# x(1, 2);

# test 9: arity mismatch
max(1, 2, 3);