#include "Monitor.h"
#include "Sensor.h"
#include <iostream>

int main() {
    Monitor monitor;
    
    Sensor sensorTemperatura("Temperatura", 1);
    Sensor sensorPresion("Presión", 2);
    Sensor sensorVelocidad("Velocidad", 3);

    monitor.agregarSensor(&sensorTemperatura);
    monitor.agregarSensor(&sensorPresion);
    monitor.agregarSensor(&sensorVelocidad);

    sensorTemperatura.iniciar();
    sensorPresion.iniciar();
    sensorVelocidad.iniciar();

    monitor.iniciarMonitoreo();

    std::this_thread::sleep_for(std::chrono::seconds(10));

    sensorTemperatura.detener();
    sensorPresion.detener();
    sensorVelocidad.detener();
    monitor.detenerMonitoreo();

    return 0;
}
