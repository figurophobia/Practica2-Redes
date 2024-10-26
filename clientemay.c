#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ctype.h>

// NOTA: Las funciones (parámetros y salidas) están comentadas más exhaustivamente en cliente.c.

// Definimos el tamaño del buffer para el mensaje
#define BUFFER_SIZE 1024

// Función para convertir una cadena a mayúsculas
void aMayuscula(char *nombre) {
    for (int i = 0; nombre[i]; i++) {
        nombre[i] = toupper(nombre[i]);
    }
}

int main(int argc, char *argv[]) {
    
    //PASO 0: Creamos las variables de los parametros de entrada -------------------------------------------
    char *archivo_entrada;
    char *ip_servidor;
    int puerto;
    if (argc == 4 && atoi(argv[3]) > 0 && atoi(argv[3]) < 65536) {
        *archivo_entrada = argv[1];
        *ip_servidor = argv[2];
        puerto = atoi(argv[3]);
    } else {
        do
        {
            printf("Uso: %s <archivo_entrada> <ip_servidor> <puerto>\n", argv[0]);
            printf("Introduce la dirección IP del servidor: ");
            scanf("%s", ip_servidor);
            printf("Introduce el puerto del servidor: ");
            scanf("%d", &puerto);


        }while (puerto < 0 || puerto > 65535);
    }

    // PASO 1: Abrimos los archivos de entrada y salida -----------------------------------------------------
    printf("Abriendo archivo de entrada: %s\n", archivo_entrada);
    FILE *entrada = fopen(archivo_entrada, "r");
    if (!entrada) {
        perror("Error al abrir el archivo de entrada");
        exit(EXIT_FAILURE);
    }

    char archivo_salida[BUFFER_SIZE]; // Variable para el nombre del archivo de salida
    strcpy(archivo_salida, archivo_entrada); // Copiar el nombre del archivo de entrada
    aMayuscula(archivo_salida); // Convertir el nombre del archivo a mayúsculas

    printf("Abriendo archivo de salida: %s\n", archivo_salida);
    FILE *salida = fopen(archivo_salida, "w");
    if (!salida) {
        perror("Error al abrir el archivo de salida");
        fclose(entrada);
        exit(EXIT_FAILURE);
    }

    // PASO 2: Creamos el socket del cliente ----------------------------------------------------------------
    int cliente_socket; // Socket de la conexion
    struct sockaddr_in direccion_servidor; // Dirección del servidor
    char buffer[BUFFER_SIZE]; // Cadena de texto para enviar y recibir datos

    printf("Creando socket del cliente...\n");
    // Crear socket del cliente
    cliente_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (cliente_socket < 0) {
        perror("Error al crear el socket del cliente");
        fclose(entrada);
        fclose(salida);
        exit(EXIT_FAILURE);
    }

    // PASO 3: Configuramos la dirección del servidor -----------------------------------------------------
    direccion_servidor.sin_family = AF_INET; // Familia de direcciones IPv4
    direccion_servidor.sin_port = htons(puerto); // Convertir el puerto de cadena a entero
    if (inet_pton(AF_INET, ip_servidor, &direccion_servidor.sin_addr) <= 0) {
        perror("Error en inet_pton");
        close(cliente_socket);
        fclose(entrada);
        fclose(salida);
        exit(EXIT_FAILURE);
    }

    // PASO 4: Conectamos al servidor -----------------------------------------------------------------------
    printf("Conectando al servidor en %s:%d...\n", ip_servidor, puerto);
    // Conectar al servidor
    if (connect(cliente_socket, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor)) < 0) { // Si la función devuelve un valor negativo, hay error
        perror("Error al conectar con el servidor");
        close(cliente_socket);
        fclose(entrada);
        fclose(salida);
        exit(EXIT_FAILURE);
    }

    // PASO 5: Enviar y recibir datos ------------------------------------------------------------------------
    printf("Conexión establecida. Enviando datos...\n");
    while (fgets(buffer, BUFFER_SIZE, entrada)) { // Leer línea por línea del archivo de entrada
        send(cliente_socket, buffer, strlen(buffer)+1, 0); // Enviar la línea al servidor
        memset(buffer, 0, BUFFER_SIZE); // Limpiar el buffer
        recv(cliente_socket, buffer, BUFFER_SIZE, 0); // Recibir la línea convertida a mayúsculas
        fprintf(salida, "%s", buffer); // Escribir la línea en el archivo de salida
    }

    printf("Cerrando conexión y archivos...\n"); // Cerrar la conexión y los archivos
    close(cliente_socket); // Cerrar el socket del cliente
    fclose(entrada);
    fclose(salida);

    printf("Proceso completado.\n");
    return 0;
}
