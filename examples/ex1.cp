routine main() : integer is
    var x : integer is 5 + 5 * 3;
    var y : real is 5.0 - 5.0 * 4.0;
    var z : boolean is true and false;
    
    print x;          # 20
    print y;          # -15.0
    print z;          # 0
    print 5 / 2;      # 2
    print 5.0 / 2.0;  # 2.5

    if x = y then
        print 1;
    else
        print 2;
    end

    return 0;
end