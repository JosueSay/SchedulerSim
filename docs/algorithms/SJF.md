# Simulación del Algoritmo SJF (Shortest Job First)

El algoritmo **SJF no expropiativo** simula la planificación de procesos seleccionando en cada instante el proceso con la menor duración de ráfaga (`burstTime`) entre los que ya han llegado y no han terminado. Una vez seleccionado, el proceso se ejecuta por completo sin interrupciones.

La simulación tiene como objetivo representar gráficamente cómo los procesos se registran, esperan y se ejecutan según el criterio de menor duración, generando eventos que permiten visualizar el comportamiento del planificador y exportar métricas clave.

## Funcionamiento general

La simulación avanza ciclo a ciclo. En cada ciclo, se selecciona el proceso más corto disponible para ejecutarse y se actualizan los estados del resto de procesos. Este algoritmo es **no expropiativo**, lo que significa que, una vez que un proceso comienza a ejecutarse, continuará hasta que finalice, sin ser interrumpido por procesos que lleguen después, incluso si estos tienen menor `burstTime`.

## Inicialización

Antes de comenzar, se configuran los siguientes elementos:

* `currentTime`: tiempo actual de simulación (inicia en 0).
* `completed`: cantidad de procesos que ya han terminado.
* `eventCount`: contador de eventos generados durante la simulación (se reinicia a 0).
* Se emiten eventos `NEW` para todos los procesos cuyo `arrivalTime` sea 0.

Las estructuras recibidas para la simulación:

* **Procesos**: cada uno con atributos como `arrivalTime`, `burstTime`, y otros detalles descritos en `docs/definitions/process.md`.
* **Eventos (TimelineEvent)**: estructura para registrar los estados y acciones de los procesos, explicada en `docs/definitions/event-metrics.md`.
* **Control de simulación (SimulationControl)**: contiene parámetros de control como el quantum, enviado desde el frontend y descrito en `docs/definitions/common.md`.
* **`processCount`**: cantidad de procesos dada la lectura del input de procesos en el txt enviado de frontend.
* **`eventCount`**: cantidad de eventos soportados para el algoritmos (se reinicia en 0 nuevmaente para los escenarios de reinicio de la simulación desde 0).

## Ciclo de simulación

En cada ciclo de tiempo se sigue esta secuencia:

1. **Registro de nuevos procesos (`NEW`)**:
   Se verifica si algún proceso llega en el ciclo actual (`arrivalTime == currentTime`) y se genera el evento correspondiente.

2. **Selección del proceso con menor burst (`SJF`)**:
   Se escanea el conjunto de procesos que ya llegaron (`arrivalTime <= currentTime`) y que no han terminado.
   Entre ellos, se selecciona el de menor `burstTime`.
   Si no hay procesos listos, se avanza el tiempo (`currentTime++`) y se aplica un retardo (`usleep`).

3. **Ejecución del proceso seleccionado**:
   El proceso elegido se ejecuta completamente durante `burstTime` ciclos sin interrupciones.

   * En cada ciclo:

     * Se registran eventos `WAITING` para los demás procesos que ya llegaron pero no están ejecutándose ni han terminado.
     * Se genera un evento `ACCESSED` para el proceso en ejecución.
     * Se incrementa el `currentTime`.
     * Se verifica si nuevos procesos llegan y se marcan con `NEW`.

4. **Finalización del proceso (`TERMINATED`)**:
   Al concluir su ejecución:

   * Se marca el proceso como `TERMINATED`.
   * Se registran sus métricas: `startTime`, `finishTime`, `waitingTime`.
   * Se incrementa el contador de procesos completados.
   * Se exportan sus métricas.

## Finalización

La simulación termina cuando todos los procesos han sido ejecutados completamente (`completed == processCount`).

Finalmente, se invoca `exportSimulationEnd()` para marcar el final de la simulación y se asegura que todos los eventos fueron registrados correctamente para ser visualizados o analizados posteriormente.

## Referencias a documentación

* [docs/definitions/process.md](../definitions/process.md): Estructura de los procesos y sus campos.
* [docs/definitions/event-metrics.md](../definitions/event_metrics.md): Definición y uso de los eventos generados durante la simulación.
* [docs/definitions/common.md](../definitions/common.md): Control y parámetros globales de simulación.
* [docs/protocols/protocolo\_scheduling.md](../protocols/protocolo_scheduling.md): Protocolo general para algoritmos de planificación.
