/**
 * IPK 2018 project 1
 * @file string.h
 * @author Jiri Jurica
 */

#ifndef PROJ1_STRING_H
#define PROJ1_STRING_H

/**
 * Reads characters by groups while not found line feed and concatenates them to one string. Return string must be free
 * after use.
 * @param file
 * @return On success returns line string with different length, else NULL
 */
char *readLine(FILE *file);

/**
 * By function strtok 'seeks' to nth item from /etc/passwd, index starts in 0.
 * @param token - string after first strtok call
 * @param index
 * @param delim
 */
void getNthItem(char **token, int index, char *delim);

/**
 * Finds user information in /etc/passwd file for login
 * @param file
 * @param login
 * @return string with user info or NULL in case of allocation error or login not found
 */
char *getUserInfo(FILE *file, const char *login);

/**
 * Finds user home path in /etc/passwd file for login
 * @param file
 * @param login
 * @return string with user home path or NULL in case of allocation error or login not found
 */
char *getHomePath(FILE *file, const char *login);

/**
 * Compares if string starts like pattern.
 * @param pattern
 * @param str
 * @return 1 if starts like pattern else 0
 */
int startsLike(const char *pattern, const char* str);

/**
 * Creates list of strings with logins from /etc/passwd
 * @param file
 * @param pattern - if is set, returns just logins which match pattern or all if pattern is ""
 * @param size - to size function saves number of items in list
 * @return list of logins or NULL if pattern matches 0
 */
char **getLoginList(FILE *file, char *pattern, int *size);

/**
 * Removes from end line feed char and change it to \0
 * @param str
 */
void removeLineFeed(char **str);

/**
 * Frees generated list with logins
 * @param list
 * @param size
 */
void freeList(char **list, int size);

/**
 * Returns pointer after first delimiter in string. Make sure that really contains delimiter!
 * @param str
 * @param delim
 * @return
 */
char *jumpAfterDelim(char *str, const char *delim);

/**
 * Checks if delim is in string and if yes, changes it to \0
 * @param str
 * @param delim
 * @param size
 * @return 1 id delim is in str elso 0
 */
int findChar(char **str, const char *delim, int size);

#endif //PROJ1_STRING_H
