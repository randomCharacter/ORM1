/*
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2017/2018
    Semestar:       Zimski (V)

    Ime fajla:      client.c
    Opis:           TCP/IP klijent

    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include <stdio.h>      //printf
#include <string.h>     //strlen
#include <sys/socket.h> //socket
#include <arpa/inet.h>  //inet_addr
#include <fcntl.h>      //for open
#include <unistd.h>     //for close
#include <stdlib.h>     //atoi

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015

int main(int argc , char *argv[])
{
    // Check if port is provided
    int port;
    if (argc != 2)
    {
        puts("Port nije specifiran!");
        printf("%s PORT\n", argv[0]);
        return 1;
    }
    port = atoi(argv[1]);

    //Create socket to send data
    int sock;
    struct sockaddr_in server, client;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        puts("Could not create socket");
        return 1;
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
    puts("Konektovan. Cekam drugog igraca...");

    // Create socket to receive data
    int socket_desc , client_sock;
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        puts("Could not create socket");
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // Bind
    if (bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return 1;
    }

    // Send port to server
    if (send(sock, &port, sizeof(int), 0) < 0)
    {
        perror("send failed. Error");
        return 1;
    }

    // Listen
    listen(socket_desc, 3);

    // Accept connection from server
    int c = sizeof(struct sockaddr_in);
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed. Error");
        return 1;
    }

    // Recieve message
    char message[DEFAULT_BUFLEN];
    if (recv(client_sock , message, DEFAULT_BUFLEN , 0) == -1)
    {
        perror("recv failed. Error");
        return 1;
    }

    // Show start message
    printf("Poruka od servera: %s\n", message);

    // Read sign
    char sign;
    printf("Unesite znak (l, k, m)\n> ");
    scanf("%c", &sign);
    puts("Cekam na drugog igraca...");

    // Send sign to server
    if (send(sock, &sign, sizeof(char), 0) < 0)
    {
        perror("send failed. Error");
        return 1;
    }

    // Receive message
    if (recv(client_sock , message , DEFAULT_BUFLEN , 0) == -1)
    {
        perror("recv failed. Error");
        return 1;
    }

    // Print message
    puts(message);

    // Close sockets
    close(sock);
    close(socket_desc);

    return 0;
}
