.data
	matrix: .word 1, 2, 3, 4 # Initialize matrix


.text
    # Main code section
    main:
        # Example values to multiply
        la $t0, matrix       # Load in matrix
	
        # Call the mul function
        jal det

        # Print the result
        move $a0, $v0   # Copy the result to $a0 for printing
        li $v0, 1       # Load system call code for print integer
        syscall

        # Print a newline
        li $v0, 11     
        li $a0, 10     
        syscall

        # Exit the program
        li $v0, 10     
        syscall

    # Function to get the determinant of a matrix
    det:
        # Save $ra (return address) on the stack
        subi $sp, $sp, 4
        sw $ra, ($sp)
        
        # Load the matrix elements into registers
    	lw $t1, ($t0)    # Load matrix element a
    	
    	lw $t2, 4($t0)    # Load matrix element b

    	lw $t3, 8($t0)    # Load matrix element c

    	lw $t4, 12($t0)    # Load matrix element d
    	
    	move $a0, $t1
    	move $a1, $t4

    	# Calculate the determinant (ad - bc)
    	jal mul          # Call the mul function to calculate ad and store the result in $v0
    	
    	move $v1, $v0
   	move $a0, $t2    # Move b to $a0
    	move $a1, $t3    # Move c to $a1
    	jal mul         # Call the mul function to calculate bc and store the result in $v0

    	# Subtract bc from ad (result in $v0)
    	sub $v0, $v1, $v0

    	# Restore $ra from the stack and return
        lw $ra, ($sp)
        addi $sp, $sp, 4
        jr $ra

    	
	

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
