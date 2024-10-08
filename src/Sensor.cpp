#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define PORT 8080

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        std::cerr << "Error: ID del sensor no proporcionado." << std::endl;
        return -1;
    }

    // Mostrar el ID recibido
    std::cout << "ID del sensor recibido: " << argv[1] << std::endl;

    int sock = 0;
    struct sockaddr_in serv_addr;
    int sensorId;

    try {
        sensorId = std::stoi(argv[1]); // Intentar convertir el ID a entero
    } catch (std::invalid_argument& e) {
        std::cerr << "Error: Argumento inválido para ID del sensor (" << argv[1] << ")" << std::endl;
        return -1;
    }

    // Inicializar random para las lecturas
    srand(time(0));

    // Crear el socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "Error creando socket" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convertir dirección IP a binario
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cout << "Dirección IP inválida" << std::endl;
        return -1;
    }

    // Conectar al monitor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Error conectando al monitor" << std::endl;
        return -1;
    }

    while (true) {
        // Generar nuevas lecturas aleatorias
        int valorTemperatura = rand() % 100; // Temperatura en °C
        int valorPresion = 950 + rand() % 100; // Presión en hPa

        // Crear el mensaje con la identificación única del sensor, temperatura y presión
        std::string data = "ID: " + std::to_string(sensorId) +
                           " temperatura: " + std::to_string(valorTemperatura) + 
                           " unidadTemp: °C presion: " + std::to_string(valorPresion) + 
                           " unidadPres: hPa";

        // Enviar el mensaje al monitor
        send(sock, data.c_str(), data.length(), 0);

        // Esperar 5 segundos antes de la próxima lectura
        sleep(5);
    }

    // Cerrar el socket
    close(sock);
    return 0;
}
