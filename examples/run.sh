for i in {1..11}
do
    echo -e "\nTest ${i}\n================\n"
	./cplus ${i}.cp
    ./a.out > out.txt
    diff out.txt ${i}.ans
done

rm a.out ir.ll out.txt
