#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

void convertir_nombre_archivo(char *nombre) {
    for (int i = 0; nombre[i]; i++) {
        nombre[i] = toupper(nombre[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <archivo_entrada> <IP_servidor> <puerto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *archivo_entrada = argv[1];
    char *ip_servidor = argv[2];
    int puerto = atoi(argv[3]);

    printf("Abriendo archivo de entrada: %s\n", archivo_entrada);
    FILE *entrada = fopen(archivo_entrada, "r");
    if (!entrada) {
        perror("Error al abrir el archivo de entrada");
        exit(EXIT_FAILURE);
    }

    char archivo_salida[BUFFER_SIZE];
    strcpy(archivo_salida, archivo_entrada);
    convertir_nombre_archivo(archivo_salida);

    printf("Abriendo archivo de salida: %s\n", archivo_salida);
    FILE *salida = fopen(archivo_salida, "w");
    if (!salida) {
        perror("Error al abrir el archivo de salida");
        fclose(entrada);
        exit(EXIT_FAILURE);
    }

    int cliente_socket;
    struct sockaddr_in direccion_servidor;
    char buffer[BUFFER_SIZE];

    printf("Creando socket del cliente...\n");
    // Crear socket del cliente
    cliente_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (cliente_socket < 0) {
        perror("Error al crear el socket del cliente");
        fclose(entrada);
        fclose(salida);
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_port = htons(puerto);
    if (inet_pton(AF_INET, ip_servidor, &direccion_servidor.sin_addr) <= 0) {
        perror("Error en inet_pton");
        close(cliente_socket);
        fclose(entrada);
        fclose(salida);
        exit(EXIT_FAILURE);
    }

    printf("Conectando al servidor en %s:%d...\n", ip_servidor, puerto);
    // Conectar al servidor
    if (connect(cliente_socket, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor)) < 0) {
        perror("Error al conectar con el servidor");
        close(cliente_socket);
        fclose(entrada);
        fclose(salida);
        exit(EXIT_FAILURE);
    }

    printf("Conexión establecida. Enviando datos...\n");
    while (fgets(buffer, BUFFER_SIZE, entrada)) {
        printf("Enviando línea al servidor: %s", buffer);
        send(cliente_socket, buffer, strlen(buffer), 0);
        memset(buffer, 0, BUFFER_SIZE);
        recv(cliente_socket, buffer, BUFFER_SIZE, 0);
        printf("Recibido del servidor: %s", buffer);
        fprintf(salida, "%s", buffer);
    }

    printf("Cerrando conexión y archivos...\n");
    close(cliente_socket);
    fclose(entrada);
    fclose(salida);

    printf("Proceso completado.\n");
    return 0;
}