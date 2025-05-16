#include "simulator.h"
#include "fifo.h" // Incluir la cabecera de FIFO
#include <stdio.h>

#define MAX_PROCESSES 100
#define MAX_RESOURCES 50
#define MAX_ACTIONS 200
#define MAX_EVENTS 500

int main()
{
  Process processes[MAX_PROCESSES];
  Resource resources[MAX_RESOURCES];
  Action actions[MAX_ACTIONS];
  TimelineEvent timelineEvents[MAX_EVENTS];
  SimulationControl control = {SIMULATION_NOT_STARTED, 0, 0, 1, {ALGO_FIFO, 0, 0}};
  int eventCount = 0;

  int processCount = loadProcesses("../data/input/procesos.txt", processes, MAX_PROCESSES);
  int resourceCount = loadResources("../data/input/recursos.txt", resources, MAX_RESOURCES);
  int actionCount = loadActions("../data/input/acciones.txt", actions, MAX_ACTIONS);

  printf("=== Procesos Cargados (%d) ===\n", processCount);
  for (int i = 0; i < processCount; i++)
  {
    printf("PID: %s, BT: %d, AT: %d, Priority: %d, State: %s\n",
           processes[i].pid,
           processes[i].burstTime,
           processes[i].arrivalTime,
           processes[i].priority,
           getProcessStateName(processes[i].state));
  }

  printf("\n=== Recursos Cargados (%d) ===\n", resourceCount);
  for (int i = 0; i < resourceCount; i++)
  {
    printf("Resource: %s, Counter: %d, Locked: %d\n",
           resources[i].name,
           resources[i].counter,
           resources[i].isLocked);
  }

  printf("\n=== Acciones Cargadas (%d) ===\n", actionCount);
  for (int i = 0; i < actionCount; i++)
  {
    printf("PID: %s, Action: %s, Resource: %s, Cycle: %d\n",
           actions[i].pid,
           getActionTypeName(actions[i].action),
           actions[i].resourceName,
           actions[i].cycle);
  }

  // Simulación de eventos de timeline (provisional)
  for (int i = 0; i < processCount; i++)
  {
    timelineEvents[eventCount].startCycle = processes[i].arrivalTime;
    timelineEvents[eventCount].endCycle = processes[i].arrivalTime + processes[i].burstTime;
    snprintf(timelineEvents[eventCount].pid, PID_MAX_LEN, "%s", processes[i].pid);
    timelineEvents[eventCount].state = STATE_RUNNING;
    eventCount++;
  }

  SimulationMetrics metrics = calculateMetrics(processes, processCount);

  exportTimelineEvents("../data/output/timeline.txt", timelineEvents, eventCount);
  exportMetrics("../data/output/metrics.txt", metrics);

  printf("\n=== Ejecutando Simulación FIFO ===\n");

  // Ejecutar FIFO real
  eventCount = 0; // Resetear contador de eventos antes de la simulación real
  simulateFIFO(processes, processCount, timelineEvents, &eventCount, &control);

  // Calcular métricas basadas en la simulación real
  metrics = calculateMetrics(processes, processCount);

  // Exportar resultados reales
  exportMetrics("../data/output/metrics.txt", metrics);
  exportTimelineEvents("../data/output/timeline.txt", timelineEvents, eventCount);

  printf("Métricas y Timeline exportados a ../data/output/\n");

  return 0;
}
