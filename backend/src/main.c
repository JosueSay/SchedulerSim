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
  SimulationControl control = {{ALGO_FIFO, 0, 0}};
  int eventCount = 0;

  // Cargar archivos de entrada
  int processCount = loadProcesses("../data/input/procesos.txt", processes, MAX_PROCESSES);
  int resourceCount = loadResources("../data/input/recursos.txt", resources, MAX_RESOURCES);
  int actionCount = loadActions("../data/input/acciones.txt", actions, MAX_ACTIONS);

  // Depuración: mostrar datos cargados (opcional)
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
    printf("Nombre: %s, Contador: %d, isLocked: %d\n",
           resources[i].name,
           resources[i].counter,
           resources[i].isLocked);
  }

  printf("\n=== Acciones Cargadas (%d) ===\n", actionCount);
  for (int i = 0; i < actionCount; i++)
  {
    printf("PID: %s, Acción: %s, Recurso: %s, Ciclo: %d\n",
           actions[i].pid,
           getActionTypeName(actions[i].action),
           actions[i].resourceName,
           actions[i].cycle);
  }

  printf("\n=== Ejecutando Simulación FIFO ===\n");

  // Simular
  simulateFIFO(processes, processCount, timelineEvents, &eventCount, &control);

  // Calcular y exportar métricas
  SimulationMetrics metrics = calculateMetrics(processes, processCount);
  exportMetrics("../data/output/metrics.txt", metrics);

  // Notificar fin de simulación y enviar métrica promedio final (JSON)
  exportSimulationEnd();
  printf("{\"type\": \"metrics\", \"Average Waiting Time\": %.2f}\n", metrics.avgWaitingTime);
  fflush(stdout);

  return 0;
}
