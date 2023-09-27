#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_HISTORY_SIZE 100

void printTime() {
    time_t currentTime;
    struct tm *timeInfo;

    time(&currentTime);
    timeInfo = localtime(&currentTime);
    if(timeInfo->tm_min - 32 < 0){
        printf("Current time: %02d:%02d:%02d\n", (timeInfo->tm_hour-1), (timeInfo->tm_min+60)-32, timeInfo->tm_sec);
    }
    else{
        printf("Current time: %02d:%02d:%02d\n", timeInfo->tm_hour, (timeInfo->tm_min-32), timeInfo->tm_sec);
    }
}
void printDate() {
    time_t currentTime;
    struct tm *timeInfo;

    time(&currentTime);
    timeInfo = localtime(&currentTime);
    printf("Current date: %02d/%02d/%04d\n", timeInfo->tm_mon + 1, timeInfo->tm_mday, timeInfo->tm_year + 1900);
}

void printHistory(char *history[], int count) {
    printf("Command History:\n");
    for (int i = 0; i < count; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}

int main() {
    char input[MAX_INPUT_SIZE];
    char *history[MAX_HISTORY_SIZE];
    int historyCount = 0;

    while (1) {
        printf("MyShell> ");
        fgets(input, sizeof(input), stdin);
        input[strlen(input) - 1] = '\0'; 

        if (historyCount < MAX_HISTORY_SIZE) {
            history[historyCount] = strdup(input);
            historyCount++;
        } else {
            free(history[0]);
            for (int i = 0; i < MAX_HISTORY_SIZE - 1; i++) {
                history[i] = history[i + 1];
            }
            history[MAX_HISTORY_SIZE - 1] = strdup(input);
        }

        char *token = strtok(input, " ");
        if (token == NULL) {
            continue; 
        }

        if (strcmp(token, "time") == 0) {
            printTime();
        } else if (strcmp(token, "date") == 0) {
            printDate();
        } else if (strcmp(token, "echo") == 0) {
            token = strtok(NULL, ""); 
            if (token != NULL) {
                printf("%s\n", token);
            }
        } else if (strcmp(token, "history") == 0) {
            printHistory(history, historyCount);
        } else if (strcmp(token, "exit") == 0) {
            for (int i = 0; i < historyCount; i++) {
                free(history[i]);
            }
            exit(0);
        } else {
            if (access(token, X_OK) != -1) {
                pid_t pid = fork();
                if (pid == 0) {
                    execlp(token, token, NULL);
                    perror("Error executing command");
                    exit(1);
                } else if (pid < 0) {
                    perror("Fork failed");
                } else {
                    wait(NULL);
                }
            } else {
                printf("Command not found: %s\n", token);
            }
        }
    }

    return 0;
}