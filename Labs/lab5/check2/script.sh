#!/bin/bash
declare -a input_arr=("input1.txt" "input2.txt")
declare -a output_arr=("output1.txt" "output2.txt")
make -f make.mak clean
if [ $? -eq 0 ]
then
    echo "Successfully cleaned the project."
else
    echo "Cleaning the project failed. Terminating the script." >&2
    exit 1
fi
make -f make.mak all
if [ $? -eq 0 ]
then
    echo "Successfully built the project."
else
    echo "Building the project failed. Terminating the script." >&2
    exit 1
fi

echo "******************************************************************************"

## now loop through the array of inputs

for input_idx in "${!input_arr[@]}"
do
    if [ "$1" = "valgrind" ]
    then
        echo valgrind --leak-check=full --show-leak-kinds=all .\mm \< "${input_arr[$input_idx]}" 2\>\&1 \| tee "${output_arr[$input_idx]}"
        valgrind --leak-check=full --show-leak-kinds=all ./mm < "${input_arr[$input_idx]}" 2>&1 | tee "${output_arr[$input_idx]}"
    else
        echo .\mm \< "${input_arr[$input_idx]}" 2\>\&1 \| tee "${output_arr[$input_idx]}"
        ./mm < "${input_arr[$input_idx]}" 2>&1 | tee "${output_arr[$input_idx]}"
    fi
    if [ $? -eq 0 ];
    then
        echo "Successfully ran the project with input ${input_arr[$input_idx]} and captured output in ${output_arr[$input_idx]}."
    elif [ $? -eq 137 ];
    then
        echo "Running the project with input ${input_arr[$input_idx]} failed due to out of memory condition." >&2
    else
        echo "Running the project with input ${input_arr[$input_idx]} failed." >&2
    fi

    echo "******************************************************************************"
done