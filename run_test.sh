python3 list_test/gen_cmake.py

for i in $(seq 1 5);
do 
    capacity=$(($RANDOM % 1000))
    insert_num=$(($RANDOM % 5000))
    delete_num=$(($RANDOM % $insert_num))

    echo gen_test.py $i.cpp $capacity $insert_num $delete_num
    python3 list_test/gen_test.py $i.cpp $capacity $insert_num $delete_num

done

sleep 5

cd build/
cmake ../ -DCMAKE_BUILD_TYPE=Debug
cmake --build ./

filename="../list_test/run_test.txt"

echo
echo RUNNING TESTS
echo

while read line;
do
    if [ "$line" != "\n" ]
    then
        #memory leaks and adress sanitizers
        leaks --list --atExit -- $line -faddress-sanitizer
    fi
done < $filename


#clear all test files
cd ../list_test
rm *.cpp
rm *.txt
cd ../
rm CMakeLists.txt