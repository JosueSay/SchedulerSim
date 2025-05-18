#include "simulator.h"
#include <string.h>
#include <stdlib.h>

/*
 * Inicia la simulación.
 * @param control: Puntero al controlador de simulación.
 */
void startSimulation(SimulationControl *control)
{
  if (control == NULL)
    return;
  control->state = SIMULATION_RUNNING;
  control->currentCycle = 0;
}

/*
 * Pausa la simulación si se está ejecutando.
 * @param control: Puntero al controlador de simulación.
 */
void pauseSimulation(SimulationControl *control)
{
  if (control == NULL)
    return;
  if (control->state == SIMULATION_RUNNING)
  {
    control->state = SIMULATION_PAUSED;
  }
}

/*
 * Reanuda la simulación si está en pausa.
 * @param control: Puntero al controlador de simulación.
 */
void resumeSimulation(SimulationControl *control)
{
  if (control == NULL)
    return;
  if (control->state == SIMULATION_PAUSED)
  {
    control->state = SIMULATION_RUNNING;
  }
}

/*
 * Reinicia la simulación a su estado inicial.
 * @param control: Puntero al controlador de simulación.
 */
void resetSimulation(SimulationControl *control)
{
  if (control == NULL)
    return;
  control->state = SIMULATION_NOT_STARTED;
  control->currentCycle = 0;
  control->totalCycles = 0;
  control->isConfigured = 0;
  control->config.algorithm = ALGO_NONE;
  control->config.quantum = 0;
  control->config.isPreemptive = 0;
}

/**
 * Devuelve el nombre en cadena del estado de un proceso dado su enum.
 *
 * @param state Estado del proceso (ProcessState).
 * @return Cadena con el nombre del estado, o "UNKNOWN" si no es válido.
 */
const char *getProcessStateName(ProcessState state)
{
  switch (state)
  {
  case STATE_NEW:
    return "NEW";
  case STATE_READY:
    return "READY";
  case STATE_RUNNING:
    return "RUNNING";
  case STATE_WAITING:
    return "WAITING";
  case STATE_TERMINATED:
    return "TERMINATED";
  default:
    return "UNKNOWN";
  }
}

/**
 * Devuelve el nombre en cadena del tipo de acción dado su enum.
 *
 * @param action Tipo de acción (ActionType).
 * @return Cadena con el nombre de la acción, o "UNKNOWN" si no es válido.
 */
const char *getActionTypeName(ActionType action)
{
  switch (action)
  {
  case ACTION_READ:
    return "READ";
  case ACTION_WRITE:
    return "WRITE";
  case ACTION_NONE:
    return "NONE";
  default:
    return "UNKNOWN";
  }
}

/**
 * Devuelve el nombre en cadena del algoritmo de planificación dado su enum.
 *
 * @param algorithm Algoritmo de planificación (SchedulingAlgorithm).
 * @return Cadena con el nombre del algoritmo, o "UNKNOWN" si no es válido.
 */
const char *getAlgorithmName(SchedulingAlgorithm algorithm)
{
  switch (algorithm)
  {
  case ALGO_FIFO:
    return "FIFO";
  case ALGO_PRIORITY:
    return "PRIORITY";
  case ALGO_RR:
    return "ROUND_ROBIN";
  case ALGO_SJF:
    return "SJF";
  case ALGO_SRT:
    return "SRT";
  case ALGO_NONE:
    return "NONE";
  default:
    return "UNKNOWN";
  }
}

/*
 * Carga los procesos desde un archivo de texto.
 * @param filename: Nombre del archivo que contiene los procesos.
 * @param processes: Arreglo de estructuras de procesos.
 * @param maxProcesses: Número máximo de procesos a cargar.
 * @return Número de procesos cargados o -1 en caso de error.
 */
int loadProcesses(const char *filename, Process *processes, int maxProcesses)
{
  FILE *file = fopen(filename, "r");
  if (!file)
    return -1;

  char line[128];
  int count = 0;

  while (count < maxProcesses && fgets(line, sizeof(line), file))
  {
    line[strcspn(line, "\n")] = '\0';

    if (sscanf(line, " %[^,], %d, %d, %d",
               processes[count].pid,
               &processes[count].burstTime,
               &processes[count].arrivalTime,
               &processes[count].priority) == 4)
    {
      processes[count].state = STATE_NEW;
      processes[count].startTime = -1;
      processes[count].finishTime = -1;
      processes[count].waitingTime = 0;
      count++;
    }
  }

  fclose(file);
  return count;
}

/*
 * Carga los recursos desde un archivo de texto.
 * @param filename: Nombre del archivo que contiene los recursos.
 * @param resources: Arreglo de estructuras de recursos.
 * @param maxResources: Número máximo de recursos a cargar.
 * @return Número de recursos cargados o -1 en caso de error.
 */
int loadResources(const char *filename, Resource *resources, int maxResources)
{
  FILE *file = fopen(filename, "r");
  if (!file)
    return -1;

  char line[128];
  int count = 0;

  while (count < maxResources && fgets(line, sizeof(line), file))
  {
    line[strcspn(line, "\n")] = '\0';

    if (sscanf(line, " %[^,], %d",
               resources[count].name,
               &resources[count].counter) == 2)
    {
      resources[count].isLocked = 0;
      count++;
    }
  }

  fclose(file);
  return count;
}

/**
 * Carga las acciones desde un archivo de texto.
 * Cada línea debe tener el formato: PID, ACTION, RESOURCE, CYCLE
 * Donde ACTION puede ser "READ" o "WRITE".
 *
 * @param filename Nombre del archivo de texto con las acciones.
 * @param actions Arreglo donde se almacenarán las acciones cargadas.
 * @param maxActions Número máximo de acciones a cargar.
 * @return Número de acciones cargadas correctamente, o -1 si hubo un error al abrir el archivo.
 */
int loadActions(const char *filename, Action *actions, int maxActions)
{
  FILE *file = fopen(filename, "r");
  if (!file)
    return -1;

  char line[128];
  char actionStr[10];
  int count = 0;

  while (count < maxActions && fgets(line, sizeof(line), file))
  {
    // Limpiar saltos de línea
    line[strcspn(line, "\n")] = '\0';

    // Formato esperado: PID, ACTION, RESOURCE, CYCLE
    if (sscanf(line, " %[^,], %[^,], %[^,], %d",
               actions[count].pid,
               actionStr,
               actions[count].resourceName,
               &actions[count].cycle) == 4)
    {

      if (strcmp(actionStr, "READ") == 0)
      {
        actions[count].action = ACTION_READ;
      }
      else if (strcmp(actionStr, "WRITE") == 0)
      {
        actions[count].action = ACTION_WRITE;
      }
      else
      {
        actions[count].action = ACTION_NONE;
      }
      count++;
    }
  }

  fclose(file);
  return count;
}

/**
 * Calcula las métricas promedio de la simulación a partir de los procesos dados.
 * Métricas calculadas: tiempo promedio de espera, tiempo promedio de respuesta y tiempo promedio de turnaround.
 *
 * @param processes Arreglo de procesos con sus tiempos registrados.
 * @param processCount Número total de procesos en el arreglo.
 * @return Estructura SimulationMetrics con los valores promedio calculados.
 */
SimulationMetrics calculateMetrics(Process *processes, int processCount)
{
  SimulationMetrics metrics = {0.0f, 0.0f, 0.0f};
  int totalWaitingTime = 0;
  int totalTurnaroundTime = 0;
  int totalResponseTime = 0;

  for (int i = 0; i < processCount; i++)
  {
    int turnaroundTime = processes[i].finishTime - processes[i].arrivalTime;
    int responseTime = processes[i].startTime - processes[i].arrivalTime;

    totalWaitingTime += processes[i].waitingTime;
    totalTurnaroundTime += turnaroundTime;
    totalResponseTime += responseTime;
  }

  if (processCount > 0)
  {
    metrics.avgWaitingTime = (float)totalWaitingTime / processCount;
    metrics.avgTurnaroundTime = (float)totalTurnaroundTime / processCount;
    metrics.avgResponseTime = (float)totalResponseTime / processCount;
  }

  return metrics;
}

/**
 * Exporta los eventos de la línea de tiempo de la simulación a un archivo de texto.
 * Cada evento se escribe con formato: PID, startCycle, endCycle, estado.
 *
 * @param filename Nombre del archivo donde se guardarán los eventos.
 * @param events Arreglo de eventos a exportar.
 * @param eventCount Número de eventos en el arreglo.
 */
void exportTimelineEvents(const char *filename, TimelineEvent *events, int eventCount)
{
  FILE *file = fopen(filename, "w");
  if (!file)
    return;

  for (int i = 0; i < eventCount; i++)
  {
    fprintf(file, "%s, %d, %d, %s\n",
            events[i].pid,
            events[i].startCycle,
            events[i].endCycle,
            getProcessStateName(events[i].state));
  }

  fclose(file);
}

/**
 * Exporta las métricas de simulación a un archivo de texto.
 * Se escriben las métricas promedio de espera, turnaround y respuesta.
 *
 * @param filename Nombre del archivo donde se guardarán las métricas.
 * @param metrics Estructura con las métricas a exportar.
 */
void exportMetrics(const char *filename, SimulationMetrics metrics)
{
  FILE *file = fopen(filename, "w");
  if (!file)
    return;

  fprintf(file, "Average Waiting Time: %.2f\n", metrics.avgWaitingTime);
  fprintf(file, "Average Turnaround Time: %.2f\n", metrics.avgTurnaroundTime);
  fprintf(file, "Average Response Time: %.2f\n", metrics.avgResponseTime);

  fclose(file);
}
