routine main() : integer is
    var x is 5 + 5 * 3;
    var y is 5.0 - 5.0 * 4.0;
    var z is true and false;

    print x;          # 20
    print y;          # -15.0
    print z;          # 0
    print 5 / 2;      # 2
    print 5.0 / 2.0;  # 2.5

    x := y;  
    print x;          # -15
    x := true;
    print x;          # 1
    
    y := 1;
    print y;          # 1.0
    y := false;
    print y;          # 0.0

    z := 5;
    print z;          # 1 (true)
    z := 0;
    print z;          # 0 (false)
   
    var t is x + 1;

    if x = t - 1 then
        print 1;
    else
        print 2;
    end

    var overkill : record {
        var f1 : integer;
        var f2 : record {
            var f3 : integer;
            var f4 : array[3] real; 
        } end
        ;
    } end
    ;

    overkill.f1 := 1;
    overkill.f2.f3 := 2;
    overkill.f2.f4[1] := 3.5;

    print overkill.f1 + overkill.f2.f3 + overkill.f2.f4[1] + 1;

    return 0;
end
