# Procesos / Process

## Formato del Archivo

El archivo de entrada contiene una lista de procesos, uno por línea, con el siguiente formato:

```bash
<PID>, <BT>, <AT>, <Priority>
```

Donde:

* **PID**: Identificador único del proceso (cadena de texto).
* **BT** (Burst Time): Tiempo total que el proceso requiere para ejecutarse (entero).
* **AT** (Arrival Time): Tiempo en que el proceso llega al sistema (entero).
* **Priority**: Prioridad asignada al proceso (entero), donde un valor menor indica mayor prioridad.

### Ejemplo de línea válida

```bash
P1, 8, 0, 1
```

## Estructura Interna del Proceso

Cada línea del archivo es convertida a una estructura de datos interna llamada `Proceso` que contiene al menos los siguientes atributos:

| Atributo       | Tipo    | Descripción                                     |
| -------------- | ------- | ----------------------------------------------- |
| `pid`          | String  | Identificador único del proceso.                |
| `burst_time`   | Integer | Tiempo requerido para la ejecución del proceso. |
| `arrival_time` | Integer | Momento en que el proceso llega al sistema.     |
| `priority`     | Integer | Prioridad del proceso.                          |

> Nota: La estructura `Proceso` puede contener otros atributos adicionales según la implementación del algoritmo de planificación, pero estos cuatro atributos son obligatorios y se obtienen directamente del archivo de entrada.

## Detalles y funcionalidades implementadas

### Estructura `Process`

* Representa un proceso individual en la simulación.
* Contiene atributos esenciales para la simulación de planificación:

  * `pid`: Identificador único del proceso (string).
  * `burstTime`: Tiempo total que el proceso necesita para ejecutarse (CPU burst).
  * `arrivalTime`: Momento en el que el proceso llega al sistema (tiempo de llegada).
  * `priority`: Prioridad asignada al proceso (para algoritmos que la utilicen).
  * Métricas para evaluación del desempeño:

    * `startTime`: Momento en el que el proceso comenzó su ejecución (inicializado en -1).
    * `finishTime`: Momento en el que el proceso terminó su ejecución (inicializado en -1).
    * `waitingTime`: Tiempo total que el proceso estuvo esperando para ejecutarse.
  * `state`: Estado actual del proceso, que puede ser:

    * `STATE_NEW` (nuevo)
    * `STATE_WAITING` (en espera)
    * `STATE_ACCESSED` (ejecutándose o accediendo al CPU)
    * `STATE_TERMINATED` (terminado)
