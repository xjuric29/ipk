#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


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

/**
 * Reads characters by groups while not found line feed and concatenates them to one string. Return string must be free
 * after use.
 * @param file
 * @return On success returns line string with different length, else NULL
 */
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

/**
 * By function strtok 'seeks' to nth item from /etc/passwd, index starts in 0.
 * @param token - string after first strtok call
 * @param index
 */
void getNthItem(char **token, int index) {
    for (int i = 0; i < index; i++) *token = strtok(NULL, ":");
}

/**
 * Finds user information in /etc/passwd file for login
 * @param file
 * @param login
 * @return string with user info or NULL in case of allocation error or login not found
 */
char *getUserInfo(FILE *file, const char *login) {
    char *line, *token, *info;

    while ((line = readLine(file)) != NULL) {
        token = strtok(line, ":");

        if (strcmp(login, token) == 0) {
            getNthItem(&token, 4);  // 4 - user info

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

/**
 * Finds user home path in /etc/passwd file for login
 * @param file
 * @param login
 * @return string with user home path or NULL in case of allocation error or login not found
 */
char *getHomePath(FILE *file, const char *login) {
    char *line, *token, *path;

    while ((line = readLine(file)) != NULL) {
        token = strtok(line, ":");

        if (strcmp(login, token) == 0) {
            getNthItem(&token, 5);  // 5 - user home path

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

/**
 * Compares if string starts like pattern.
 * @param pattern
 * @param str
 * @return 1 if starts like pattern else 0
 */
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

/**
 * Frees generated list with logins
 * @param list
 * @param size
 */
void freeList(char **list, int size) {
    for (int i = 0; i < size; i++) {
        free(list[i]);
    }

    free(list);
}

/**
 * Creates list of strings with logins from /etc/passwd
 * @param file
 * @param pattern - if is set, returns just logins which match pattern or all if pattern is ""
 * @param size - to size function saves number of items in list
 * @return list of logins or NULL if pattern matches 0
 */
char **getLoginList(FILE *file, char *pattern, int *size) {
    const int step = 10;
    int count = 1;
    char **list, *line, *token, *login;

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

int main(int argc, char **argv) {
    // Argument parsing
    char option;
    int port = -1;

    while ((option = getopt(argc, argv, "p:")) != -1) {
        switch (option) {
            case ':':   // In case of set option p without argument. Getopt prints error message
            case '?':
                return 1;
            case 'p':
                if (port == -1) {
                    if (sscanf(optarg, "%d", &port) != 1) { // ./ipk-server -p stonozka
                        fprintf(stderr, "./ipk-server: bad port\n");
                        return 1;
                    }
                }
                else {  // ./ipk-server -p 123 -p 8080
                    fprintf(stderr, "./ipk-server: multiple port options set\n");
                    return 1;
                }
        }
    }

    if (port == -1) {   // ./ipk-server
        fprintf(stderr, "./ipk-server: missing mandatory option -- 'p'\n");
        return 1;
    }

    FILE *passwd = fopen("/etc/passwd", "r");

    int size;
    char **list = getLoginList(passwd, "", &size);

    for (int i = 0; i < size; i++) {
        printf("%s\n", list[i]);
    }
    //printf("Port %d\n", port);*/
    freeList(list, size);
    fclose(passwd);
}