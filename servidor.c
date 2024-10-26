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
    // Si solo se ha introducido un argumento (dos contando el nombre del ejecutable), y el puerto introducido es válido, seleccionamos el puerto introducido
    if (argc == 2 && atoi(argv[1]) > 0 && atoi(argv[1]) < 65536) { // atoi convierte a entero. Nos aseguramos de que el puerto introducido sea válido.
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

    printf("Creando socket del servidor...\n");
    socketServidor = socket(AF_INET, SOCK_STREAM, 0); // AF_INET para IPv4, SOCK_STREAM para conexiones orientadas (protocolo TCP), 0 para seleccionar el protocolo adecuado para el socket elegido (con nuestros parámetros, será TCP).
    if (socketServidor < 0) { // Si la función devuelve un valor negativo, significa que hubo un error en la creación del socket
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }

    // PASO 2: Configuramos la dirección del servidor ----------------------------------------------------

    printf("Configurando la dirección del servidor...\n");
    direccionServidor.sin_family = AF_INET; // Familia de direcciones IPv4
    direccionServidor.sin_addr.s_addr = INADDR_ANY; // Escuchar en todas las interfaces
    direccionServidor.sin_port = htons(puerto); // Convierte el puerto pasado por línea de comandos a formato de red
    
    // Asociamos el socket con la dirección y puerto

    printf("Asociando el socket con la dirección y puerto...\n");
    if (bind(socketServidor, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) < 0) { // Si devuelve un negativo, hay error
        // socketServidor: socket previamente configurado
        // (struct sockaddr *)&direccionServidor: puntero a sockaddr_in con la dirección y puerto del servidor. EL cast es porque el bind() espera un puntero a struct sockaddr; lo convertimos.
        // sizeof(direccionServidor): tamaño en bytes de la estructura de la dirección
        // La función asocia el socket a una dirección específica (es decir, IP y puerto)
        
        perror("Error al hacer bind en el socket del servidor");
        close(socketServidor); // Cerramos el socket
        exit(EXIT_FAILURE);
    }

    // PASO 3. Marcamos el socket como pasivo, para que pueda escuchar peticiones ------------------------

    printf("Poniendo el socket en modo de escucha...\n");
    if (listen(socketServidor, 5) < 0) { // El 5 es el número máximo de solicitudes en espera. Si la función devuelve un negativo, hay error. socketServidor es el socket del servidor, trivialmente.
        perror("Error al hacer listen en el socket del servidor");
        close(socketServidor); // Cerramos el socket si hay error
        exit(EXIT_FAILURE);
    }

    printf("Servidor esperando conexiones en el puerto %d\n", puerto); //Imprimimos para que se sepa, efectivamente, que el servidor está esperando conexiones en el puerto

    // PASO 4. Aceptamos la conexión ---------------------------------------------------------------------

    while (1) { // Para que el servidor atienda múltiples conexiones de clientes

        // Aceptamos la conexión del cliente

        printf("Esperando una conexión...\n");
        socketCliente = accept(socketServidor, (struct sockaddr *)&direccionCliente, &tamanoCliente);
            // socketServidor: socket del servidor que escucha
            // (struct sockaddr *)&direccionCliente: estructura para almacenar la dirección del cliente.
            // &tamanoCliente: tamaño en bytes de la estructura de la dirección del cliente.
        if (socketCliente < 0) { // Si hay error, informamos y cerramos el socket del servidor
            perror("Error al aceptar la conexión");
            close(socketServidor);
            exit(EXIT_FAILURE);
        }

        // Obtenemos la IP y puerto del cliente

        char ipCliente[INET_ADDRSTRLEN]; // Cadena de caracteres para almacenar la IP del cliente
        inet_ntop(AF_INET, &direccionCliente.sin_addr, ipCliente, INET_ADDRSTRLEN); // Convertimos la dirección IP del cliente a texto legible
        
            // AF_INET: especifica la familia IPv4
            // &direccionCliente.sin_addr: puntero a la IP en binario
            // ipCliente: puntero a una cadena de caracteres en la que se almacenará la IP textual
            // INET_ADDRSTRLEN: constante que define el tamaño mínimo del buffer para almacenar una dirección IPv4 en formato textual.
        
        int puerto_cliente = ntohs(direccionCliente.sin_port); // Mostramos la IP y puerto del cliente. ntohs() convierte el puerto de formato de red a formato de host.
        printf("Conexión aceptada desde %s:%d\n", ipCliente, puerto_cliente);
        
        // Enviamos mensaje de saludo al cliente

        ssize_t enviado = send(socketCliente, buffer, strlen(buffer)+1, 0); // send envía datos al cliente conectado.
            // socketCliente: socket cliente
            // buffer: es el mensaje a enviar
            // strlen(buffer)+1: longitud del mensaje en bytes, el +1 es para incluir el caracter nulo \0
            // 0: funcionamiento por defecto

        if (enviado < 0) {
            perror("Error al enviar el mensaje al cliente");
        } else {
            printf("Mensaje enviado al cliente (%ld bytes)\n", enviado);
        }

        // Cerramos el socket de conexión con el cliente
        printf("Cerrando conexión con el cliente...\n");
        close(socketCliente);
    }

    // Cerramos el socket del servidor
    printf("Cerrando el socket del servidor...\n");
    close(socketServidor);
    return 0;
}
