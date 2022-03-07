var number : integer;

var reversedNumber is 0;

var remainder : integer;

routine main(inp : integer) is
	number := 1828329;

	while number /= 0 loop
		remainder := number % 10;
		reversedNumber := reversedNumber * 10 + remainder;
		number := number / 10;
	    println number;
	end
	
	return;
end
