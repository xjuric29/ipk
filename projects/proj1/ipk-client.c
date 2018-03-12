/**
 * IPK 2018 project 1
 * @file ipk-client.c
 * @author Jiri Jurica
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include "string.h"

int main(int argc, char **argv) {
    // Argument parsing
    char option, *hostname = NULL, *login = NULL;
    int port = -1, nFlag = 0, fFlag = 0, lFlag = 0;

    while ((option = getopt(argc, argv, "h:p:nfl")) != -1) {
        switch (option) {
            case ':':   // In case of set option without argument. Getopt prints error message
            case '?':
                return 1;
            case 'h':
                if (hostname == NULL) {
                    hostname = optarg;
                } else {  // ./ipk-server -p 123 -p 8080
                    fprintf(stderr, "./ipk-client: multiple hostname options set\n");
                    return 1;
                }
                break;
            case 'p':
                if (port == -1) {
                    if (sscanf(optarg, "%d", &port) != 1) { // ./ipk-server -p stonozka
                        fprintf(stderr, "./ipk-client: bad port\n");
                        return 1;
                    }
                } else {  // ./ipk-server -p 123 -p 8080
                    fprintf(stderr, "./ipk-client: multiple port options set\n");
                    return 1;
                }
                break;
            case 'n':
                if (nFlag == 0 && fFlag == 0 && lFlag == 0) {
                    nFlag = 1;
                } else {
                    fprintf(stderr, "./ipk-client: bad combination of options\n");
                    return 1;
                }
                break;
            case 'f':
                if (nFlag == 0 && fFlag == 0 && lFlag == 0) {
                    fFlag = 1;
                } else {
                    fprintf(stderr, "./ipk-client: bad combination of options\n");
                    return 1;
                }
                break;
            case 'l':
                if (nFlag == 0 && fFlag == 0 && lFlag == 0) {
                    lFlag = 1;
                } else {
                    fprintf(stderr, "./ipk-client: bad combination of options\n");
                    return 1;
                }
        }
    }

    if (hostname == NULL) {   // run program without hostname
        fprintf(stderr, "./ipk-client: missing mandatory option -- 'h'\n");
        return 1;
    }

    if (port == -1) {   // run program without port
        fprintf(stderr, "./ipk-client: missing mandatory option -- 'p'\n");
        return 1;
    }

    if (nFlag == 0 && fFlag == 0 && lFlag == 0) {   // run program without n, l or f option
        fprintf(stderr, "./ipk-client: missing one of option -- 'n|f|l'\n");
        return 1;
    }

    if (argv[argc - 1][0] != '-') login = argv[argc - 1];

    if ((nFlag || fFlag) && login == NULL) {
        fprintf(stderr, "./ipk-client: missing option login\n");
        return 1;
    }

    // Site part
    // Client setup
    int clientSocket;
    struct hostent *server;
    struct sockaddr_in serverAddr;
    char request[256], buff[256], originalBuff[256], *token;
    const char userInfoMSG[] = "SEND USERINFO", homePathMSG[] = "SEND HOMEPATH", userListMSG[] = "SEND USERLIST";

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        fprintf(stderr, "./ipk-client: socket error\n");
        exit(1);
    }

    if ((server = gethostbyname(hostname)) == NULL) {
        fprintf(stderr,"ipk-client: bad host %s\n", hostname);
        exit(1);
    }

    bzero((char *) &serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serverAddr.sin_addr.s_addr,
          server->h_length);
    serverAddr.sin_port = htons(port);

    if (connect(clientSocket, (const struct sockaddr *) &serverAddr, sizeof(serverAddr)) != 0) {
        fprintf(stderr,"ipk-client: connect error\n");
        exit(1);
    }

    // Crating of request contains on program options
    if (nFlag) sprintf(request, "%s %s\n", userInfoMSG, login);
    else if (fFlag) sprintf(request, "%s %s\n", homePathMSG, login);
    else {
        if (login == NULL) sprintf(request, "%s\n", userListMSG);
        else sprintf(request, "%s %s\n", userListMSG, login);
    }

    if (send(clientSocket, request, strlen(request) + 1, 0) < 0) {
        fprintf(stderr,"ipk-client: send error\n");
        exit(1);
    }

    if (recv(clientSocket, buff, 255, 0) < 0) {
        fprintf(stderr,"ipk-client: receive error\n");
        exit(1);
    }

    strcpy(originalBuff, buff); // Is important to copy buffer because strtok changes it
    token = strtok(buff, " ");

    // If server returns error
    if (strcmp(token, "ERROR") == 0) {
        fprintf(stderr,"%s", originalBuff);
        close(clientSocket);
        exit(1);
    }

    // Bad part of code writen in time need
    if (lFlag) {
        // Sends that program is ready for list of logins
        if (send(clientSocket, "READY", 6, 0) < 0) {
            fprintf(stderr,"ipk-client: send error\n");
            exit(1);
        }

        bzero((char *) &buff, sizeof(buff));

        int nextRound = 0;

        // Receives while income string with :
        do {
            if (recv(clientSocket, buff, 255, 0) < 0) {
                fprintf(stderr,"ipk-client: receive error\n");
                exit(1);
            }

            token = buff;
            nextRound = findChar(&token, ":", 256);
            printf("%s", buff);

        } while (!nextRound);

    }
    else {  // -n or -f option
        token = jumpAfterDelim(originalBuff, " ");
        printf("%s", token);
    }

    close(clientSocket);
}