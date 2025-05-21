# Definiciones: Eventos, Métricas y Línea de Tiempo en la Simulación

## ¿Qué es un evento?

Un **evento** en el contexto de la simulación de procesos es una representación puntual de un cambio de estado o acción que ocurre durante la ejecución de un proceso. Cada evento describe un estado específico de un proceso en un intervalo determinado del tiempo simulado (ciclos). Los eventos son almacenados en una estructura llamada *línea de tiempo* (`TimelineEvent`) que registra:  

- El identificador del proceso (`pid`)  
- El ciclo de inicio y fin del evento (`startCycle`, `endCycle`)  
- El estado del proceso durante ese intervalo (`state`), como `NEW`, `WAITING`, `ACCESSED`, o `TERMINATED`.

Esta estructura permite reconstruir la evolución y comportamiento de cada proceso a lo largo de la simulación.

### Estructura `TimelineEvent`

```bash
TimelineEvent {
  pid: number,           # Identificador del proceso
  startCycle: number,    # Ciclo donde inicia el evento
  endCycle: number,      # Ciclo donde termina el evento
  state: string          # Estado del proceso en este evento (ej: NEW, WAITING)
}
```

## Métricas establecidas

Para evaluar la eficiencia y desempeño del sistema simulado, se calculan métricas clave basadas en los procesos:

- **Tiempo promedio de espera (avgWaitingTime):** promedio del tiempo que los procesos pasan esperando antes de ser atendidos.
- *(Opcionalmente se pueden considerar otras métricas como tiempo promedio de respuesta y turnaround, aunque el ejemplo solo calcula avgWaitingTime.)*

### Estructura `SimulationMetrics`

```bash
SimulationMetrics {
  avgWaitingTime: number,   # Tiempo promedio de espera
  totalProcesses: number,   # Total de procesos simulados
  [otros_campos_opcionales]: any # se pueden añadir más en el futuro
}
```

Estas métricas permiten comparar diferentes algoritmos de planificación y optimizar el manejo de procesos.

## Funciones clave relacionadas a eventos y métricas

### exportSimulationEnd()

Imprime un mensaje JSON señalando la finalización de la simulación. Este evento indica a cualquier sistema externo o visualizador que la simulación ha concluido.

### printEventForProcess()

Registra y exporta un evento correspondiente a un cambio de estado de un proceso en un ciclo determinado. Crea un nuevo evento en la línea de tiempo, lo añade al arreglo de eventos y lo imprime en tiempo real en formato JSON.

### exportEventRealtime()

Exporta un evento de línea de tiempo en formato JSON hacia la salida estándar en tiempo real, permitiendo que otros sistemas monitoreen el progreso de la simulación sin esperar a su finalización.

### exportTimelineEvents()

Exporta todos los eventos almacenados en la línea de tiempo a un archivo de texto, permitiendo análisis posteriores fuera de la ejecución en vivo.

### exportMetrics()

Guarda las métricas calculadas de la simulación en un archivo de texto, facilitando el registro y comparación de resultados entre diferentes ejecuciones.

### exportProcessMetric()

Imprime métricas detalladas de un proceso individual en formato JSON, útil para análisis más específicos o debug.
