#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

// Función para convertir una cadena a mayúsculas
void convertir_a_mayusculas(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int puerto = atoi(argv[1]);
    int servidor_socket, cliente_socket;
    struct sockaddr_in direccion_servidor, direccion_cliente;
    socklen_t cliente_len = sizeof(direccion_cliente);
    char buffer[BUFFER_SIZE];

    // Crear socket del servidor
    printf("Creando socket del servidor...\n");
    servidor_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor_socket < 0) {
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    printf("Configurando la dirección del servidor...\n");
    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_addr.s_addr = INADDR_ANY;
    direccion_servidor.sin_port = htons(puerto);

    // Asociar el socket con la dirección y puerto
    printf("Asociando el socket con la dirección y puerto...\n");
    if (bind(servidor_socket, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor)) < 0) {
        perror("Error al hacer bind en el socket del servidor");
        close(servidor_socket);
        exit(EXIT_FAILURE);
    }

    // Poner el socket en modo de escucha
    printf("Poniendo el socket en modo de escucha...\n");
    if (listen(servidor_socket, 5) < 0) {
        perror("Error al poner el socket en modo de escucha");
        close(servidor_socket);
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchando en el puerto %d\n", puerto);

    while (1) {
        printf("Esperando una conexión...\n");
        cliente_socket = accept(servidor_socket, (struct sockaddr *)&direccion_cliente, &cliente_len);
        if (cliente_socket < 0) {
            perror("Error al aceptar la conexión");
            close(servidor_socket);
            exit(EXIT_FAILURE);
        }

        char ip_cliente[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &direccion_cliente.sin_addr, ip_cliente, INET_ADDRSTRLEN);
        int puerto_cliente = ntohs(direccion_cliente.sin_port);
        printf("Conexión aceptada desde %s:%d\n", ip_cliente, puerto_cliente);

        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_recibidos = recv(cliente_socket, buffer, BUFFER_SIZE, 0);
            if (bytes_recibidos <= 0) {
                printf("Cliente desconectado.\n");
                break;
            }

            printf("Recibido del cliente: %s", buffer);
            convertir_a_mayusculas(buffer);
            printf("Enviando al cliente: %s", buffer);
            send(cliente_socket, buffer, strlen(buffer), 0);
        }

        printf("Cerrando conexión con el cliente...\n");
        close(cliente_socket);
    }

    printf("Cerrando el socket del servidor...\n");
    close(servidor_socket);
    return 0;
}