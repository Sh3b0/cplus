routine main() : integer is
    var x is 5.0 + 5;
    var y is 5.0 - 5.0 * 4.0;
    var z is not (true or false) xor true;

    print x;
    print y;
    print z;

    print 5 / 2;      # 2
    print 5.0 / 2.0;  # 2.5

    var c1 is 4 > 4.0;  # false
    var c2 is 4 >= 4.0; # true
    var c3 is 4.0 > 4;  # false 
    
    print c1;
    print c2;
    print c3;

    return 0;
end
