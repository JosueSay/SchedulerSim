#include "simulator.h"
#include "fifo.h"
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
  SimulationControl control = {0, 0, {ALGO_FIFO, 0, 0}};
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
    printf("Nombre: %s, Contador: %d, Estado (isLocked): %d\n",
           resources[i].name,
           resources[i].counter,
           resources[i].isLocked);
  }

  printf("\n=== Acciones Cargadas (%d) ===\n", actionCount);
  for (int i = 0; i < actionCount; i++)
  {
    printf("PID: %s, Acción: %d, Recurso: %s, Ciclo: %d\n",
           actions[i].pid,
           actions[i].action,
           actions[i].resourceName,
           actions[i].cycle);
  }

  printf("\n=== Ejecutando Simulación FIFO en Tiempo Real ===\n");

  eventCount = 0;
  simulateFIFO(processes, processCount, timelineEvents, &eventCount, &control);

  // Calcular métricas finales
  SimulationMetrics metrics = calculateMetrics(processes, processCount);
  exportMetrics("../data/output/metrics.txt", metrics);

  // Notificar fin de simulación (para WebSocket o frontend)
  exportSimulationEnd();

  // Enviar métricas finales en JSON con tipo para distinguir en frontend
  printf("{\"type\": \"metrics\", \"data\": {\"Average Waiting Time\": %.2f, \"Average Turnaround Time\": %.2f, \"Average Response Time\": %.2f}}\n",
         metrics.avgWaitingTime, metrics.avgTurnaroundTime, metrics.avgResponseTime);

  fflush(stdout); // Asegurar salida inmediata
  return 0;
}
