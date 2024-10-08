# Declarar reglas como phony
.PHONY: all clean run-monitor run-sensor

# Directorios
INCLUDE_DIR = include
SRC_DIR = src
BUILD_DIR = build

# Archivos fuente y objetos
SOURCES = $(SRC_DIR)/Monitor.cpp $(SRC_DIR)/Sensor.cpp $(SRC_DIR)/main.cpp $(SRC_DIR)/Mensaje.cpp $(SRC_DIR)/Usuario.cpp
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

# Flags de compilación
CXXFLAGS = -I$(INCLUDE_DIR) -lpthread

# Archivo que guarda el ID del próximo sensor
SENSOR_ID_FILE = .sensor_id

# Inicializar el ID del sensor
$(SENSOR_ID_FILE):
	echo 0 > $(SENSOR_ID_FILE)

# Incrementar el ID del sensor y devolverlo
next-sensor-id:
	@id=$$(cat $(SENSOR_ID_FILE)); \
	new_id=$$((id + 1)); \
	echo $$new_id > $(SENSOR_ID_FILE); \
	echo $$new_id

# Compilar todo
all: monitor sensor

# Compilar el monitor
monitor: $(BUILD_DIR)/Monitor.o $(BUILD_DIR)/Mensaje.o $(BUILD_DIR)/Usuario.o
	$(CXX) -o monitor $(BUILD_DIR)/Monitor.o $(BUILD_DIR)/Mensaje.o $(BUILD_DIR)/Usuario.o $(CXXFLAGS)

# Compilar un sensor
sensor: $(BUILD_DIR)/Sensor.o $(BUILD_DIR)/Mensaje.o $(BUILD_DIR)/Usuario.o
	$(CXX) -o sensor $(BUILD_DIR)/Sensor.o $(BUILD_DIR)/Mensaje.o $(BUILD_DIR)/Usuario.o $(CXXFLAGS)

# Compilar archivos .cpp a .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Limpiar archivos compilados
clean:
	rm -f $(BUILD_DIR)/*.o monitor sensor $(SENSOR_ID_FILE)

# Ejecutar el monitor
run-monitor: monitor
	./monitor

# Ejecutar un sensor (con asignación incremental de ID)
run-sensor: sensor $(SENSOR_ID_FILE)
	@id=$$(id=$$(cat $(SENSOR_ID_FILE)); new_id=$$((id + 1)); echo $$new_id > $(SENSOR_ID_FILE); echo $$new_id); \
	echo "Ejecutando sensor con ID: $$id"; \
	./sensor $$id
