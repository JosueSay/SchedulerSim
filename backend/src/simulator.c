#include "simulator.h"
#include <string.h>
#include <stdlib.h>

/**
 * Control de Simulación
 */

void startSimulation(SimulationControl *control)
{
  if (control == NULL)
    return;
  control->state = SIMULATION_RUNNING;
  control->currentCycle = 0;
}

void pauseSimulation(SimulationControl *control)
{
  if (control == NULL)
    return;
  if (control->state == SIMULATION_RUNNING)
  {
    control->state = SIMULATION_PAUSED;
  }
}

void resumeSimulation(SimulationControl *control)
{
  if (control == NULL)
    return;
  if (control->state == SIMULATION_PAUSED)
  {
    control->state = SIMULATION_RUNNING;
  }
}

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
 * Conversión de enums a cadenas
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

/**
 * Carga de Procesos desde archivo
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
    // Eliminar salto de línea
    line[strcspn(line, "\n")] = '\0';

    // Leer respetando los delimitadores con coma y espacios
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

/**
 * Carga de Recursos desde archivo
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
    // Eliminar salto de línea
    line[strcspn(line, "\n")] = '\0';

    // Formato esperado: RESOURCE_NAME, COUNTER
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
 * Carga de Acciones desde archivo
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
