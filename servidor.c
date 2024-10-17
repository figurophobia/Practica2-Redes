#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// Tamaño del buffer para el mensaje de saludo
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {

    int puerto = 0;
    // Verificar que se haya pasado el puerto como argumento
    if (argc == 2 && atoi(argv[1]) > 0 && atoi(argv[1]) < 65536) {
        printf("Puerto: %s\n", argv[1]);
        puerto = atoi(argv[1]);
    } else {
        printf("Usando el puerto 50000 por defecto\n");
        puerto = 50000;
    }


    int servidor_socket, cliente_socket;
    struct sockaddr_in direccion_servidor, direccion_cliente;
    socklen_t cliente_len = sizeof(direccion_cliente);
    char buffer[BUFFER_SIZE] = "¡Hola, cliente! Bienvenido al servidor.\n";

    // Crear socket del servidor
    servidor_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor_socket < 0) {
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_addr.s_addr = INADDR_ANY; // Escuchar en todas las interfaces
    direccion_servidor.sin_port = htons(puerto);
    // Asociar el socket con la dirección y puerto
    if (bind(servidor_socket, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor)) < 0) {
        perror("Error al hacer bind en el socket del servidor");
        close(servidor_socket);
        exit(EXIT_FAILURE);
    }

    // Poner el servidor en modo de escucha
    if (listen(servidor_socket, 5) < 0) {
        perror("Error al hacer listen en el socket del servidor");
        close(servidor_socket);
        exit(EXIT_FAILURE);
    }

    printf("Servidor esperando conexiones en el puerto %d...\n", puerto);

    while (1) {
        // Aceptar una conexión de cliente
        cliente_socket = accept(servidor_socket, (struct sockaddr *)&direccion_cliente, &cliente_len);
        if (cliente_socket < 0) {
            perror("Error al aceptar la conexión");
            close(servidor_socket);
            exit(EXIT_FAILURE);
        }

        // Obtener la IP y puerto del cliente
        char cliente_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &direccion_cliente.sin_addr, cliente_ip, INET_ADDRSTRLEN);
        printf("Conexión aceptada de %s:%d\n", cliente_ip, ntohs(direccion_cliente.sin_port));

        // Enviar mensaje de saludo al cliente
        ssize_t enviado = send(cliente_socket, buffer, strlen(buffer)+1, 0);
        if (enviado < 0) {
            perror("Error al enviar el mensaje al cliente");
        } else {
            printf("Mensaje enviado al cliente (%ld bytes)\n", enviado);
        }


        // Cerrar el socket de conexión con el cliente
        close(cliente_socket);
    }

    // Cerrar el socket del servidor
    close(servidor_socket);

    return 0;
}
