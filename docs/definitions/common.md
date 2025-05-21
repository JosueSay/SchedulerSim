
# Definiciones Comunes

Este archivo contiene definiciones, constantes y funciones comunes utilizadas en el simulador de procesos, que permiten gestionar datos de entrada, configuración general y resultados.

## Constantes Principales

| Constante             | Valor               | Descripción                                                                                                     |
| --------------------- | ------------------- | --------------------------------------------------------------------------------------------------------------- |
| `COMMON_MAX_LEN`      | 10                  | Tamaño máximo estándar para cadenas de texto, como IDs de procesos o nombres de recursos.                       |
| `MAX_PROCESSES`       | 100                 | Número máximo de procesos que el simulador puede manejar. Limitado para controlar uso de memoria y rendimiento. |
| `MAX_EVENTS`          | 500                 | Número máximo de eventos en la línea de tiempo para la simulación.                                              |
| `SIMULATION_DELAY_US` | 1000000 (1 segundo) | Tiempo de delay en microsegundos entre ciclos o eventos para simular el avance temporal.                        |

**Nota:**
`MAX_PROCESSES` se limita a 100 para equilibrar entre permitir una simulación representativa y evitar un uso excesivo de recursos en sistemas típicos donde se ejecuta el simulador.

## Enumeraciones

### Algoritmos de Planificación (`SchedulingAlgorithm`)

| Valor           | Descripción                 |
| --------------- | --------------------------- |
| `ALGO_FIFO`     | First In First Out          |
| `ALGO_PRIORITY` | Planificación por prioridad |
| `ALGO_RR`       | Round Robin                 |
| `ALGO_SJF`      | Shortest Job First          |
| `ALGO_SRT`      | Shortest Remaining Time     |
| `ALGO_NONE`     | Sin algoritmo asignado      |

### Estados de Proceso (`ProcessState`)

| Estado             | Descripción                   |
| ------------------ | ----------------------------- |
| `STATE_NEW`        | Proceso recién creado         |
| `STATE_WAITING`    | Proceso esperando recurso     |
| `STATE_ACCESSED`   | Proceso ejecutándose o activo |
| `STATE_TERMINATED` | Proceso terminado             |

### Tipos de Acción (`ActionType`)

| Acción         | Descripción         |
| -------------- | ------------------- |
| `ACTION_READ`  | Acción de lectura   |
| `ACTION_WRITE` | Acción de escritura |

## Estructuras Principales

* **Process**: Representa un proceso con sus atributos y métricas para la simulación.
* **Resource**: Representa un recurso que puede ser bloqueado o liberado para sincronización.
* **Action**: Representa una acción sobre un recurso en un ciclo determinado.
* **TimelineEvent**: Evento en la línea de tiempo para registro y visualización (Gantt).
* **SimulationConfig**: Configuración de la simulación (algoritmo, quantum, preemptividad).
* **SimulationMetrics**: Métricas calculadas después de la simulación (ej. tiempo promedio de espera).

## Funciones Comunes para Carga de Datos

Estas funciones leen archivos de texto con formato CSV para cargar las estructuras necesarias en la simulación:

### `int loadProcesses(const char *filename, Process *processes, int maxProcesses)`

* Carga procesos desde un archivo.
* Formato esperado por línea:
  `PID, burstTime, arrivalTime, priority`
* Inicializa estados y métricas de cada proceso.
* Retorna número de procesos cargados o -1 si error.

### `int loadResources(const char *filename, Resource *resources, int maxResources)`

* Carga recursos desde un archivo.
* Formato esperado por línea:
  `resourceName, counter`
* Inicializa el estado de bloqueo como libre.
* Retorna número de recursos cargados o -1 si error.

### `int loadActions(const char *filename, Action *actions, int maxActions)`

* Carga acciones desde un archivo.
* Formato esperado por línea:
  `PID, ACTION (READ/WRITE), resourceName, cycle`
* Convierte cadena ACTION a enum interno.
* Retorna número de acciones cargadas o -1 si error.

## Funciones Utilitarias de Conversión

Estas funciones convierten enums a cadenas legibles, para mostrar resultados o imprimir logs:

* `const char *getProcessStateName(ProcessState state);`
* `const char *getActionTypeName(ActionType action);`
* `const char *getAlgorithmName(SchedulingAlgorithm algorithm);`
