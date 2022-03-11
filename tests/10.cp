routine getNumberOfFactors(x: integer) : integer is
    var k is 0;

    for i in 1 .. x loop
        if x % i = 0 then
           k := k + 1;
	    end
    end

   return k;
end

routine main() is
	var factorsNumber : integer is getNumberOfFactors(10);
	println factorsNumber;
	return;
end
