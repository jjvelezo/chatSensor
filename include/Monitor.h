#ifndef MONITOR_H
#define MONITOR_H

#include <vector>
#include "Sensor.h"

class Monitor {
public:
    Monitor();
    void agregarSensor(Sensor* sensor);
    void iniciarMonitoreo();
    void detenerMonitoreo();

private:
    void monitorearSensores();
    
    std::vector<Sensor*> sensores;
    bool enFuncionamiento;
    std::thread hiloMonitor;
};

#endif // MONITOR_H
