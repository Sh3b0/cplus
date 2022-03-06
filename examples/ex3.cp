
routine max (a : integer, b : integer) : integer is
    if a > b then
        return a;
    end
    return b;
end

type tuple is array[3] integer;
type point is record { var x : integer; var y : integer; } end;

routine say_hi() is
    println "Hello!";
    return;
end

routine main() : integer is
    say_hi();

    var x is (1 + 1) * 2;
    println x;
    
    var p : point;
    p.x := 1;
    p.y := 2;

    println p.x; # inline comment
    println p.y; # inline comment

    var h is p.x + p.y;
    println h;

    var t : tuple;
    t[1] := 1;
    var res is t[1];
    println res;

    var mx is max(5, 4);
    println mx;

    return 0;
end