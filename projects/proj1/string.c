/**
 * IPK 2018 project 1
 * @file string.c
 * @author Jiri Jurica
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string.h"

/**
 * Checks if line feed character is in string. isLineFeed and readLine functions are from my previous projects.
 * @param str - input string
 * @param length - length of the string
 * @return 1 if line feed is found, else 0
 */
int isLineFeed (const char *str, int length)
{
    for (int i = 0; i < length - 2; i++)    // -1 for index and next -1 for zero byte
    {
        if (str[i] == '\n')
        {
            return 1;
        }
    }

    return 0;
}

char *readLine(FILE *file)         // Reads one line.
{
    const int step = 30;
    int count = 1;

    char *line;
    char *endOfString;   // Points always on latest 'step' values of line

    if ((endOfString = line = malloc(step * sizeof(char))) == NULL) return NULL;

    if (fgets(line, step, file) == NULL)
    {
        free(line);
        return NULL;
    }

    while (isLineFeed(endOfString, step) == 0)
    {
        if ((line = realloc(line, (count + 1) * step * sizeof(char))) == NULL) return NULL;

        endOfString = line + (step - 1) * count;    // There could be a new address after realloc.
        fgets(endOfString, step, file);
        count++;
    }

    return line;
}

void getNthItem(char **token, int index, char *delim) {
    for (int i = 0; i < index; i++) *token = strtok(NULL, delim);
}

char *getUserInfo(FILE *file, const char *login) {
    char *line, *token, *info;

    fseek(file, 0, SEEK_SET);

    while ((line = readLine(file)) != NULL) {
        token = strtok(line, ":");

        if (strcmp(login, token) == 0) {
            getNthItem(&token, 4, ":");  // 4 - user info

            if ((info = malloc((strlen(token) + 1) * sizeof(char))) == NULL) {
                free(line);
                return NULL;
            }

            strcpy(info, token);
            free(line);
            return info;
        }

        free(line);
    }

    return NULL;
}

char *getHomePath(FILE *file, const char *login) {
    char *line, *token, *path;

    fseek(file, 0, SEEK_SET);

    while ((line = readLine(file)) != NULL) {
        token = strtok(line, ":");

        if (strcmp(login, token) == 0) {
            getNthItem(&token, 5,":");  // 5 - user home path

            if ((path = malloc((strlen(token) + 1) * sizeof(char))) == NULL) {
                free(path);
                return NULL;
            }

            strcpy(path, token);
            free(line);
            return path;
        }

        free(line);
    }

    return NULL;
}

int startsLike(const char *pattern, const char* str) {
    int equality = 1;

    while (pattern[0] != '\0') {
        if (str[0] == '\0' || pattern[0] != str[0]) {   // If chars are different or str size is less then pattern size
            equality = 0;
            break;
        }

        pattern++;
        str++;
    }

    return equality;
}

void freeList(char **list, int size) {
    for (int i = 0; i < size; i++) {
        free(list[i]);
    }

    free(list);
}

char **getLoginList(FILE *file, char *pattern, int *size) {
    const int step = 10;
    int count = 1;
    char **list, *line, *token, *login;

    fseek(file, 0, SEEK_SET);
    *size = 0;

    if ((list = malloc(step * sizeof(char*))) == NULL) return NULL;

    while ((line = readLine(file)) != NULL) {
        token = strtok(line, ":");

        if (startsLike(pattern, token)) {   // if login starts like pattern
            if ((login = malloc((strlen(token) + 1) * sizeof(char))) == NULL) {
                free(line);
                freeList(list, *size);
                return NULL;
            }

            if (*size == step * count) { // if list is full
                if ((list = realloc(list, ++count * step * sizeof(char*))) == NULL) {
                    free(line);
                    free(login);
                    freeList(list, *size);
                    return NULL;
                }
            }

            strcpy(login, token);
            list[(*size)++] = login;
        }

        free(line);
    }

    if (*size == 0) {
        free(list);
        return NULL;
    }
    else return list;
}

void removeLineFeed(char **str) {
    char *point = *str;

    while (*point != '\0') {
        if (*point == '\n' || *point == 13) {   // Telnet from another way sends CR on end of line
            *point = '\0';
            break;
        }

        point++;
    }
}

// Bad part of code writen in time need

char *jumpAfterDelim(char *str, const char *delim) {
    char *point = str;

    while (*point != delim[0]) {
        point++;
    }

    return point + 1;
}

int findChar(char **str, const char *delim, int size) {
    int result = 0;

    for (int i = 0; i < size; i++) {
        if ((*str)[i] == delim[0]) {
            (*str)[i] = '\0';
            result = 1;
            break;
        }
    }

    return result;
}