.data
    arg1_max: .word 10
    arg1_min: .word -10
    arg2_max: .word 7
    arg2_min: .word -7
	

.text
main:
    # Load argument limits from memory
    lw $t0, arg1_min
    lw $t1, arg1_max
    lw $t2, arg2_min
    lw $t3, arg2_max

    # Iterate over all possible combinations
    move $t4, $t1  # Initialize a counter for arg1
loop_arg1:
    beq $t4, $t0, doney  # Exit loop when arg1 reaches 0

    move $t5, $t3  # Initialize a counter for arg2
loop_arg2:
    beq $t5, $t2, next_arg1  # Exit inner loop when arg2 reaches 0

    # Compute the arguments for the current combination
    move $a0, $t4
    move $a1, $t5

    # Call the mul function
    jal mul

    # Calculate the expected product
    mul $t6, $t4, $t5

    # Print the arguments, expected product, and actual product
    move $a0, $t4
    li $v0, 1
    syscall
    
    li $v0, 11  # Print a space
    li $a0, 32
    syscall
    
    move $a0, $t5
    li $v0, 1
    syscall
    
    li $v0, 11  # Print a space
    li $a0, 32
    syscall
    
    move $a0, $t6  # Expected product
    li $v0, 1
    syscall
    li $v0, 11  # Print a space
    li $a0, 32
    syscall
    
    move $a0, $t4
    move $a1, $t5

    # Call the mul function
    jal mul
    
    move $a0, $v0  # Actual product
    li $v0, 1
    syscall

    # Compare the actual product with the expected product
    beq $a0, $t6, no_error
    li $v0, 11  # Print a space
    li $a0, 32
    syscall
    li $v0, 69  # Print "Error!"
    syscall
    li $v0, 11     # Load system call code for print character
    li $a0, 10     # Load ASCII code for newline
    syscall
no_error:

    # Increment the arg2 counter and repeat the loop
    subi $t5, $t5, 1
    li $v0, 11     # Load system call code for print character
    li $a0, 10     # Load ASCII code for newline
    syscall
    j loop_arg2
    
    

next_arg1:
    # Increment the arg1 counter and repeat the loop
    subi $t4, $t4, 1
    j loop_arg1

doney:
    # Exit the program
    li $v0, 10
    syscall
    
# Function to perform multiplication by addition
    mul:
        # Save $ra (return address) on the stack
        subi $sp, $sp, 4
        sw $ra, ($sp)

        # Initialize $s0 to 0 (accumulator)
        move $s0, $zero
	
	bgez $a1, not_negative
	neg $a1, $a1
	neg $a0, $a0
	j end
	not_negative:
	# Value in $t0 is non-negative, so do nothing
	end:
	
        # Loop to perform multiplication by addition
        multiply_loop:
            # Check if $a1 is zero, if so, exit the loop
            beqz $a1, done

            # Add $a0 to $s0
            add $s0, $s0, $a0

            # Decrement $a1
            subi $a1, $a1, 1

            # Repeat the loop
            j multiply_loop

        done:
            # Store the result in $v0
            move $v0, $s0

            # Restore $ra from the stack and return
            lw $ra, ($sp)
            addi $sp, $sp, 4
            jr $ra