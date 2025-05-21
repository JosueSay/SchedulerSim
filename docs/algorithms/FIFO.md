# Simulación del Algoritmo FIFO (First In, First Out)

El algoritmo FIFO simula la planificación de procesos atendiendo cada proceso estrictamente en orden de llegada, según su tiempo `arrivalTime`. Cada proceso debe ejecutarse completamente por su duración `burstTime` antes de pasar al siguiente.

El objetivo es reflejar en la simulación cómo los procesos ingresan, esperan y se ejecutan, registrando eventos que describen el estado de cada proceso durante la simulación, así como exportar métricas y el estado final.

## Funcionamiento general

El proceso de simulación se organiza en ciclos de tiempo que avanzan uno a uno. En cada ciclo se actualizan los estados de los procesos y se generan eventos para registrar lo que ocurre en ese instante.

## Inicialización

Se preparan las variables base para la simulación:

* Se define el tiempo actual de simulación, que comienza en cero.
* Se inicializa un contador para procesos completados.
* Se define el índice del proceso que se está ejecutando actualmente (inicialmente ninguno).
* Se crea un arreglo que registra el tiempo restante que le queda a cada proceso para terminar su ejecución.
* Se reinicia el contador de eventos para registrar la simulación.

Las estructuras recibidas para la simulación:

* **Procesos**: cada uno con atributos como `arrivalTime`, `burstTime`, y otros detalles descritos en `docs/definitions/process.md`.
* **Eventos (TimelineEvent)**: estructura para registrar los estados y acciones de los procesos, explicada en `docs/definitions/event-metrics.md`.
* **Control de simulación (SimulationControl)**: contiene parámetros de control como el quantum, enviado desde el frontend y descrito en `docs/definitions/common.md`.
* **`processCount`**: cantidad de procesos dada la lectura del input de procesos en el txt enviado de frontend.
* **`eventCount`**: cantidad de eventos soportados para el algoritmos (se reinicia en 0 nuevmaente para los escenarios de reinicio de la simulación desde 0).

Antes de iniciar, se asigna a cada proceso su tiempo de ejecución restante (igual a `burstTime`) y se marca que aún no ha comenzado (`startTime` inicializado en -1).

## Ciclo de simulación

En cada ciclo de tiempo se realiza lo siguiente:

1. **Registro de nuevos procesos**: Se marca con un evento `NEW` a cada proceso que llega en ese instante, es decir, cuyo `arrivalTime` coincide con el tiempo actual.

2. **Selección del proceso a ejecutar**: Si no hay un proceso en ejecución, se busca el siguiente proceso listo para ejecutarse. Se selecciona el proceso con menor `arrivalTime` que aún no ha comenzado ni terminado, respetando el orden FIFO.

3. **Registro de procesos en espera**: Todos los procesos que han llegado, no están terminados y no están en ejecución se registran con un evento `WAITING` para reflejar que están listos pero esperando su turno.

4. **Ejecución del proceso activo**: El proceso seleccionado se ejecuta un ciclo, se registra con un evento `ACCESSED` y se decrementa su tiempo restante.

5. **Terminación de proceso**: Si un proceso termina su ráfaga (`remainingBurst` llega a cero), se registra un evento `TERMINATED`, se calculan métricas como tiempo de espera y se incrementa el contador de procesos completados.

6. **Avance del tiempo**: El tiempo de simulación se incrementa en uno y se hace una pausa corta para simular el paso del tiempo real.

## Finalización

La simulación continúa ciclo a ciclo hasta que todos los procesos han terminado su ejecución.

Al final, se exportan las métricas finales de cada proceso y se genera un evento indicando el término de la simulación.

## Referencias a documentación

* La estructura de un proceso y sus atributos está detallada en [docs/definitions/process.md](../definitions/process.md).
* La definición y uso de los eventos de timeline se encuentra en [docs/definitions/event-metrics.md](../definitions/event_metrics.md).
* La estructura y funciones del control de simulación están descritas en [docs/definitions/common.md](../definitions/common.md).
* Protocolo efectuado para algoritmos de calendarización en [docs/protocols/protocolo_scheduling.md](../protocols/protocolo_scheduling.md).
