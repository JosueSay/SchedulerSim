# Backend de Simulación – SyncSim

Este backend en C implementa la simulación de mecanismos de sincronización entre procesos usando **mutex** o **semáforos**. Procesa archivos de entrada con procesos, recursos y acciones, y emite eventos JSON en tiempo real para representar el acceso y bloqueo de recursos.

## Estructura General

```bash
backend/
├── synchronization.c       # Simulación principal
├── sync_mutex.c / .h       # Implementación de mutex
├── sync_semaphore.c / .h   # Implementación de semáforo
├── simulator.c / .h        # Utilidades generales: métricas, eventos, estados
```

## Configuración de Entrada (JSON por stdin)

```json
{
  "mechanism": "mutex"
}
```

La variable `useMutex` controla si se usa mutex (`1`) o semáforo (`0`) durante la simulación.

## Archivos de Entrada

Los archivos deben estar en `../data/input/` y se leen mediante funciones auxiliares del backend:

* `procesos.txt`
* `recursos.txt`
* `acciones.txt`

Formato de acciones:

```bash
<PID>, <ACTION>, <RECURSO>, <CICLO>
```

Ejemplo:

```bash
A, READ, R1, 0
B, WRITE, R2, 3
```

## Lógica del Backend

### Función principal

La función `simulateSynchronization()`:

1. Lee la configuración y archivos de entrada.
2. Ciclo a ciclo, evalúa qué procesos pueden ejecutar sus acciones según los recursos disponibles.
3. Aplica `acquireMutex()` o `acquireSemaphore()` según el modo.
4. Registra eventos: `NEW`, `WAITING`, `ACCESSED`, `TERMINATED`, `OMITED`.
5. Detecta deadlock si no hay progreso y hay procesos esperando.
6. Calcula métricas y finaliza con un mensaje `SIMULATION_END`.

### Utilidades

`simulator.c` incluye funciones como:

* `printEventForSyncProcess()`: genera eventos por ciclo
* `exportProcessMetricWithOriginalBT()`: métricas con burst time original
* `calculateMetrics()`: tiempo de espera promedio
* `exportSimulationEnd()`: mensaje de cierre

## Mecanismos de Sincronización

| Mecanismo | Archivo            | Comportamiento                        |
| --------- | ------------------ | ------------------------------------- |
| Mutex     | `sync_mutex.c`     | Solo un proceso accede al recurso     |
| Semáforo  | `sync_semaphore.c` | Permite múltiples accesos si hay cupo |

Funciones:

* `acquireMutex()` / `releaseMutex()`
* `acquireSemaphore()` / `releaseSemaphore()`

## Eventos Generados (JSON)

Eventos por acción:

```json
{
  "pid": "A",
  "startCycle": 0,
  "endCycle": 1,
  "state": "ACCESSED",
  "action": "READ"
}
```

Métricas por proceso:

```json
{
  "event": "PROCESS_METRIC",
  "pid": "A",
  "arrivalTime": 0,
  "burstTime": 5,
  "burstTimeConsumed": 5,
  "burstTimePending": 0,
  "priority": 1,
  "startTime": 0,
  "endTime": 5,
  "waitingTime": 1
}
```

Finalización:

```json
{ "event": "SIMULATION_END" }
```

## Manejo de Deadlocks

Si no hay progreso y hay procesos en estado `WAITING`, se declara un deadlock. Los procesos involucrados se marcan como `OMITED` y se genera una advertencia por `stderr`.

## Simulación en Tiempo Real

La simulación avanza ciclo por ciclo con pausas de tiempo real mediante `usleep(SIMULATION_DELAY_US)`, permitiendo una visualización animada de los estados.
