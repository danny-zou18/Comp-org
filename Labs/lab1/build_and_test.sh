CLEAN=false
if [[ "$1" == "-c" ]]; then
    CLEAN=true
fi

if [ "$CLEAN" = true ]; then
    make clean
fi

make

input_files=("test1.mat" "test2.mat" "test3.mat" "test4.mat" "test5.mat"
             "test6.mat" "test7.mat" "test8.mat" "test9.mat" "test10.mat")

for file in "${input_files[@]}"; do
    echo "Running test with input file: $file"
    ./main <<< "$file"
done

echo "Tests completed."
