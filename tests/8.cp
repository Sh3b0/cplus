var x : integer is 0;
var y : real is 3.4;

routine isFive (x : integer) : boolean is
    return x = 5;
end

type bool is boolean;

routine isSix (z : integer) : bool is
    if z = 6 then
        return true;
    end
    return false;
end


routine main() is 
    if x then
        x := 5.3;
    else
        if y > 5 then
            x := true;
        end
    end

    if x then
        x := 5.3;
    else
        y := 10;
    end

    var z is 5 + x * y / (4 = 4);

    var a : array[10] bool;
    a[10] := 0;
    a[9] := true;
    a[8] := 9.8 /= 50;
    
    println x;
    println y;
    println z;
    println isSix(x);
    println isFive(x);
    println a[8];
    
    return;
end

