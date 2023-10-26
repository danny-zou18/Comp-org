.data
	newline: .asciiz "\n"
	
.text 
.globl main

main:
	li $a0, 5
	li $a1, 3
	
	jal mul
	
	move $a0, $v0
	li $v0, 1
	syscall
	
	li $v0, 4
	la $a0, newline
	syscall
	
	li $v0, 10
	syscall
	
mul: 
	addi $sp, $sp, -8
	
	sw $s0, 0($sp)
	
	sw $t0, 4($sp)
	
	move $s0, $zero
	move $t0, $a0
	
multiply_loop:
	beq $a1, $zero, done
	
	add $s0, $s0, $t0
	
	addi $a1, $a1, -1
	
	j multiply_loop
	
done:
	move $v0, $s0
	
	lw $s0, 0($sp)
	lw $t0, 4($sp)
	addi $sp, $sp, 8
	
	jr $ra	
