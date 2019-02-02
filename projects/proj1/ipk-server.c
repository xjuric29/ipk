/**
 * IPK 2018 project 1
 * @file ipk-server.c
 * @author Jiri Jurica
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>
#include <netinet/in.h>
#include "string.h"


// Globals
FILE *passwd;
int serverSocket;

/**
 * Closes socket and file after SIGINT
 * @param signal
*/
void catchSignal(int signal) {
    if (signal == SIGINT) {
        fclose(passwd);
        close(serverSocket);
        exit(0);
    }
}

int main(int argc, char **argv) {
    // Signal behaviour
    signal(SIGINT, catchSignal);

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

    // Site part
    // Server setup
    int acceptSocket, badRequest;
    char buff[256];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        fprintf(stderr, "./ipk-server: socket error\n");
        exit(1);
    }

    bzero((char *) &serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons((unsigned short)port);

    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        fprintf(stderr, "./ipk-server: port is using\n");
        exit(1);
    }

    if ((listen(serverSocket, 10)) < 0)
    {
        fprintf(stderr, "./ipk-server: listen error\n");
        exit(1);
    }

    int listSize;
    char *token, *data, response[128], **dataList;
    const char nonexistLoginMSG[] = "ERROR NONEXIST LOGIN\n", badRequestMSG[] = "ERROR BAD REQUEST\n";
            //internalErrorMSG[] = "ERROR SERVER PROBLEM\n";

    passwd = fopen("/etc/passwd", "r");

    // Request processing
    while (1) {
        acceptSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);


        if (recv(acceptSocket, buff, 256, 0) <= 0) break;

        badRequest = 0;

        // Request syntax: 'SEND USERINFO|HOMEPATH|USERLIST {LOGIN}'
        if ((token = strtok(buff, " ")) != NULL && strcmp(token, "SEND") == 0) {
            if ((token = strtok(NULL, " ")) != NULL) {
                removeLineFeed(&token); // Login in end have line feed char

                if (strcmp(token, "USERINFO") == 0) {
                    if ((token = strtok(NULL, " ")) != NULL) {
                        removeLineFeed(&token); // Login in end have line feed char
                        if ((data = getUserInfo(passwd, token)) != NULL) {
                            //Response syntax: 'OK {DATA_STRING}'
                            sprintf(response, "OK %s\n", data);
                            send(acceptSocket, response, strlen(response) + 1, 0);
                            free(data);
                        }
                        else send(acceptSocket, nonexistLoginMSG, strlen(nonexistLoginMSG), 0);
                    }
                    else badRequest = 1;
                }
                else if (strcmp(token, "HOMEPATH") == 0) {
                    if ((token = strtok(NULL, " ")) != NULL) {
                        removeLineFeed(&token); // Login in end have line feed char

                        if ((data = getHomePath(passwd, token)) != NULL) {
                            sprintf(response, "OK %s\n", data);
                            send(acceptSocket, response, strlen(response) + 1, 0);
                            free(data);
                        }
                        else send(acceptSocket, nonexistLoginMSG, strlen(nonexistLoginMSG), 0);
                    }
                    else badRequest = 1;
                }
                else if (strcmp(token, "USERLIST") == 0) {
                    if ((token = strtok(NULL, " ")) != NULL) {
                        removeLineFeed(&token); // Login in end have line feed char
                        dataList = getLoginList(passwd, token, &listSize);
                    }
                    else dataList = getLoginList(passwd, "", &listSize);

                    sprintf(response, "OK %d\n", listSize);
                    send(acceptSocket, response, strlen(response), 0);

                    if (recv(acceptSocket, buff, 256, 0) <= 0) break;

                    token = buff;
                    removeLineFeed(&token);

                    if (strcmp(buff, "READY") == 0) {

                        for (int i = 0; i < listSize; i++) {
                            sprintf(response, "%s\n", dataList[i]);
                            send(acceptSocket, response, strlen(response), 0);
                        }

                        send(acceptSocket, ":\n", 5, 0);
                        freeList(dataList, listSize);
                    }
                    else badRequest = 1;
                }
                else badRequest = 1;
            }
        }
        else badRequest = 1;

        if (badRequest) send(acceptSocket, badRequestMSG, strlen(badRequestMSG), 0);

        close(acceptSocket);
    }
}