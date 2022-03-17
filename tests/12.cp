type int is integer;

routine calculate(a : int, b : int, c : int) : integer is
	return ((a + (b + (c - (a * b)))) + b) * (a + (a * 2 + (c - a)) - b);
end


var a is 3;


routine f1(x: integer) : real is
	return x * 1.0;
end

routine f2(x: integer) : real is
	return f1(x);
end

routine f3(x: integer) : real is 
	return f2(x);
end

routine f4(x: integer) : real is 
	return f3(x);
end

routine f5(x: integer) : real is 
	return f4(x);
end

routine f6(x: integer) : real is 
	return f5(x);
end

routine f7(x: integer) : real is 
	return f6(x);
end

routine main() : real is
	var res is calculate(1, 2, a);
	println f7(res);
	return 0;
end

