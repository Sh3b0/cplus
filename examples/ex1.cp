type int is integer;

routine main() : int is
    println "Hello C+";
    var tmp is 5;

    print "If statement: ";
    var t is tmp + 1;
    if tmp = t - 1 then
        println "works";
    else
        println "does not work";
    end

    print "While loop: ";
    while tmp loop
        print tmp;
        print " ";
        tmp := tmp - 1;
    end

    println "";
    println "For loop: ";
    tmp := 0;
    for i in 0 .. 4 loop
        tmp := tmp + i;
        print i;
        print " ";
        print tmp;
        println "";
    end

    println "";

    var x is 5 + 5 * 3;
    var y is 5.0 - 5.0 * 4.0;
    var z is true and false;

    println "Expressions:";
    print "x = "; println x;
    print "y = "; println y;
    print "z = "; println z;
    print "5 / 2 = "; println 5 / 2;
    print "5.0 / 2 = "; println 5.0 / 2;

    println "";
    println "Casting:";
    x := y;  
    println x;          # -15
    x := true;
    println x;          # 1
    
    y := 1;
    println y;          # 1.0
    y := false;
    println y;          # 0.0

    z := 5;
    println z;          # 1 (true)
    z := 0;
    println z;          # 0 (false)
    
    println "Arrays and records: ";

    var overkill : record {
        var f1 : integer;
        var f2 : record {
            var f3 : integer;
            var f4 : array[3] real; 
        } end;
    } end;

    overkill.f1 := 1;
    overkill.f2.f3 := 2;
    overkill.f2.f4[1] := 3.5;

    println overkill.f1 + overkill.f2.f3 + overkill.f2.f4[1] + 1;  # 7.5

    return 0;
end
