/*
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2017/2018
    Semestar:       Zimski (V)

    Ime fajla:      server.c
    Opis:           TCP/IP server

    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include <stdio.h>     //printf
#include <string.h>    //memset
#include <arpa/inet.h>  //inet_aton
#include <sys/socket.h> //socket

#define BUFLEN 512   // Max length of buffer
#define PORT 8888    // The port on which to listen for incoming data
#define UNDEFINED -1 // Value for undefined stat variables

int main(void)
{
    struct sockaddr_in sock_server, sock_client;
    int s, slen = sizeof(sock_client), recv_len;
    char buf[BUFLEN];
    // Statistic variables
    int stat_min = UNDEFINED, stat_max = UNDEFINED, stat_n = 0;
    float stat_avg = 0;

    // Create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        puts("Could not create socket");
        return 1;
    }

    // Empty the structure
    memset((char*) &sock_server, 0, sizeof(sock_server));

    sock_server.sin_family = AF_INET;
    sock_server.sin_port = htons(PORT);
    sock_server.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind
    if(bind(s, (struct sockaddr*)&sock_server, sizeof(sock_server)) == -1)
    {
        perror("bind failed. Error");
        return 1;
    }

    // Listen for data
    while(1)
    {
        printf("\nWaiting for data...\n");
        fflush(stdout);

        // Empty the string
        memset((char*)buf, 0, BUFLEN);

        // Receive data (blocking call)
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &sock_client, (socklen_t*)&slen)) == -1)
        {
            perror("recvfrom failed. Error");
            return 1;
        }

        // Print client info and received data
        printf("Received packet from %s:%d\n", inet_ntoa(sock_client.sin_addr), ntohs(sock_client.sin_port));
        printf("Data: %s" , buf);

        // Calculate stat values
        stat_avg = (stat_avg * stat_n + recv_len) / (stat_n + 1);
        stat_n++;

        if (recv_len > stat_max || stat_max == UNDEFINED) {
          stat_max = recv_len;
        }

        if (recv_len < stat_min || stat_min == UNDEFINED) {
          stat_min = recv_len;
        }

        // Print stat values
        printf("This package: %d\nMax package: %d\nMin package: %d\nAVG package: %.2f\n", recv_len, stat_max, stat_min, stat_avg);

        // Send message back to client
        if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &sock_client, slen) == -1)
        {
            perror("sendto failed. Error");
            return 1;
        }
    }

    close(s);
    return 0;
}
