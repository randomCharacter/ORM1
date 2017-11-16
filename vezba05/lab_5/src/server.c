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

#include <stdio.h>      //printf
#include <string.h>     //strlen
#include <sys/socket.h> //socket
#include <arpa/inet.h>  //inet_addr
#include <unistd.h>     //write

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015

#define START_MESSAGE "start"
#define WIN_MESSAGE   "Odigrali ste %c, a protivnik %c.\nPobedili ste."
#define LOSE_MESSAGE  "Odigrali ste %c, a protivnik %c.\nIzgubili ste."
#define TIE_MESSAGE   "Odigrali ste %c, a protivnik %c.\nNereseno je."

#define ROCK     'k'
#define PAPER    'l'
#define SCISSORS 'm'

#define WON   1
#define LOST -1
#define TIE   0

int main(int argc, char *argv[])
{
    int socket_desc, client1_sock, client2_sock, c;
    struct sockaddr_in server1, server2, server, client;

    // Create socket to receive data
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        puts("Could not create socket");
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);

    // Bind
    if(bind(socket_desc,(struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return 1;
    }

    // Listen
    listen(socket_desc, 3);

    // Accept and incoming connection
    puts("Cekam na igrace...");
    c = sizeof(struct sockaddr_in);
    int client1_port, client2_port;

    // Accept connection from first client
    client1_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client1_sock < 0)
    {
        perror("accept failed. Error");
        return 1;
    }

    // Create socket to send data to first client
    int sock1;

    sock1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock1 == -1)
    {
        perror("Could not create socket");
        close(socket_desc);
        return 1;
    }

    // Receive port from first client
    if (recv(client1_sock, &client1_port, sizeof(int), 0) == -1)
    {
        perror("recv failed. Error");
        return 1;
    }

    // Get ip and port of first client
    server1.sin_addr.s_addr = client.sin_addr.s_addr;
    server1.sin_family = AF_INET;
    server1.sin_port = htons(client1_port);

    // Accept connection from second client
    client2_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client2_sock < 0)
    {
        perror("accept failed. Error");
        return 1;
    }

    // Create socket to send data to second client
    int sock2;

    sock2 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock2 == -1)
    {
        puts("Could not create socket");
        close(socket_desc);
        return 1;
    }

    // Receive port from second client
    if (recv(client2_sock, &client2_port, sizeof(int), 0) == -1)
    {
        perror("recv failed. Error");
        return 1;
    }

    // Get ip and port of second client
    server2.sin_addr.s_addr = client.sin_addr.s_addr;
    server2.sin_family = AF_INET;
    server2.sin_port = htons(client2_port);

    // Connect to first client
    if (connect(sock1, (struct sockaddr *)&server1, sizeof(server1)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    // Connect to second client
    while (connect(sock2, (struct sockaddr *)&server2, sizeof(server2)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    // Send clients start message
    if (send(sock1, START_MESSAGE, strlen(START_MESSAGE) + 1, 0) < 0)
    {
        perror("send failed. Error");
        return 1;
    }
    if (send(sock2, START_MESSAGE, strlen(START_MESSAGE) + 1, 0) < 0)
    {
        perror("send failed. Error");
        return 1;
    }

    // Receive a sign from first client
    char sign1;
    if (recv(client1_sock, &sign1, sizeof(char), 0) == -1)
    {
        perror("recv failed. Error");
        return 1;
    }

    // Receive a sign from second client
    char sign2;
    if (recv(client2_sock, &sign2, sizeof(char), 0) == -1)
    {
        perror("recv failed. Error");
        return 1;
    }

    // Find winner
    int status; // 1 if won, -1 if lost, 0 if tie
    char message1[DEFAULT_BUFLEN], message2[DEFAULT_BUFLEN];

    switch (sign1)
    {
    case ROCK:
        if (sign2 == SCISSORS)
        {
            status = WON;
        }
        else if (sign2 == PAPER)
        {
            status = LOST;
        }
        else
        {
            status = TIE;
        }
        break;
    case PAPER:
        if (sign2 == ROCK)
        {
            status = WON;
        }
        else if (sign2 == SCISSORS)
        {
            status = LOST;
        }
        else
        {
            status = TIE;
        }
        break;
    case SCISSORS:
        if (sign2 == PAPER)
        {
            status = WON;
        }
        else if (sign2 == ROCK)
        {
            status = LOST;
        }
        else
        {
            status = TIE;
        }
        break;
    }

    // Generate messages
    if (status == WON)
    {
        sprintf(message1, WIN_MESSAGE, sign1, sign2);
        sprintf(message2, LOSE_MESSAGE, sign2, sign1);
    }
    else if (status == LOST)
    {
        sprintf(message1, LOSE_MESSAGE, sign1, sign2);
        sprintf(message2, WIN_MESSAGE, sign2, sign1);
    }
    else
    {
        sprintf(message1, TIE_MESSAGE, sign1, sign2);
        sprintf(message2, TIE_MESSAGE, sign2, sign1);
    }

    // Send clients the result
    if (send(sock1, message1, strlen(message1) + 1, 0) < 0)
    {
        perror("send failed. Error");
        return 1;
    }
    if (send(sock2, message2, strlen(message2) + 1, 0) < 0)
    {
        perror("send failed. Error");
        return 1;
    }

    // Close sockets
    close(socket_desc);
    close(client1_sock);
    close(client2_sock);

    return 0;
}
