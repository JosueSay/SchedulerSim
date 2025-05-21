# Simulación del Algoritmo RR (Round Robin)

El algoritmo Round Robin (RR) simula la planificación de procesos asignando a cada uno un intervalo fijo de tiempo llamado **quantum**, en orden circular. A diferencia de FIFO, los procesos pueden ser interrumpidos si exceden el quantum y luego reinsertados al final de la cola de ejecución.

El objetivo es simular una planificación **justa y equitativa** en la que todos los procesos tengan oportunidad de ejecutarse sin bloquear a los demás por largos períodos. Durante la simulación se registran eventos que describen los estados de los procesos en el tiempo, y se exportan métricas de ejecución y espera.

## Funcionamiento general

La simulación avanza en ciclos de tiempo discretos, uno por uno. En cada ciclo se verifica si hay nuevos procesos por agregar, se selecciona el siguiente proceso a ejecutar según la cola RR, y se actualizan los estados y eventos correspondientes.

## Inicialización

Se preparan las variables base para la simulación:

* Se define el **tiempo actual** de simulación (`currentTime`), comenzando en cero.
* Se inicializa un contador de procesos **completados**.
* Se crea una **cola de procesos** para seguir el orden RR de ejecución.
* Se define el proceso actualmente en ejecución (`currentProcess`), que al inicio es ninguno (`-1`).
* Se inicializa un arreglo que registra el **tiempo restante** (`remainingBurst`) de cada proceso.
* Se configura el valor del **quantum** a partir del parámetro recibido desde `control`.
* Se reinicia el contador de eventos (`eventCount = 0`) para registrar desde cero.

Las estructuras recibidas para la simulación:

* **Procesos**: cada uno con atributos como `arrivalTime`, `burstTime`, y otros detalles descritos en `docs/definitions/process.md`.
* **Eventos (TimelineEvent)**: estructura para registrar los estados y acciones de los procesos, explicada en `docs/definitions/event-metrics.md`.
* **Control de simulación (SimulationControl)**: contiene parámetros de control como el quantum, enviado desde el frontend y descrito en `docs/definitions/common.md`.
* **`processCount`**: cantidad de procesos dada la lectura del input de procesos en el txt enviado de frontend.
* **`eventCount`**: cantidad de eventos soportados para el algoritmos (se reinicia en 0 nuevmaente para los escenarios de reinicio de la simulación desde 0).

Se inicializa el arreglo `remainingBurst` con los tiempos `burstTime` de cada proceso. Todos los procesos comienzan con estado `STATE_NEW`.

## Ciclo de simulación

En cada ciclo de tiempo, se realiza lo siguiente:

1. **Ingreso de nuevos procesos**: Se revisa si algún proceso tiene `arrivalTime == currentTime` y sigue en estado `STATE_NEW`. Si es así:

   * Se registra un evento `NEW`.
   * Se cambia su estado a `STATE_WAITING`.
   * Se agrega a la **cola de ejecución**.

2. **Registro de procesos en espera**: Para todos los procesos en la cola que **no están en ejecución**, se registra un evento `WAITING` indicando que están en cola pero aún no les toca.

3. **Cambio de proceso por quantum agotado**:

   * Si el proceso actual ya consumió su quantum y no ha terminado, se reinserta al final de la cola.
   * Si la cola no está vacía, se toma el siguiente proceso para ejecutarse.
   * Si el proceso no ha comenzado aún (`startTime == -1`), se registra su inicio.

4. **Estado de espera implícito**: Para procesos que han iniciado (`startTime != -1`), aún tienen ráfaga pendiente y **no están ejecutando en este ciclo**, se registra un evento `WAITING`.

5. **Ejecución del proceso activo**:

   * Se registra un evento `ACCESSED`.
   * Se reduce en 1 el tiempo restante (`remainingBurst`) y se incrementa el contador del quantum.
   * Si el proceso finaliza (`remainingBurst == 0`), se marca como `STATE_TERMINATED`:

     * Se calcula su tiempo de finalización y espera.
     * Se exportan sus métricas.
     * Se elimina de la ejecución actual.

6. **Avance del tiempo**: Se incrementa `currentTime` en uno y se hace una pausa con `usleep` para simular el paso del tiempo real.

## Finalización

La simulación continúa hasta que todos los procesos han sido completados. Una vez finalizado, se exportan los resultados de la simulación y se genera un evento global de finalización.

## Referencias a documentación

* Estructura del proceso: [docs/definitions/process.md](../definitions/process.md)
* Definición de eventos en timeline: [docs/definitions/event-metrics.md](../definitions/event_metrics.md)
* Control de simulación (SimulationControl): [docs/definitions/common.md](../definitions/common.md)
* Protocolo para algoritmos de planificación: [docs/protocols/protocolo\_scheduling.md](../protocols/protocolo_scheduling.md)
* Video explicativo de referencia: [ALGORITMO DE PLANIFICACIÓN ROUND ROBIN DE LA CPU PARA LA ASIGNACIÓN DE PROCESOS DEL SO](https://www.youtube.com/watch?v=YNBKR8eQJnE)
