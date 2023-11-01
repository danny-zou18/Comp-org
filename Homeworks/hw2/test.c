#include <stdio.h>

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

int main() {
    int constant = 32; // Replace with the constant you want to analyze
    int exponents[32]; // An array to store the exponents
    int numExponents;  // Variable to store the number of exponents

    findExponents(constant, exponents, &numExponents);

    printf("Exponents of 2 for %d: ", constant);
    for (int i = 0; i < numExponents; i++) {
        printf("%d ", exponents[i]);
    }
    printf("\n");

    return 0;
}