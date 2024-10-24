#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Definimos el tamaño del buffer para el mensaje de saludo
#define buffer_SIZE 1024

int main(int argc, char *argv[]) {
    char ip[INET_ADDRSTRLEN];
    int puerto;

    // Verificar los argumentos de línea de comandos
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <IP del servidor> <Puerto>\n", argv[0]);
        do {
            printf("Introduce la IP del servidor: ");
            scanf("%s", ip);
            printf("Introduce el puerto (0-65535): ");
            scanf("%d", &puerto);
        } while (puerto < 0 || puerto > 65535);
    } else {
        strcpy(ip, argv[1]);
        puerto = atoi(argv[2]);
        if (puerto < 0 || puerto > 65535) {
            do {
                printf("Puerto inválido. Introduce el puerto (0-65535): ");
                scanf("%d", &puerto);
            } while (puerto < 0 || puerto > 65535);
        }
    }

    // PASO 0: Creamos las variables ---------------------------------------------------------------------
    int sock; // Socket de la conexión
    struct sockaddr_in direccionServidor; // Dirección del servidor
    char buffer[buffer_SIZE]; // Cadena de texto para recibir datos
    int bytesRecibidos; // Número de bytes recibidos

    // PASO 1: Creamos el socket del cliente ------------------------------------------------------------
    sock = socket(AF_INET, SOCK_STREAM, 0); // AF_INET para IPv4, SOCK_STREAM para protocolo TCP
    if (sock == -1) { // Si la función devuelve un valor negativo, significa que hubo un error en la creación del socket
        perror("Error al crear el socket");
        return 1;
    }

    // PASO 2: Configuramos la dirección del servidor ----------------------------------------------------
    direccionServidor.sin_family = AF_INET; // Familia de direcciones IPv4
    direccionServidor.sin_port = htons(puerto); // Convertir el puerto de entero a formato de red
    if (inet_pton(AF_INET, ip, &direccionServidor.sin_addr) <= 0) {
        perror("Error al convertir la IP");
        close(sock);
        return 1;
    }

    // PASO 3: Conectamos al servidor ---------------------------------------------------------------------
    if (connect(sock, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) == -1) { // Si la función devuelve un valor negativo, hay error
        perror("Error al conectar al servidor");
        close(sock); // Cerramos el socket
        return 1;
    }

    // PASO 4: Recibimos datos del servidor ----------------------------------------------------------------
    bytesRecibidos = recv(sock, buffer, buffer_SIZE - 1, 0); // Recibimos datos del servidor
    if (bytesRecibidos == -1) { // Si la función devuelve un valor negativo, hay error
        perror("Error al recibir datos del servidor");
        close(sock);
        return 1;
    }

    // PASO 5: Mostramos los datos recibidos ----------------------------------------------------------------
    buffer[bytesRecibidos] = '\0'; // Asegurarse de que el buffer esté terminado en nulo
    printf("buffer recibido: %s\n", buffer);
    printf("Número de bytes recibidos: %d\n", bytesRecibidos);

    // Cerrar el socket
    close(sock);

    return 0;
}
