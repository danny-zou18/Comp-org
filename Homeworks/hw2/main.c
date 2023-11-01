#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef int bool;
#define true 1
#define false 0

struct registerMapping {
    char variableLetter;
    char mipsRegister[4];
    int value;
};

void saveRegister(struct registerMapping* registers, int index, char letter, int value){
    registers[index].variableLetter = letter;
    // printf("%c \n", registers[index].variableLetter);
    char registerString[4]; // Assuming a maximum register name size of 8 characters
    sprintf(registerString, "$s%d", index);
    strcpy(registers[index].mipsRegister, registerString);
    // printf("%s ",registers[index].mipsRegister);
    registers[index].value = value;
    // printf("%d \n",registers[index].value);
}
char* findRegister(struct registerMapping* registers, int size, char variable){
    for (int i = 0; i < size; i++){
        if (registers[i].variableLetter == variable){
            return registers[i].mipsRegister;
        }
    }
    return "";
}
void findExponents(int constant, int exponents[], int* numExponents) {
    int exponent = 0;
    *numExponents = 0;

    while (constant > 0) {
        if (constant & 1) {
            exponents[(*numExponents)++] = exponent;
        }
        constant >>= 1;
        exponent++;
    }
}
int calculateInstructions(int partCount){
    int instructions = partCount / 2;
    return instructions;
}
int isNumeric(const char* str) {
    while (*str) {
        if (!isdigit(*str) && *str != '-') {
            return 0; // Not a number
        }
        str++;
    }
    return 1; // It's a number
}
bool isPowerOfTwo(int number) {
    if (number <= 0) {
        return false; // Numbers less than or equal to 0 are not powers of 2
    }
    // Check if there is only one set bit in the binary representation
    return (number & (number - 1)) == 0;
}
int powerOfTwoExponent(int number) {
    int exponent = 0;
    while ((number & 1) == 0) {
        number >>= 1;
        exponent++;
    }
    return exponent;
}
int main(int argc, char *argv[]){
    if (argc < 2) {
        printf("Missing command line argument\n");
        return 1;
    }
    char filename[128];
    strncpy(filename, argv[1], sizeof(filename));

    FILE *file = fopen(filename, "r");
    if (file == NULL){
        printf("Failed to open file: %s", filename);
        return 1;
    }
    struct registerMapping* registers = NULL;
    int size = 0;
    int capacity = 100;
    registers = (struct registerMapping*)malloc(capacity * sizeof(struct registerMapping));

    char buffer[128];
    bool needTempReg = false;
    int tempRegisters = 0;
    int labels = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("# %s", buffer);

        if (buffer[strlen(buffer)-1]!='\n'){
            printf("\n");
        }
        char* token = strtok(buffer, " \t\n\r");
        char* parts[128];
        int partCount = 0;
        while (token != NULL) {
            if (token[strlen(token)-1] == ';'){
                token[strlen(token) - 1] = '\0';
            }
            parts[partCount] = token;
            partCount++;
            token = strtok(NULL, " \t\n\r");
        }
        if (partCount == 3) {
            saveRegister(registers, size, *parts[0], atoi(parts[2]));
            printf("li %s,%d\n", registers[size].mipsRegister, registers[size].value);
            size++;
        } else if (partCount >= 5) {
            saveRegister(registers,size,*parts[0],1);
            size++;

            int instructions = partCount / 2 - 1;
            for (int i = 1; i <= instructions; i++){
                int firstOperandIndex = 2 * i;
                int secondOperandIndex = firstOperandIndex + 2;
                int operatorIndex = firstOperandIndex + 1;
                char firstOperandVar = *parts[firstOperandIndex];
                char secondOperandVar = *parts[secondOperandIndex];
                char operator = *parts[operatorIndex];
                if (strcmp(findRegister(registers, size, secondOperandVar), "") == 0 && !isNumeric(parts[secondOperandIndex])){
                    saveRegister(registers,size,secondOperandVar,1);
                    size++;
                }
                if (operator == '+'){
                    if (isNumeric(parts[secondOperandIndex])){
                        if (instructions == i){
                            if (needTempReg == true){
                                printf("addi %s,$t%d,%s\n", findRegister(registers, size, *parts[0]),tempRegisters-1,parts[secondOperandIndex]);
                            } else {
                                printf("addi %s,%s,%s\n", findRegister(registers, size, *parts[0]), findRegister(registers, size, firstOperandVar), parts[secondOperandIndex]);
                            }
                        } else {
                            if (tempRegisters == 0){
                                printf("addi $t0,%s,%s\n", findRegister(registers, size, firstOperandVar), parts[secondOperandIndex]);
                                needTempReg = true;
                                tempRegisters++;
                            } else {
                                printf("addi $t%d,$t%d,%s\n", tempRegisters, tempRegisters - 1, parts[secondOperandIndex]);
                                tempRegisters++;
                            }
                        } 
                    } else {
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
                } else if (operator == '-'){
                    if (isNumeric(parts[secondOperandIndex])){
                        if (instructions == i){
                            if (needTempReg == true){
                                printf("addi %s,$t%d,-%s\n", findRegister(registers, size, *parts[0]),tempRegisters-1 ,parts[secondOperandIndex]);
                            } else {
                                printf("addi %s,%s,-%s\n", findRegister(registers,size, *parts[0]), findRegister(registers, size, firstOperandVar), parts[secondOperandIndex]);
                            }
                        } else {
                            if (tempRegisters == 0){
                                printf("addi $t0,%s,-%s\n", findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
                                needTempReg = true;
                                tempRegisters++;
                            } else {
                                printf("addi $t%d,$t%d,-%s\n", tempRegisters, tempRegisters-1, parts[secondOperandIndex]);
                                tempRegisters++;
                            }
                        }   
                    } else {
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
                } else if (operator == '*'){
                    if (isNumeric(parts[secondOperandIndex])){
                        if (strcmp(parts[secondOperandIndex], "0") == 0){
                            printf("li %s,0\n",findRegister(registers,size,*parts[0]));
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
                            bool isNegative = false;
                            if (parts[secondOperandIndex][0] == '-'){
                                isNegative = true;
                                int len = strlen(parts[secondOperandIndex]);
                                for (int i = 0; i < len; i++) {
                                    parts[secondOperandIndex][i] = parts[secondOperandIndex][i + 1];
                                }
                            }
                            needTempReg = true;
                            int neededExponents[32];
                            int constant = atoi(parts[secondOperandIndex]);
                            int numExponents;
                            findExponents(constant, neededExponents, &numExponents);
                            for (int i = 0; i < numExponents; i++){
                                if (i == 0){
                                    printf("sll $t%d,%s,%d\n",tempRegisters, findRegister(registers, size, firstOperandVar), neededExponents[numExponents-i-1]);
                                    printf("move $t%d,$t%d\n", tempRegisters+1, tempRegisters);
                                    continue;
                                }
                                if (neededExponents[numExponents-i-1] != 0){
                                    printf("sll $t%d,%s,%d\n",tempRegisters, findRegister(registers, size, firstOperandVar), neededExponents[numExponents-i-1]);
                                    printf("add $t%d,$t%d,$t%d\n",tempRegisters+1, tempRegisters+1, tempRegisters);
                                } else {
                                    printf("add $t%d,$t%d,%s\n",tempRegisters+1, tempRegisters+1, findRegister(registers, size, firstOperandVar));
                                }
                            }
                            if (instructions == i){
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
                        if (instructions == i){
                            if (needTempReg == true){
                                printf("mult $t%d,%s\n",tempRegisters-1 ,findRegister(registers, size, secondOperandVar));
                                printf("mflo %s\n", findRegister(registers, size, *parts[0]));
                            } else {
                                printf("mult %s,%s\n", findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
                                printf("mflo %s\n", findRegister(registers, size, *parts[0]));
                            }
                        } else {
                            if (tempRegisters == 0){
                                printf("mult %s,%s\n", findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
                                printf("mflo $t0\n");
                                needTempReg = true;
                                tempRegisters++;
                            } else {
                                printf("mult $t%d,%s\n", tempRegisters-1,findRegister(registers, size, secondOperandVar));
                                printf("mflo $t%d\n", tempRegisters);
                                tempRegisters++;
                            }
                            
                        }
                    }
                } else if (operator == '/'){
                    if (isNumeric(parts[secondOperandIndex])){
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
                        } else {
                            bool isNegative = false;
                            if (parts[secondOperandIndex][0] == '-'){
                                isNegative = true;
                                int len = strlen(parts[secondOperandIndex]);
                                for (int i = 0; i < len; i++) {
                                    parts[secondOperandIndex][i] = parts[secondOperandIndex][i + 1];
                                }
                            }
                            if (isPowerOfTwo(atoi(parts[secondOperandIndex]))){
                                int powerOfTwo = powerOfTwoExponent(atoi(parts[secondOperandIndex]));
                                if (needTempReg){
                                    printf("bltz $t%d,L%d\n",tempRegisters - 1,labels);
                                    printf("srl %s,$t%d,%d\n",findRegister(registers,size,*parts[0]),tempRegisters -1, powerOfTwo);
                                    if (isNegative){
                                        printf("sub %s,$zero,%s\n", findRegister(registers,size,*parts[0]),findRegister(registers,size,*parts[0]));
                                    }
                                    printf("j L%d\n",labels+1);
                                    printf("L%d:\n",labels);
                                    printf("li $t%d,%s\n",tempRegisters,parts[secondOperandIndex]);
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
                                    printf("li $t%d,%s\n",tempRegisters,parts[secondOperandIndex]);
                                    printf("div %s,$t%d\n",findRegister(registers,size,firstOperandVar),tempRegisters);
                                    printf("mflo %s\n",findRegister(registers,size,*parts[0]));
                                    printf("L%d:\n",labels+1);
                                    needTempReg = true;
                                    tempRegisters++;
                                    labels += 2;
                                }
                            } else {
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
                        if (instructions == i){
                            if (needTempReg == true){
                                printf("div $t%d,%s\n",tempRegisters - 1 ,findRegister(registers, size, secondOperandVar));
                                printf("mflo %s\n", findRegister(registers, size, *parts[0]));
                            }  else {
                                printf("div %s,%s\n", findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
                                printf("mflo %s\n", findRegister(registers, size, *parts[0]));
                            }
                        } else {
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
                } else if (operator == '%'){
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