#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef int bool;
#define true 1
#define false 0

//Struct to link a variable to a saved register
struct registerMapping {
    char variableLetter;
    char mipsRegister[4];
    int value;
};
//Function to save a variable to a saved register based on how many registers there currently are(index)
void saveRegister(struct registerMapping* registers, int index, char letter, int value){
    registers[index].variableLetter = letter;
    char registerString[4]; // Assuming a maximum register name size of 4 characters
    sprintf(registerString, "$s%d", index);
    strcpy(registers[index].mipsRegister, registerString);
    registers[index].value = value;
}
//Function that finds a register given the variable linked to the said register, then returns it as a string
char* findRegister(struct registerMapping* registers, int size, char variable){
    for (int i = 0; i < size; i++){
        if (registers[i].variableLetter == variable){
            return registers[i].mipsRegister;
        }
    }
    return "";
}
//Function that takes a given constant and then finds the exponents of 2 needed to make up the 
//constant using its binary representation. 
void findExponents(int constant, int exponents[], int* numExponents) {
    int exponent = 0; 
    *numExponents = 0; //Initialize number of exponents
    while (constant > 0) { //Loop until the binary representation is 0
        //If the least significant bit is 1, signal us to store
        //the current exponent in exponents array
        if (constant & 1) {
            exponents[(*numExponents)++] = exponent;
        }
        constant >>= 1; //Shift right in binary one spot to examine the next bit
        exponent++; //Increment the exponent to represent the next higher bit.
    }
}
//Function that checks if the second operand is a number
int isNumeric(const char* str) {
    while (*str) {
        if (!isdigit(*str) && *str != '-') {
            return 0; // Not a number
        }
        str++;
    }
    return 1; // It's a number
}
//Function that checks if a number is a power of two using its binary representation
bool isPowerOfTwo(int number) {
    if (number <= 0) {
        return false; // Numbers less than or equal to 0 are not powers of 2
    }
    // Check if there is only one set bit in the binary representation
    return (number & (number - 1)) == 0;
}
//Function that returns the exponent of 2 of a number using its binary representation
int powerOfTwoExponent(int number) {
    int exponent = 0;
    // Keep shifting to the right until we find the first set bit.
    while ((number & 1) == 0) {
        number >>= 1; //Shift right in binary one spot to examine the next bit
        exponent++; // Increment the exponent to represent the next higher bit.
    }
    return exponent; // Return the exponent found.
}
int main(int argc, char *argv[]){
    //ERROR CHECKING
    if (argc < 2) {
        printf("Missing command line argument\n");
        return 1;
    }
    char filename[128]; //Initialize string to hold filename
    strncpy(filename, argv[1], sizeof(filename)); //Get the file name

    //Open file to read
    FILE *file = fopen(filename, "r");
    //ERROR CHECKING
    if (file == NULL){
        printf("Failed to open file: %s", filename);
        return 1;
    }
    struct registerMapping* registers = NULL; //Initialize array to store our variable-registers structs
    int size = 0; //Initialize size of the array to track what register to use next
    int capacity = 100;
    //Initialize memory needed for the register array to hold the structs
    registers = (struct registerMapping*)malloc(capacity * sizeof(struct registerMapping));

    char buffer[128]; //Buffer to store each line from file, at most 128 characters
    bool needTempReg = false; //Var used to track if a temporary register is used
    int tempRegisters = 0; //Track number of temporary registers used
    int labels = 0; //Track needed labels
    while (fgets(buffer, sizeof(buffer), file) != NULL) { //Loop until there are no more lines
        printf("# %s", buffer); //Print the commented line
        if (buffer[strlen(buffer)-1]!='\n'){
            printf("\n");
        }
        //Parse the line using tokens
        char* token = strtok(buffer, " \t\n\r"); //Point to the first parsed string
        char* parts[128]; //Initialize array to store each part of the parsed stirng
        int partCount = 0; 
        while (token != NULL) { //Loop until we have finished parsing
            if (token[strlen(token)-1] == ';'){ //If it is the last part, remove the semicolon
                token[strlen(token) - 1] = '\0';
            }
            parts[partCount] = token; //store the part
            partCount++;
            token = strtok(NULL, " \t\n\r"); //move onto next part
        }
        //If there are only 3 parts, we know that we are only assigning
        if (partCount == 3) {
            saveRegister(registers, size, *parts[0], atoi(parts[2]));
            printf("li %s,%d\n", registers[size].mipsRegister, registers[size].value);
            size++;
        } else if (partCount >= 5) {
            saveRegister(registers,size,*parts[0],1);
            size++;
            int instructions = partCount / 2 - 1; //Calculate number of operators
            for (int i = 1; i <= instructions; i++){
                //Calculate index of each operand and operator
                int firstOperandIndex = 2 * i;
                int secondOperandIndex = firstOperandIndex + 2;
                int operatorIndex = firstOperandIndex + 1;
                char firstOperandVar = *parts[firstOperandIndex];
                char secondOperandVar = *parts[secondOperandIndex];
                char operator = *parts[operatorIndex];
                //If the second operand is a variable that has not yet been linked to a register, save it
                if (strcmp(findRegister(registers, size, secondOperandVar), "") == 0 && !isNumeric(parts[secondOperandIndex])){
                    saveRegister(registers,size,secondOperandVar,1);
                    size++;
                }
                //If the operator is add
                if (operator == '+'){
                    //Check if second operand is a number
                    if (isNumeric(parts[secondOperandIndex])){
                        //Check if it is the last instruction
                        if (instructions == i){
                            //If it is, check if a temporary register was used, then print
                            if (needTempReg == true){
                                printf("addi %s,$t%d,%s\n", findRegister(registers, size, *parts[0]),tempRegisters-1,parts[secondOperandIndex]);
                            } else {
                                printf("addi %s,%s,%s\n", findRegister(registers, size, *parts[0]), findRegister(registers, size, firstOperandVar), parts[secondOperandIndex]);
                            }
                        } else {
                            //If not the last instruction, check if any temporary registers have been used
                            if (tempRegisters == 0){
                                //If none has been used
                                printf("addi $t0,%s,%s\n", findRegister(registers, size, firstOperandVar), parts[secondOperandIndex]);
                                needTempReg = true;
                                tempRegisters++;
                            } else {
                                //If has been used
                                printf("addi $t%d,$t%d,%s\n", tempRegisters, tempRegisters - 1, parts[secondOperandIndex]);
                                tempRegisters++;
                            }
                        } 
                    } else {
                        //If not a number and is another variable, do the same thing above
                        if (instructions == i){
                            if (needTempReg == true){
                                printf("add %s,$t%d,%s\n", findRegister(registers, size, *parts[0]),tempRegisters-1 ,findRegister(registers, size, secondOperandVar));
                            } else {
                                printf("add %s,%s,%s\n", findRegister(registers, size, *parts[0]), findRegister(registers, size, firstOperandVar),  findRegister(registers, size, secondOperandVar));
                            }
                        } else {
                            if (tempRegisters == 0){
                                printf("add $t0,%s,%s\n", findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
                                needTempReg = true;
                                tempRegisters++;
                            } else {
                                printf("add $t%d,$t%d,%s\n",tempRegisters, tempRegisters - 1,findRegister(registers, size, secondOperandVar));
                                tempRegisters++;
                            }
                            
                        }
                    }
                    //If the operator is subtract
                } else if (operator == '-'){
                     //Check if second operand is a number
                    if (isNumeric(parts[secondOperandIndex])){
                        //Check if it is the last instruction
                        if (instructions == i){
                            //If it is, check if a temporary register was used, then print
                            if (needTempReg == true){
                                printf("addi %s,$t%d,-%s\n", findRegister(registers, size, *parts[0]),tempRegisters-1 ,parts[secondOperandIndex]);
                            } else {
                                printf("addi %s,%s,-%s\n", findRegister(registers,size, *parts[0]), findRegister(registers, size, firstOperandVar), parts[secondOperandIndex]);
                            }
                            //If not the last instruction, check if any temporary registers have been used
                        } else {
                            if (tempRegisters == 0){
                                //If none has been used
                                printf("addi $t0,%s,-%s\n", findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
                                needTempReg = true;
                                tempRegisters++;
                            } else {
                                //If has been used
                                printf("addi $t%d,$t%d,-%s\n", tempRegisters, tempRegisters-1, parts[secondOperandIndex]);
                                tempRegisters++;
                            }
                        }   
                    } else {
                        //If not a number and is another variable, do the same thing above
                        if (instructions == i){
                            if (needTempReg == true){
                                printf("sub %s,$t%d,%s\n", findRegister(registers, size, *parts[0]),tempRegisters-1 ,findRegister(registers, size, secondOperandVar));
                            } else {
                                printf("sub %s,%s,%s\n", findRegister(registers, size, *parts[0]), findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
                            }
                        } else {
                            if (tempRegisters == 0){
                                printf("sub $t0,%s,%s\n",findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
                                needTempReg = true;
                                tempRegisters++;
                            } else {
                                printf("sub $t%d,$t%d,%s\n",tempRegisters ,tempRegisters-1 ,findRegister(registers,size,secondOperandVar));
                                tempRegisters++;
                            }
                        }
                    }
                    //If the operator is multiplication
                } else if (operator == '*'){
                    //Check if the second operand is a number
                    if (isNumeric(parts[secondOperandIndex])){
                        //SPECIAL CASE : If the number is 0
                        if (strcmp(parts[secondOperandIndex], "0") == 0){
                            printf("li %s,0\n",findRegister(registers,size,*parts[0]));
                            //SPECIAL CASE : If the number is 1 or -1
                        } else if (strcmp(parts[secondOperandIndex], "1") == 0 ||strcmp(parts[secondOperandIndex], "-1") == 0){
                            printf("move $t%d,%s\n",tempRegisters,findRegister(registers,size,firstOperandVar));
                            if (strcmp(parts[secondOperandIndex], "1") == 0) {
                                printf("move %s,$t%d\n",findRegister(registers,size,*parts[0]),tempRegisters);
                            } else if (strcmp(parts[secondOperandIndex], "-1") == 0){
                                printf("sub %s,$zero,$t%d\n",findRegister(registers,size,*parts[0]), tempRegisters);
                            }
                            tempRegisters++;
                            needTempReg=true;
                        } else {
                            //For all other numbers
                            //Check if  negative
                            bool isNegative = false;
                            if (parts[secondOperandIndex][0] == '-'){ //If negative remove the - 
                                isNegative = true;
                                int len = strlen(parts[secondOperandIndex]);
                                for (int i = 0; i < len; i++) {
                                    parts[secondOperandIndex][i] = parts[secondOperandIndex][i + 1];
                                }
                            }
                            needTempReg = true;
                            int neededExponents[32]; //initialize exponents array
                            int constant = atoi(parts[secondOperandIndex]); //convert string number into a int
                            int numExponents; 
                            findExponents(constant, neededExponents, &numExponents); //Find the exponents of 2 needed
                            for (int i = 0; i < numExponents; i++){ //Loop through the exponents
                                if (i == 0){ //If first instance of the loop
                                    printf("sll $t%d,%s,%d\n",tempRegisters, findRegister(registers, size, firstOperandVar), neededExponents[numExponents-i-1]);
                                    printf("move $t%d,$t%d\n", tempRegisters+1, tempRegisters);
                                    continue;
                                }
                                //If the exponent is not 0, meaning the value is not 1
                                if (neededExponents[numExponents-i-1] != 0){
                                    printf("sll $t%d,%s,%d\n",tempRegisters, findRegister(registers, size, firstOperandVar), neededExponents[numExponents-i-1]);
                                    printf("add $t%d,$t%d,$t%d\n",tempRegisters+1, tempRegisters+1, tempRegisters);
                                } else {
                                    //If it is 0
                                    printf("add $t%d,$t%d,%s\n",tempRegisters+1, tempRegisters+1, findRegister(registers, size, firstOperandVar));
                                }
                            }
                            if (instructions == i){
                                //If the last instance of the loop
                                if (isNegative){
                                    printf("sub %s,$zero,$t%d\n",findRegister(registers, size, *parts[0]),tempRegisters+1);
                                } else {
                                    printf("move %s,$t%d\n",findRegister(registers, size, *parts[0]), tempRegisters+1);
                                }
                            } else {
                                tempRegisters += 2;
                                printf("move $t%d,$t%d\n", tempRegisters, tempRegisters-1);
                            }
                        }
                    } else {
                        //If the second operand is another variable
                        //Check if it is the last instruction
                        if (instructions == i){
                            //Check if a temporary register is used, then print
                            if (needTempReg == true){
                                printf("mult $t%d,%s\n",tempRegisters-1 ,findRegister(registers, size, secondOperandVar));
                                printf("mflo %s\n", findRegister(registers, size, *parts[0]));
                            } else {
                                printf("mult %s,%s\n", findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
                                printf("mflo %s\n", findRegister(registers, size, *parts[0]));
                            }
                        } else {
                            //If not the last instruction
                            //Check if a temporary register has been used
                            if (tempRegisters == 0){
                                //If it hasnt, print
                                printf("mult %s,%s\n", findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
                                printf("mflo $t0\n");
                                needTempReg = true;
                                tempRegisters++;
                            } else {
                                //If it has, check if the two operands are the same, then print
                                if (firstOperandVar == secondOperandVar){
                                    printf("mult %s,%s\n", findRegister(registers,size,firstOperandVar),findRegister(registers, size, secondOperandVar));
                                    printf("mflo $t%d\n", tempRegisters);
                                } else {
                                    printf("mult $t%d,%s\n", tempRegisters-1,findRegister(registers, size, secondOperandVar));
                                    printf("mflo $t%d\n", tempRegisters);
                                }
                                tempRegisters++;
                            }
                            
                        }
                    }
                    //If the operator is divide
                } else if (operator == '/'){
                    //Check if the second operand is a number
                    if (isNumeric(parts[secondOperandIndex])){
                        //SPECIAL CASES : If the number is 1 or -1
                        if (strcmp(parts[secondOperandIndex], "1") == 0 || strcmp(parts[secondOperandIndex], "-1") == 0){
                            if (strcmp(parts[secondOperandIndex], "1") == 0){
                                if (instructions == i){
                                    printf("move %s,%s\n",findRegister(registers,size,*parts[0]),findRegister(registers,size,firstOperandVar));
                                } else {
                                    printf("move $t%d,%s\n",tempRegisters, findRegister(registers,size,firstOperandVar));
                                    needTempReg = true;
                                    tempRegisters++;
                                }
                            } else if (strcmp(parts[secondOperandIndex], "-1") == 0){
                                if (instructions == i){
                                    printf("sub %s,$zero,%s\n", findRegister(registers,size,*parts[0]),findRegister(registers,size,firstOperandVar));
                                } else {
                                    printf("sub $t%d,$zero,%s\n",tempRegisters,findRegister(registers,size,firstOperandVar));
                                    needTempReg = true;
                                    tempRegisters++;
                                }
                            }
                            //FOR ALL OTHER NUMBERS
                        } else {
                            //Check if the number is negative
                            bool isNegative = false;
                            if (parts[secondOperandIndex][0] == '-'){ //If negative, remove the - from the number string
                                isNegative = true;
                                int len = strlen(parts[secondOperandIndex]);
                                for (int i = 0; i < len; i++) {
                                    parts[secondOperandIndex][i] = parts[secondOperandIndex][i + 1];
                                }
                            }
                            //Check if the number is a power of 2
                            if (isPowerOfTwo(atoi(parts[secondOperandIndex]))){
                                //Get the power of two
                                int powerOfTwo = powerOfTwoExponent(atoi(parts[secondOperandIndex]));
                                //Check if a temporary register has been used, then print bltz method
                                if (needTempReg){
                                    printf("bltz $t%d,L%d\n",tempRegisters - 1,labels);
                                    printf("srl %s,$t%d,%d\n",findRegister(registers,size,*parts[0]),tempRegisters -1, powerOfTwo);
                                    if (isNegative){
                                        printf("sub %s,$zero,%s\n", findRegister(registers,size,*parts[0]),findRegister(registers,size,*parts[0]));
                                    }
                                    printf("j L%d\n",labels+1);
                                    printf("L%d:\n",labels);
                                    if (isNegative){
                                        printf("li $t%d,-%s\n",tempRegisters,parts[secondOperandIndex]);
                                    } else {
                                        printf("li $t%d,%s\n",tempRegisters,parts[secondOperandIndex]);
                                    }
                                    printf("div $t%d,$t%d\n",tempRegisters - 1,tempRegisters);
                                    printf("mflo %s\n",findRegister(registers,size,*parts[0]));
                                    printf("L%d:\n",labels+1);
                                    tempRegisters++;
                                    labels += 2;
                                } else {
                                    printf("bltz %s,L%d\n",findRegister(registers,size,firstOperandVar), labels);
                                    printf("srl %s,%s,%d\n",findRegister(registers,size,*parts[0]),findRegister(registers,size,firstOperandVar), powerOfTwo);
                                    if (isNegative){
                                        printf("sub %s,$zero,%s\n",findRegister(registers,size,*parts[0]),findRegister(registers,size,*parts[0]));
                                    }
                                    printf("j L%d\n",labels+1);
                                    printf("L%d:\n",labels);
                                    if (isNegative){
                                        printf("li $t%d,-%s\n",tempRegisters,parts[secondOperandIndex]);
                                    } else {
                                        printf("li $t%d,%s\n",tempRegisters,parts[secondOperandIndex]);
                                    }
                                    printf("div %s,$t%d\n",findRegister(registers,size,firstOperandVar),tempRegisters);
                                    printf("mflo %s\n",findRegister(registers,size,*parts[0]));
                                    printf("L%d:\n",labels+1);
                                    needTempReg = true;
                                    tempRegisters++;
                                    labels += 2;
                                }
                            } else {
                                //If not a power of two, print the normal method
                                if (instructions == i){
                                    if (isNegative){
                                        printf("li $t%d,-%s\n",tempRegisters,parts[secondOperandIndex]);
                                    } else {
                                        printf("li $t%d,%s\n",tempRegisters,parts[secondOperandIndex]);
                                    }
                                    printf("div %s,$t%d\n", findRegister(registers, size, firstOperandVar), tempRegisters);
                                    printf("mflo %s\n",findRegister(registers,size,*parts[0]));
                                } else {
                                    if (isNegative){
                                         printf("li $t%d,-%s\n",tempRegisters,parts[secondOperandIndex]);
                                    } else {
                                         printf("li $t%d,%s\n",tempRegisters,parts[secondOperandIndex]);
                                    }
                                    printf("div %s,$t%d\n", findRegister(registers, size, firstOperandVar), tempRegisters);
                                    printf("mflo $t%d\n",tempRegisters+1);
                                    needTempReg = true;
                                    tempRegisters += 2;
                                }
                            }
                        }
                    } else {
                        //If not a constant
                        if (instructions == i){
                            //If last instruction, check if a temporary register is needed then print
                            if (needTempReg == true){
                                printf("div $t%d,%s\n",tempRegisters - 1 ,findRegister(registers, size, secondOperandVar));
                                printf("mflo %s\n", findRegister(registers, size, *parts[0]));
                            }  else {
                                printf("div %s,%s\n", findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
                                printf("mflo %s\n", findRegister(registers, size, *parts[0]));
                            }
                        } else {
                            //If not the last instruction, check if a temp register has been used, then print and add temp registers as needed
                            if (tempRegisters == 0){
                                printf("div %s,%s\n", findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
                                printf("mflo $t0\n");
                                needTempReg = true;
                                tempRegisters++;
                            } else {
                                printf("div $t%d,%s\n", tempRegisters-1, findRegister(registers, size, secondOperandVar));
                                printf("mflo $t%d\n", tempRegisters);
                                tempRegisters++;
                            }
                        }
                    }
                    //If the operator is modulo
                } else if (operator == '%'){
                    //Check if a consant, then print
                    if (isNumeric(parts[secondOperandIndex])){
                        printf("li $t%d,%s\n", tempRegisters+1, parts[secondOperandIndex]);
                        printf("div $t%d,$t%d\n",tempRegisters, tempRegisters+1);
                        tempRegisters++;
                        needTempReg = true;
                        if (instructions == i){
                            printf("mfhi %s\n", findRegister(registers, size, *parts[0]));
                        } else {
                            printf("mfhi $t%d\n", tempRegisters+1);
                            tempRegisters++;
                        }
                    } else {
                        //If not a constant, do the same as above
                        if (instructions == i){
                            if (needTempReg == true){
                                printf("div $t%d,%s\n", tempRegisters -1, findRegister(registers, size, secondOperandVar));
                                printf("mfhi %s\n",findRegister(registers, size, *parts[0]));
                            } else {
                                printf("div %s,%s\n",findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
                                printf("mfhi %s\n", findRegister(registers, size, *parts[0]));
                            }
                        } else {
                            if (tempRegisters == 0){
                                printf("div %s,%s\n", findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
                                printf("mfhi $t0\n");
                                needTempReg = true;
                                tempRegisters++;
                            } else {
                                printf("div $t%d,%s\n",tempRegisters, findRegister(registers, size, secondOperandVar));
                                printf("mfhi $t%d\n", tempRegisters+1);
                                tempRegisters++;
                            }
                        }
                    }
                }

            }
        }
    }
    return 0;
}