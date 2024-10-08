#ifndef SENSOR_H
#define SENSOR_H

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

class Sensor {
public:
    Sensor(const std::string& tipo, int id);
    void iniciar();
    void detener();
    double obtenerLectura();

private:
    void generarLecturas();
    
    std::string tipoSensor;
    int idSensor;
    bool enFuncionamiento;
    double valorActual;
    std::thread hiloSensor;
    std::mutex mutexValor;
    std::condition_variable cv;
};

#endif // SENSOR_H
