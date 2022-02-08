a.b := 1;

var x is 1;
var y is 1.1;
var z is true;
var iv : integer;
var rv : real;
var bv : boolean;
type int is integer;
type bool is boolean;
type double is real;
var id : integer is 5;

type Point2D is record { var x : integer; var y : integer; } end;
type Tuple3D is array[3] integer;
var numbers : array[3] integer;

routine test (a : integer, b : integer) : integer is
    var result is 1;
    
    if 1 then 
        a := 1;
        while 1 loop
            a := 1;
        end
        for i in 0 .. 4 loop
            a := 1;
        end
    end

    for i in reverse 0 .. 4 loop
        a := 1;
    end
    
    print x;

    return result;    
end

var id : integer is 5 + 5;
var id is 5 + 5;
