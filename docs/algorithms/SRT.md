# Simulación del Algoritmo SRT (Shortest Remaining Time)

El algoritmo **SRT** es una variante con desalojo del clásico **SJF (Shortest Job First)**. En esta planificación, se elige en cada ciclo el proceso con el **menor tiempo restante de ejecución**, considerando solo aquellos que ya han llegado. Esta estrategia permite reaccionar dinámicamente a la llegada de nuevos procesos más cortos, desplazando al proceso en ejecución si es necesario.

El objetivo de esta simulación es representar cómo se comporta un planificador **SRT** en tiempo real, mostrando cómo los procesos son seleccionados, esperan, ejecutan y finalizan, además de registrar métricas y eventos asociados a cada estado.

## Funcionamiento general

La simulación se ejecuta en **ciclos de tiempo** que avanzan segundo a segundo. En cada ciclo se:

* Detectan procesos que acaban de llegar.
* Se elige el proceso con menor tiempo restante.
* Se registra el estado actual de cada proceso.
* Se simula la ejecución de un ciclo para el proceso más corto disponible.

## Inicialización

Se preparan las siguientes estructuras y variables base:

* `currentTime`: tiempo actual de la simulación (comienza en 0).
* `completed`: contador de procesos completados.
* `remainingTime[]`: arreglo donde se almacena el tiempo restante de ejecución de cada proceso.
* `eventCount`: contador de eventos registrados, reinicializado a 0 para cada simulación.
* Cada proceso se inicializa con:

  * `startTime = -1` para indicar que aún no ha comenzado.
  * `remainingTime = burstTime` para determinar el tiempo total de ejecución restante.

Las estructuras recibidas para la simulación:

* **Procesos**: cada uno con atributos como `arrivalTime`, `burstTime`, y otros detalles descritos en `docs/definitions/process.md`.
* **Eventos (TimelineEvent)**: estructura para registrar los estados y acciones de los procesos, explicada en `docs/definitions/event-metrics.md`.
* **Control de simulación (SimulationControl)**: contiene parámetros de control como el quantum, enviado desde el frontend y descrito en `docs/definitions/common.md`.
* **`processCount`**: cantidad de procesos dada la lectura del input de procesos en el txt enviado de frontend.
* **`eventCount`**: cantidad de eventos soportados para el algoritmos (se reinicia en 0 nuevmaente para los escenarios de reinicio de la simulación desde 0).

Si un proceso tiene `arrivalTime = 0`, se registra inmediatamente un evento de tipo `NEW`.

## Ciclo de simulación

Durante cada segundo de simulación se ejecutan los siguientes pasos:

1. **Registro de nuevos procesos**
   Se registra un evento `NEW` para cada proceso cuyo `arrivalTime` coincide con `currentTime`.

2. **Selección del proceso con menor tiempo restante**
   Se recorre la lista de procesos para encontrar el proceso que:

   * Ya haya llegado (`arrivalTime <= currentTime`)
   * No esté terminado (`STATE_TERMINATED`)
   * Aún tenga tiempo restante (`remainingTime > 0`)
   * Y que tenga el **menor** `remainingTime`

   Si no hay procesos disponibles, el tiempo simplemente avanza en 1 unidad y se realiza una pausa con `usleep`.

   En caso de **empate** entre varios procesos con el mismo `remainingTime`, se aplican las siguientes reglas de desempate:

      1. Se continúa ejecutando el **proceso que ya estaba en ejecución** (si aplica).
      2. Si ninguno de los empatados estaba ejecutándose previamente, se selecciona el proceso con **mayor prioridad** (valor de `priority` más bajo).

3. **Registro de procesos en espera**
   Todos los procesos activos (ya llegaron, no terminados, con tiempo restante) que **no** están en ejecución en ese ciclo, se marcan como `WAITING`.

4. **Ejecución del proceso más corto**
   El proceso seleccionado se marca con un evento `ACCESSED` y ejecuta 1 unidad de tiempo. Si es la primera vez que se ejecuta, se registra su `startTime`.

5. **Terminación del proceso**
   Si el `remainingTime` del proceso ejecutado llega a 0, se considera terminado:

   * Se marca con `STATE_TERMINATED`.
   * Se calcula su `finishTime` y `waitingTime`.
   * Se registra en métricas.
   * Se incrementa el contador de procesos completados.

6. **Avance del tiempo**
   Se incrementa `currentTime` y se introduce una pausa (`usleep`) para simular tiempo real.

## Finalización

El ciclo continúa hasta que todos los procesos hayan sido completados (`completed == processCount`).

Una vez finalizada la simulación:

* Se exportan las métricas individuales de cada proceso (`exportProcessMetric`).
* Se genera un evento que indica el término de la simulación (`exportSimulationEnd`).

## Referencias a documentación

* Estructura del proceso: [docs/definitions/process.md](../definitions/process.md)
* Definición de eventos en timeline: [docs/definitions/event-metrics.md](../definitions/event_metrics.md)
* Control de simulación (SimulationControl): [docs/definitions/common.md](../definitions/common.md)
* Protocolo para algoritmos de planificación: [docs/protocols/protocolo\_scheduling.md](../protocols/protocolo_scheduling.md)
* Video explicativo de referencia: [ALGORITMO DE PLANIFICACIÓN SRTF DE LA CPU PARA LA ASIGNACIÓN DE PROCESOS DEL SISTEMA OPERATIVO](https://www.youtube.com/watch?v=UuNPVxJ17j0)
