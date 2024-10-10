#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int puerto = atoi(argv[1]);
    int server_fd, nuevo_socket;
    struct sockaddr_in direccion;
    struct sockaddr_in direccion_cliente;
    socklen_t tamano_direccion_cliente = sizeof(direccion_cliente);
    char buffer[BUFFER_SIZE] = {0};
    char *mensaje = "Hola desde el servidor";

    // PASO 1: CREAR SOCKET
    // Creamos el descriptor de archivo del socket
    // Usamos AF_INET para IPv4 y SOCK_STREAM para conexión TCP
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("fallo en el socket");
        exit(EXIT_FAILURE);
    }

    // Adjuntar el socket al puerto de forma forzada
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(puerto);

    if (bind(server_fd, (struct sockaddr *)&direccion, sizeof(direccion)) < 0) {
        perror("fallo en el bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("fallo en el listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Esperando una conexión...\n");
        if ((nuevo_socket = accept(server_fd, (struct sockaddr *)&direccion_cliente, &tamano_direccion_cliente)) < 0) {
            perror("fallo en el accept");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        char ip_cliente[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &direccion_cliente.sin_addr, ip_cliente, INET_ADDRSTRLEN);
        int puerto_cliente = ntohs(direccion_cliente.sin_port);

        printf("Conexión aceptada desde %s:%d\n", ip_cliente, puerto_cliente);
        send(nuevo_socket, mensaje, strlen(mensaje), 0);
        printf("Mensaje enviado a %s:%d\n", ip_cliente, puerto_cliente);

        close(nuevo_socket);
    }

    close(server_fd);
    return 0;
}