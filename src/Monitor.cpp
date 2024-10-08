#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <vector>
#include <map>
#include <mutex>

#define PORT 8080

// Colores para la terminal
#define RED     "\033[31m"
#define RESET   "\033[0m"

struct SensorData {
    int temperatura;
    std::string unidadTemp;
    int presion;
    std::string unidadPres;
};

std::map<int, SensorData> sensorData; // Map para almacenar datos de sensores por ID
std::mutex dataMutex;                 // Mutex para sincronizar el acceso a sensorData
int currentSensorId = -1;             // ID del sensor que el usuario desea monitorear

void handleSensorConnection(int new_socket) {
    char buffer[1024] = {0};

    while (true) {
        int valread = read(new_socket, buffer, 1024);
        if (valread > 0) {
            std::string receivedData(buffer);

            // Extraer el ID del sensor, temperatura, y presión
            size_t idPos = receivedData.find("ID: ");
            size_t tempPos = receivedData.find("temperatura: ");
            size_t presPos = receivedData.find("presion: ");

            if (idPos != std::string::npos && tempPos != std::string::npos && presPos != std::string::npos) {
                int sensorId = std::stoi(receivedData.substr(idPos + 4, tempPos - idPos - 5));
                int temperatura = std::stoi(receivedData.substr(tempPos + 13, presPos - tempPos - 24));
                std::string unidadTemp = "°C";
                int presion = std::stoi(receivedData.substr(presPos + 9));
                std::string unidadPres = "hPa";

                // Almacenar los datos en el map sincronizado
                {
                    std::lock_guard<std::mutex> lock(dataMutex);
                    sensorData[sensorId] = {temperatura, unidadTemp, presion, unidadPres};
                }

                // Si el ID actual es el que el usuario desea monitorear, mostrar los datos
                if (sensorId == currentSensorId) {
                    std::cout << "-----------------------------------------" << std::endl;
                    std::cout << "Máquina " << sensorId << std::endl;
                    std::cout << "Temperatura: " << temperatura << " " << unidadTemp << std::endl;
                    std::cout << "Presión: " << presion << " " << unidadPres << std::endl;

                    // Mostrar advertencia en rojo si la temperatura es mayor a 80°C
                    if (temperatura > 80) {
                        std::cout << RED << "¡Peligro! Revisar máquina " << sensorId << " - Temperatura: " << temperatura << "°C" << RESET << std::endl;
                    }
                    std::cout << "-----------------------------------------" << std::endl << std::endl;
                }
            }
        } else {
            break;
        }
    }
    close(new_socket);
}

void userInputThread() {
    while (true) {
        int selectedId;
        std::cout << "Ingrese el ID de la máquina que desea monitorear: ";
        std::cin >> selectedId;

        {
            std::lock_guard<std::mutex> lock(dataMutex);
            currentSensorId = selectedId;
        }

        // Mostrar la temperatura actual del sensor seleccionado (si ya se ha recibido)
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            if (sensorData.find(currentSensorId) != sensorData.end()) {
                SensorData data = sensorData[currentSensorId];
                std::cout << "-----------------------------------------" << std::endl;
                std::cout << "Mostrando datos para Máquina " << currentSensorId << std::endl;
                std::cout << "Temperatura: " << data.temperatura << " " << data.unidadTemp << std::endl;
                std::cout << "Presión: " << data.presion << " " << data.unidadPres << std::endl;

                // Mostrar advertencia en rojo si la temperatura es mayor a 80°C
                if (data.temperatura > 80) {
                    std::cout << RED << "¡Peligro! Revisar máquina " << currentSensorId << " - Temperatura: " << data.temperatura << "°C" << RESET << std::endl;
                }
                std::cout << "-----------------------------------------" << std::endl << std::endl;
            } else {
                std::cout << "No se han recibido datos de la Máquina " << currentSensorId << " aún." << std::endl << std::endl;
            }
        }
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Crear el socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cout << "Error creando socket" << std::endl;
        return -1;
    }

    // Opción para reutilizar el puerto
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cout << "Error en setsockopt" << std::endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Vincular el socket al puerto 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cout << "Error en bind" << std::endl;
        return -1;
    }

    // Escuchar por conexiones entrantes
    if (listen(server_fd, 3) < 0) {
        std::cout << "Error en listen" << std::endl;
        return -1;
    }

    // Crear un thread para manejar la entrada del usuario
    std::thread inputThread(userInputThread);
    inputThread.detach();

    // Aceptar y manejar conexiones entrantes
    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            std::cout << "Error aceptando conexión" << std::endl;
            return -1;
        }

        // Crear un thread para manejar la conexión del sensor
        std::thread sensorThread(handleSensorConnection, new_socket);
        sensorThread.detach();
    }

    return 0;
}
