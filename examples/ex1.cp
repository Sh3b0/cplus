type int is integer;

routine main() : int is
    var tmp : int;
    tmp := 5;
    print tmp;       # 5

    print "Hello C+";

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
    end

    return 0;
end
