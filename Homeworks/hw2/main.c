#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct registerMapping {
    char variableLetter;
    char mipsRegister[4];
    int value;
};


void loadImediate(struct registerMapping* registers, int index, char letter, int value){
    registers[index].variableLetter = letter;
    // printf("%c ", registers[index].variableLetter);
    char registerString[4]; // Assuming a maximum register name size of 8 characters
    sprintf(registerString, "$s%d", index);
    strcpy(registers[index].mipsRegister, registerString);
    // printf("%s ",registers[index].mipsRegister);
    registers[index].value = value;
    // printf("%d \n",registers[index].value);
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

    char buffer[100];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("# %s", buffer);
        char* token = strtok(buffer, " \t\n\r");
        char* parts[100];
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
            loadImediate(registers, size, *parts[0], atoi(parts[2]));
            printf("li %s,%d\n", registers[size].mipsRegister, registers[size].value);
            size++;
        } else if (partCount >= 5) {
            int instructions = partCount / 2 - 1;
            for (int i = 1; i <= instructions; i++){
                int firstOperandIndex = 2 * i;
                int secondOperandIndex = firstOperandIndex + 2;
                int operatorIndex = firstOperandIndex + 1;

                if (*parts[operatorIndex] == '+'){
                    if (isNumeric(parts[secondOperandIndex])){
                        printf("addi $t0,%s", parts[secondOperandIndex]);
                    }
                }

            }
        }
    }

    printf("\n");

    return 0;
}