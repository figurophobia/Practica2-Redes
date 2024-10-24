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

    int servidor_socket, cliente_socket; // Variables para los sockets de servidor y conexión
    struct sockaddr_in direccion_servidor, direccion_cliente; // Struct para la dirección IP
    socklen_t cliente_len = sizeof(direccion_cliente); // Variable para el tamaño de la dirección del cliente
    char buffer[BUFFER_SIZE] = "¡Hola, cliente! Bienvenido al servidor.\n"; // String para el mensaje de saludo del servidor

    /// PASO 1: Creamos el socket del servidor ------------------------------------------------------------

    servidor_socket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET para IPv4, SOCK_STREAM para protocolo TCP
    if (servidor_socket < 0) { // Si la función devuelve un valor negativo, significa que hubo un error en la creación del socket
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }

    // PASO 2: Configuramos la dirección del servidor ----------------------------------------------------

    direccion_servidor.sin_family = AF_INET; // Familia de direcciones IPv4
    direccion_servidor.sin_addr.s_addr = INADDR_ANY; // Escuchar en todas las interfaces
    direccion_servidor.sin_port = htons(puerto); // Convierte el puerto pasado por línea de comandos a formato de red
    
    // Asociamos el socket con la dirección y puerto
    if (bind(servidor_socket, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor)) < 0) { // Si devuelve un negativo, hay error
        perror("Error al hacer bind en el socket del servidor");
        close(servidor_socket); // Cerramos el socket
        exit(EXIT_FAILURE);
    }

    // PASO 3. Marcamos el socket como pasivo, para que pueda escuchar peticiones ------------------------

    if (listen(servidor_socket, 5) < 0) { // El 5 es el número máximo de solicitudes en espera. Si la función devuelve un negativo, hay error
        perror("Error al hacer listen en el socket del servidor");
        close(servidor_socket); // Cerramos el socket si hay error
        exit(EXIT_FAILURE);
    }

    printf("Servidor esperando conexiones en el puerto %d...\n", puerto); //Imprimimos para que se sepa, efectivamente, que el servidor está esperando conexiones en el puerto

    // PASO 4. Aceptamos la conexión ---------------------------------------------------------------------

    while (1) { // Para que el servidor atienda múltiples conexiones de clientes

        // Aceptamos la conexión del cliente

        cliente_socket = accept(servidor_socket, (struct sockaddr *)&direccion_cliente, &cliente_len);
        if (cliente_socket < 0) { // Si hay error, informamos y cerramos el socket del servidor
            perror("Error al aceptar la conexión");
            close(servidor_socket);
            exit(EXIT_FAILURE);
        }

        // Obtenemos la IP y puerto del cliente

        char cliente_ip[INET_ADDRSTRLEN]; // Cadena de caracteres para almacenar la IP del cliente
        inet_ntop(AF_INET, &direccion_cliente.sin_addr, cliente_ip, INET_ADDRSTRLEN); // Convertimos la dirección IP del cliente a texto legible
        printf("Conexión aceptada de %s:%d\n", cliente_ip, ntohs(direccion_cliente.sin_port)); // Mostramos la IP y puerto del cliente. ntohs() convierte el puerto de formato de red a formato de host.

        // Enviamos mensaje de saludo al cliente

        ssize_t enviado = send(cliente_socket, buffer, strlen(buffer)+1, 0);
        if (enviado < 0) {
            perror("Error al enviar el mensaje al cliente");
        } else {
            printf("Mensaje enviado al cliente (%ld bytes)\n", enviado);
        }

        // Cerramos el socket de conexión con el cliente

        close(cliente_socket);
    }

    // Cerramos el socket del servidor
    close(servidor_socket);

    return 0;
}
