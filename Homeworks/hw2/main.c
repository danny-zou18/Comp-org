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
    (index)++;
}
char* findRegister(struct registerMapping* registers, int size, char variable){
    for (int i = 0; i < size; i++){
        if (registers[i].variableLetter == variable){
            return registers[i].mipsRegister;
        }
    }
    return "";
}

int calculateInstructions(int partCount){
    int instructions = partCount / 2;
    return instructions;
}
int isNumeric(const char* str) {
    while (*str) {
        if (!isdigit(*str)) {
            return 0; // Not a number
        }
        str++;
    }
    return 1; // It's a number
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
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("# %s", buffer);
        bool needTempReg = false;
        int tempRegisters = 0;

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
            saveRegister(registers, size, *parts[0], 1);
            size++;
            int instructions = partCount / 2 - 1;
            for (int i = 1; i <= instructions; i++){
                int firstOperandIndex = 2 * i;
                int secondOperandIndex = firstOperandIndex + 2;
                int operatorIndex = firstOperandIndex + 1;
                char firstOperandVar = *parts[firstOperandIndex];
                char secondOperandVar = *parts[secondOperandIndex];
                char operator = *parts[operatorIndex];
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
                        continue;
                    } else {
                        if (instructions == i){
                            if (needTempReg == true){
                                printf("mult $t0,%s\n", findRegister(registers, size, secondOperandVar));
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
                                printf("mult $t%d, %s\n", tempRegisters-1,findRegister(registers, size, secondOperandVar));
                                printf("mflo $t%d\n", tempRegisters);
                                tempRegisters++;
                            }
                            
                        }
                    }
                } else if (operator == '/'){
                    if (isNumeric(parts[secondOperandIndex])){
                        continue;
                    } else {
                        if (instructions == i){
                            if (needTempReg == true){
                                printf("div $t0,%s\n", findRegister(registers, size, secondOperandVar));
                                printf("mflo %s\n", findRegister(registers, size, *parts[0]));
                            }  else {
                                printf("div %s,%s\n", findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
                                printf("mflo %s\n", findRegister(registers, size, *parts[0]));
                            }
                        } else {
                            if (tempRegisters == 0){
                                printf("div %s, %s\n", findRegister(registers, size, firstOperandVar), findRegister(registers, size, secondOperandVar));
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
                }

            }
        }
    }
    return 0;
}