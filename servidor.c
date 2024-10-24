#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// Definimos el tamaño del buffer para el mensaje de saludo
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {

    int puerto = 0;
    // Si solo se ha introducido un argumento, y el puerto introducido es válido, seleccionamos el puerto introducido
    if (argc == 2 && atoi(argv[1]) > 0 && atoi(argv[1]) < 65536) {
        printf("Puerto: %s\n", argv[1]);
        puerto = atoi(argv[1]);
    // En caso contrario, seleccionamos un puerto por defecto
    } else {
        printf("Usando el puerto 50000 por defecto\n");
        puerto = 50000;
    }

    // PASO 0: Creamos las variables ---------------------------------------------------------------------

    int socketServidor, socketCliente; // Variables para los sockets de servidor y conexión
    struct sockaddr_in direccionServidor, direccionCliente; // Struct para la dirección IP
    socklen_t tamanoCliente = sizeof(direccionCliente); // Variable para el tamaño de la dirección del cliente
    char buffer[BUFFER_SIZE] = "¡Hola, cliente! Bienvenido al servidor.\n"; // String para el mensaje de saludo del servidor

    /// PASO 1: Creamos el socket del servidor ------------------------------------------------------------

    socketServidor = socket(AF_INET, SOCK_STREAM, 0); // AF_INET para IPv4, SOCK_STREAM para protocolo TCP
    if (socketServidor < 0) { // Si la función devuelve un valor negativo, significa que hubo un error en la creación del socket
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }

    // PASO 2: Configuramos la dirección del servidor ----------------------------------------------------

    direccionServidor.sin_family = AF_INET; // Familia de direcciones IPv4
    direccionServidor.sin_addr.s_addr = INADDR_ANY; // Escuchar en todas las interfaces
    direccionServidor.sin_port = htons(puerto); // Convierte el puerto pasado por línea de comandos a formato de red
    
    // Asociamos el socket con la dirección y puerto
    if (bind(socketServidor, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) < 0) { // Si devuelve un negativo, hay error
        perror("Error al hacer bind en el socket del servidor");
        close(socketServidor); // Cerramos el socket
        exit(EXIT_FAILURE);
    }

    // PASO 3. Marcamos el socket como pasivo, para que pueda escuchar peticiones ------------------------

    if (listen(socketServidor, 5) < 0) { // El 5 es el número máximo de solicitudes en espera. Si la función devuelve un negativo, hay error
        perror("Error al hacer listen en el socket del servidor");
        close(socketServidor); // Cerramos el socket si hay error
        exit(EXIT_FAILURE);
    }

    printf("Servidor esperando conexiones en el puerto %d...\n", puerto); //Imprimimos para que se sepa, efectivamente, que el servidor está esperando conexiones en el puerto

    // PASO 4. Aceptamos la conexión ---------------------------------------------------------------------

    while (1) { // Para que el servidor atienda múltiples conexiones de clientes

        // Aceptamos la conexión del cliente

        socketCliente = accept(socketServidor, (struct sockaddr *)&direccionCliente, &tamanoCliente);
        if (socketCliente < 0) { // Si hay error, informamos y cerramos el socket del servidor
            perror("Error al aceptar la conexión");
            close(socketServidor);
            exit(EXIT_FAILURE);
        }

        // Obtenemos la IP y puerto del cliente

        char ipCliente[INET_ADDRSTRLEN]; // Cadena de caracteres para almacenar la IP del cliente
        inet_ntop(AF_INET, &direccionCliente.sin_addr, ipCliente, INET_ADDRSTRLEN); // Convertimos la dirección IP del cliente a texto legible
        printf("Conexión aceptada de %s:%d\n", ipCliente, ntohs(direccionCliente.sin_port)); // Mostramos la IP y puerto del cliente. ntohs() convierte el puerto de formato de red a formato de host.

        // Enviamos mensaje de saludo al cliente

        ssize_t enviado = send(socketCliente, buffer, strlen(buffer)+1, 0);
        if (enviado < 0) {
            perror("Error al enviar el mensaje al cliente");
        } else {
            printf("Mensaje enviado al cliente (%ld bytes)\n", enviado);
        }

        // Cerramos el socket de conexión con el cliente

        close(socketCliente);
    }

    // Cerramos el socket del servidor
    close(socketServidor);

    return 0;
}
