var number : integer;
var originalNumber : integer;
var isArmstrong is false;
var remainder : integer;
var result is 0;

routine findArmstrongNumber(original : integer, num : integer, rem : integer, res : integer) : boolean is
    
    while num /= 0 loop
        rem := num % 10;
        res := res + rem * rem * rem;
        num := num / 10;
    end

    if res = original then
        return true;
    end
    
    return false;

end


routine main() is
	number := 153;
	originalNumber := number;
	isArmstrong := findArmstrongNumber(number, originalNumber, remainder, result);
	if isArmstrong then
	    println "Yep";
	else
	    println "Nope";
	end
	return;
end

