routine max (a : integer, b : integer) : integer is
    var ret : integer;
    if a > b then
        ret := a;
    else
        ret := b;
    end
    return ret;
end

routine say_hi() is
    println "Hello!";
    return;
end

type tuple is array[3] integer;
type point is record { var x : integer; var y : integer; } end;

routine main() : integer is
    say_hi();

    var x is (1 + 1) * 2;
    println x;
    
    var p : point;
    p.x := 1;
    p.y := 2;

    println p.x;
    println p.y;

    var t : tuple;
    t[1] := 1;
    println t[1];

    var mx is max(5, 4);
    println mx;

    return 0;
end
