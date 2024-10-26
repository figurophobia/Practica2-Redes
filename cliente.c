#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Definimos el tamaño del buffer para el mensaje de saludo
#define buffer_SIZE 1024

int main(int argc, char *argv[]) {
    char ip[INET_ADDRSTRLEN]; // Almacena la IP del servidor
    int puerto; // Almacena el puerto del servidor

    // Verificamos los argumentos de línea de comandos
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <IP del servidor> <Puerto>\n", argv[0]); // argv[0] imprime el nombre del programa. Así, indicamos al usuario la sintaxis correcta si quiere introducir IP y puerto al ejecutar
        do {
            printf("Introduce la IP del servidor: ");
            scanf("%s", ip);
            printf("Introduce el puerto (0-65535): ");
            scanf("%d", &puerto);
        } while (puerto < 0 || puerto > 65535); // Nos aseguramos de que los datos de entrada sean válidos
    } else { // Si hay 3 argumentos...
        strcpy(ip, argv[1]); // El segundo es la IP
        puerto = atoi(argv[2]); // El primero es el puerto (atoi lo pasa a entero)
        if (puerto < 0 || puerto > 65535) { // Nos aseguramos de que el puerto sea válido
            do {
                printf("Puerto inválido. Introduce el puerto (0-65535): ");
                scanf("%d", &puerto);
            } while (puerto < 0 || puerto > 65535); // Nos aseguramos de que el puerto sea válido
        }
    }

    // PASO 0: Creamos las variables ---------------------------------------------------------------------
    int sock; // Socket de la conexión
    struct sockaddr_in direccionServidor; // Dirección del servidor
    char buffer[buffer_SIZE]; // Cadena de texto para almacenar datos recibidos
    int bytesRecibidos; // Número de bytes recibidos

    // PASO 1: Creamos el socket del cliente ------------------------------------------------------------
    sock = socket(AF_INET, SOCK_STREAM, 0); // AF_INET para IPv4, SOCK_STREAM para protocolo TCP
    if (sock == -1) { // Si la función devuelve un valor negativo, significa que hubo un error en la creación del socket
        perror("Error al crear el socket");
        return 1;
    }

    // PASO 2: Configuramos la dirección del servidor ----------------------------------------------------
    direccionServidor.sin_family = AF_INET; // AF_INET indica que estamos trabajando con la familia de direcciones IPv4
    direccionServidor.sin_port = htons(puerto); // htons convierte el puerto de entero a formato de red
    if (inet_pton(AF_INET, ip, &direccionServidor.sin_addr) <= 0) { // Convierte la IP al formato binario de red
        // AF_INET: Indica que usamos IPv4
        //ip indica la IP
        //&direccionServidor.sin_addr es un puntero a la estructura en la que se almacenará la IP en binario.
        // SI LA FUNCIÓN DEVUELVE 0, la IP no es válida. SI DEVUELVE -1, hubo un eror.
        perror("Error al convertir la IP");
        close(sock);
        return 1;
    }

    // PASO 3: Conectamos al servidor ---------------------------------------------------------------------
    if (connect(sock, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) == -1) { // Si la función devuelve un valor negativo, hay error
        // sock: socket de la conexión
        // (struct sockaddr*)&direccionServidor es un puntero a la estructura sockaddr que contiene la dirección del servidor. El cast convierte la dirección direccionServidor a un puntero genérico struct sockaddr*, para adecuarse al formato de connect()
        // sizeof(direccionServidor) indica el tamaño (en bytes) de direccionServidor. La función connect() requiere que se indique
        perror("Error al conectar al servidor");
        close(sock); // Cerramos el socket
        return 1;
    }

    // PASO 4: Recibimos datos del servidor ----------------------------------------------------------------
    bytesRecibidos = recv(sock, buffer, buffer_SIZE - 1, 0); // Recibimos datos del servidor
        //sock: socket de la conexión
        //buffer: cadena de caracteres donde se almacenará el mensaje recibido
        //buffer_SIZE: cantidad máxima de bytes. usamos buffer_SIZE-1 para dejar espacio al \0
        // 0: indica que se opera de forma estándar, sin modificaciones
    
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
