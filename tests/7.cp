routine leapYear(year : integer) : boolean is
    if year % 4 = 0 then
        if year % 100 = 0 then
            if year % 400 = 0 then
                return true;
            else
                return false;
            end
        else
            return true;
        end
    end
    return false;
end

routine main() is
    var year : integer is 2000;
    var isLeap : boolean is leapYear(year);
    println isLeap;
    return;
end

