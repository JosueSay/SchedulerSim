# Simulación del Algoritmo PS (Planificación por Prioridad Preventiva y No Preventiva)

El algoritmo PS simula la planificación de procesos atendiendo la prioridad de cada uno. Los procesos se ejecutan según su nivel de prioridad (a menor valor, mayor prioridad). En caso de empate, se utiliza el tiempo de llegada y luego la duración de la ráfaga para decidir el orden.

El algoritmo puede funcionar en dos modos:

* **Preventivo**: permite interrumpir un proceso en ejecución si llega otro proceso con mayor prioridad.
* **No preventivo**: una vez que un proceso inicia su ejecución, continúa hasta completar su ráfaga sin interrupciones.

El objetivo de la simulación es mostrar cómo los procesos ingresan, esperan, se ejecutan y terminan, registrando eventos que describen el estado de cada proceso, y exportando métricas y resultados finales.

## Funcionamiento general

La simulación avanza en ciclos de tiempo discretos (unidades), actualizando el estado de los procesos y registrando eventos que reflejan su evolución.

## Inicialización

Se preparan las variables y estructuras necesarias:

* Se define el tiempo actual de simulación, comenzando en cero.
* Se inicializa el contador de procesos completados.
* Se crea un arreglo para llevar el tiempo restante de ejecución de cada proceso.
* Se obtiene el modo preventivo o no preventivo desde la configuración de control de simulación.
* Se inicializa el contador de eventos.

Las estructuras recibidas para la simulación:

* **Procesos**: cada uno con atributos como `arrivalTime`, `burstTime`, y otros detalles descritos en `docs/definitions/process.md`.
* **Eventos (TimelineEvent)**: estructura para registrar los estados y acciones de los procesos, explicada en `docs/definitions/event-metrics.md`.
* **Control de simulación (SimulationControl)**: contiene parámetros de control como el quantum, enviado desde el frontend y descrito en `docs/definitions/common.md`.
* **`processCount`**: cantidad de procesos dada la lectura del input de procesos en el txt enviado de frontend.
* **`eventCount`**: cantidad de eventos soportados para el algoritmos (se reinicia en 0 nuevmaente para los escenarios de reinicio de la simulación desde 0).

Cada proceso comienza con su ráfaga restante igual a su duración (`burstTime`) y su tiempo de inicio (`startTime`) marcado como no iniciado (-1). Si un proceso llega en el tiempo cero, se genera un evento `NEW` para él.

## Ciclo de simulación

En cada ciclo de tiempo se realiza lo siguiente:

1. **Registro de nuevos procesos**: Se genera un evento `NEW` para cada proceso cuyo tiempo de llegada (`arrivalTime`) coincide con el tiempo actual.

2. **Selección del proceso a ejecutar**: Se elige el proceso listo (llegado y no terminado) con la mayor prioridad (menor valor numérico). En caso de empate, se selecciona el proceso con menor tiempo de llegada, y si persiste empate, el de menor ráfaga.

3. **Registro de procesos en espera**: Todos los procesos que han llegado, no están terminados ni en ejecución, se registran con evento `WAITING` para mostrar que están listos pero esperando.

4. **Ejecución del proceso activo**:

   * Si el modo es **preventivo**, el proceso se ejecuta un ciclo a la vez. Al avanzar el tiempo, se puede cambiar de proceso si llega otro con mayor prioridad.

   * Si el modo es **no preventivo**, el proceso se ejecuta hasta completar su ráfaga sin interrupciones.

   En cada ciclo que un proceso se ejecuta se genera un evento `ACCESSED`.

5. **Terminación del proceso**: Cuando un proceso completa su ráfaga (`remainingBurst` llega a cero), se registra un evento `TERMINATED`, se calculan métricas como tiempo de espera, y se incrementa el contador de procesos completados.

6. **Avance del tiempo**: El tiempo de simulación se incrementa y se simula el paso del tiempo real.

## Finalización

La simulación termina cuando todos los procesos han sido completados. Al final, se exportan las métricas finales y un evento indica el fin de la simulación.

## Referencias a documentación

* La estructura de un proceso y sus atributos está detallada en [docs/definitions/process.md](../definitions/process.md).
* La definición y uso de los eventos de timeline se encuentra en [docs/definitions/event-metrics.md](../definitions/event_metrics.md).
* La estructura y funciones del control de simulación están descritas en [docs/definitions/common.md](../definitions/common.md).
* Protocolo efectuado para algoritmos de calendarización en [docs/protocols/protocolo\_scheduling.md](../protocols/protocolo_scheduling.md).
