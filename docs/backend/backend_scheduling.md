# Backend de Simulación – SchedulerSim

Este backend escrito en C es responsable de simular algoritmos clásicos de planificación de procesos. Su arquitectura modular permite elegir entre varios algoritmos como FIFO, SJF, RR, Prioridad y SRT, leyendo procesos desde un archivo y configuraciones desde `stdin` en formato JSON.

## Estructura General

```bash
backend/
├── fifo.h / .c         # FIFO (First In, First Out)
├── sjf.h / .c          # Shortest Job First
├── rr.h / .c           # Round Robin
├── ps.h / .c           # Planificación por Prioridad
├── srt.h / .c          # Shortest Remaining Time
├── simulator.h / .c    # Utilidades generales: métricas, eventos, enums
├── main.c              # Punto de entrada principal
```

## Configuración de Entrada (JSON por stdin)

```json
{
  "algorithm": "RR",
  "quantum": 3,
  "isPreemptive": false
}
```

Se lee usando `cJSON` desde `stdin` mediante la función `readConfigFromStdin()`

## Archivos de Entrada

El binario espera los procesos en el archivo:

```bash
../data/input/procesos.txt
```

Formato por línea:

```bash
<PID>, <BT>, <AT>, <Priority>
```

Ejemplo:

```bash
P1, 5, 0, 2
P2, 3, 1, 1
```

## Lógica del Backend

### main.c

1. Lee la configuración (algoritmo, quantum, modo).
2. Carga los procesos desde archivo.
3. Selecciona el algoritmo solicitado.
4. Ejecuta la simulación y exporta métricas.

### simulator.c

Contiene funciones de utilidad como:

* `calculateMetrics()`
* `exportProcessMetric()`
* `exportEventRealtime()`
* `printEventForProcess()`
* `exportSimulationEnd()`

Todos los eventos se imprimen en `stdout` como JSON para ser leídos por el backend FastAPI en tiempo real.

## Algoritmos Soportados

| Código   | Archivo | Descripción                               |
| -------- | ------- | ----------------------------------------- |
| FIFO     | fifo.c  | Procesos por orden de llegada             |
| SJF      | sjf.c   | Proceso con menor burst, no preventivo    |
| RR       | rr.c    | Round Robin con quantum configurable      |
| PRIORITY | ps.c    | Por prioridad, preventivo o no preventivo |
| SRT      | srt.c   | Shortest Remaining Time (SJF preventivo)  |

## Eventos Generados (JSON)

Durante la simulación se imprimen líneas como:

```json
{ "pid": "P1", "startCycle": 0, "endCycle": 1, "state": "ACCESSED" }
```

Al finalizar, métricas por proceso:

```json
{
  "event": "PROCESS_METRIC",
  "pid": "P1",
  "arrivalTime": 0,
  "burstTime": 5,
  "priority": 2,
  "startTime": 0,
  "endTime": 5,
  "waitingTime": 0
}
```

Y evento final:

```json
{ "event": "SIMULATION_END" }
```

## Métricas Finales

Al concluir se imprime:

```json
{ "type": "metrics", "Average Waiting Time": 4.33 }
```

## Simulación en Tiempo Real

Cada ciclo incluye una pausa artificial con `usleep(SIMULATION_DELAY_US);` para simular el paso del tiempo.
