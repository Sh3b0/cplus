var a is 5;
var b : integer;
var c : integer is 2;

type int is integer;
type bool is boolean;


routine power(number : int, to_power : int) : int is
	var tmp is 1;
	
	if to_power < 1 then
		return 1;
	end

	for i in 0 .. to_power - 1 loop
		tmp := tmp * number;
	end
    
	return tmp;
end

routine main(input : integer) : int is 
	b := power(c, a);
	a := power(b, c);
	var d is power(c, 2) * power(c, 3);
	return d;
end

