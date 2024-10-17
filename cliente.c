#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <IP del servidor> <Puerto>\n", argv[0]);
        return 1;
    }

    // Variables
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // Crear el socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Error al crear el socket");
        return 1;
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2])); // Convertir el puerto de cadena a entero
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Error al convertir la IP");
        close(sock);
        return 1;
    }


    // Conectarse al servidor
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al conectar al servidor");
        close(sock);
        return 1;
    }

    // Recibir mensaje del servidor
    bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received == -1) {
        perror("Error al recibir datos del servidor");
        close(sock);
        return 1;
    }


    // Mostrar el mensaje recibido y el número de bytes
    printf("Mensaje recibido: %s\n", buffer);
    printf("Número de bytes recibidos: %d\n", bytes_received);

    // Cerrar el socket
    close(sock);

    return 0;
}
