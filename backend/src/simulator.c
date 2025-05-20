#include "simulator.h"
#include <string.h>
#include <stdlib.h>

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
  case STATE_WAITING:
    return "WAITING";
  case STATE_ACCESSED:
    return "ACCESSED";
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
    line[strcspn(line, "\n")] = '\0';
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
      count++;
    }
  }

  fclose(file);
  return count;
}

/**
 * Calcula métricas: solo avgWaitingTime.
 * Métricas calculadas: tiempo promedio de espera, tiempo promedio de respuesta y tiempo promedio de turnaround.
 *
 * @param processes Arreglo de procesos con sus tiempos registrados.
 * @param processCount Número total de procesos en el arreglo.
 * @return Estructura SimulationMetrics con métricas
 */
SimulationMetrics calculateMetrics(Process *processes, int processCount)
{
  SimulationMetrics metrics = {0.0f};
  int totalWaitingTime = 0;

  for (int i = 0; i < processCount; i++)
  {
    totalWaitingTime += processes[i].waitingTime;
  }

  if (processCount > 0)
  {
    metrics.avgWaitingTime = (float)totalWaitingTime / processCount;
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
  fclose(file);
}

/**
 * Imprime en formato JSON las métricas de un proceso específico en la salida estándar.
 *
 * Este método muestra información clave del proceso para su análisis o monitoreo, incluyendo:
 * - Identificador del proceso (pid)
 * - Tiempo de llegada (arrivalTime)
 * - Tiempo de ráfaga (burstTime)
 * - Prioridad (priority)
 * - Tiempo de inicio de ejecución (startTime)
 * - Tiempo de finalización (finishTime)
 * - Tiempo total de espera (waitingTime)
 *
 * La información se imprime en una línea con formato JSON, lo que facilita su procesamiento
 * automatizado, como para ser leído por scripts, sistemas de logging o monitoreo.
 *
 * @param p Puntero a la estructura Process que contiene los datos del proceso.
 */
void exportProcessMetric(const Process *p)
{
  printf(
      "{\"event\": \"PROCESS_METRIC\", \"pid\": \"%s\", \"arrivalTime\": %d, \"burstTime\": %d, \"priority\": %d, "
      "\"startTime\": %d, \"endTime\": %d, \"waitingTime\": %d}\n",
      p->pid, p->arrivalTime, p->burstTime, p->priority,
      p->startTime, p->finishTime, p->waitingTime);
  fflush(stdout);
}

void exportEventRealtime(TimelineEvent *event)
{
  printf("{\"pid\": \"%s\", \"startCycle\": %d, \"endCycle\": %d, \"state\": \"%s\"}\n",
         event->pid,
         event->startCycle,
         event->endCycle,
         getProcessStateName(event->state));
  fflush(stdout);
}

void exportSimulationEnd()
{
  printf("{\"event\": \"SIMULATION_END\"}\n");
  fflush(stdout);
}
