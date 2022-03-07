var x : integer is 3;

routine convertToMiles(x : integer) : real is
    var convertFactor : real is 0.621371;
    return x * convertFactor;
end

routine main() is
	var miles : real is convertToMiles(x);
	print miles;
	return;
end
