#include "simulator.h"
#include "fifo.h"
#include "sjf.h"
#include <stdio.h>
#include <string.h>

#define MAX_PROCESSES 100
#define MAX_EVENTS 500

// Traduce string a enum
SchedulingAlgorithm parseAlgorithm(const char *str)
{
  if (strcmp(str, "FIFO") == 0)
    return ALGO_FIFO;
  if (strcmp(str, "SJF") == 0)
    return ALGO_SJF;
  if (strcmp(str, "RR") == 0)
    return ALGO_RR;
  if (strcmp(str, "PS") == 0)
    return ALGO_PRIORITY;
  if (strcmp(str, "SRT") == 0)
    return ALGO_SRT;
  return ALGO_NONE;
}

// Lee la configuración desde stdin (en formato JSON)
void readConfigFromStdin(SimulationControl *control)
{
  char buffer[256];
  if (fgets(buffer, sizeof(buffer), stdin))
  {
    char algorithmStr[20];
    int quantum = 0;
    int isPreemptive = 0;

    sscanf(buffer, "{\"algorithm\":\"%[^\"]\", \"quantum\":%d, \"isPreemptive\":%d}",
           algorithmStr, &quantum, &isPreemptive);

    control->config.algorithm = parseAlgorithm(algorithmStr);
    control->config.quantum = quantum;
    control->config.isPreemptive = isPreemptive;
  }
  else
  {
    // Default a FIFO si no se recibe nada
    control->config.algorithm = ALGO_FIFO;
    control->config.quantum = 0;
    control->config.isPreemptive = 0;
  }
}

int main()
{
  Process processes[MAX_PROCESSES];
  TimelineEvent timelineEvents[MAX_EVENTS];
  SimulationControl control;
  int eventCount = 0;

  // Leer configuración enviada desde el frontend (stdin)
  readConfigFromStdin(&control);

  // Cargar archivos de entrada
  int processCount = loadProcesses("../data/input/procesos.txt", processes, MAX_PROCESSES);

  // Depuración: mostrar datos cargados
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

  printf("\n=== Ejecutando Algoritmo: %s ===\n", getAlgorithmName(control.config.algorithm));

  // Ejecutar el algoritmo seleccionado
  switch (control.config.algorithm)
  {
  case ALGO_FIFO:
    simulateFIFO(processes, processCount, timelineEvents, &eventCount, &control);
    break;
  case ALGO_SJF:
    simulateSJF(processes, processCount, timelineEvents, &eventCount, &control);
    break;
  default:
    printf("Algoritmo no soportado.\n");
    return 1;
  }

  // Calcular y exportar métricas
  SimulationMetrics metrics = calculateMetrics(processes, processCount);
  exportMetrics("../data/output/metrics.txt", metrics);

  // Notificar fin de simulación y enviar métrica promedio final
  exportSimulationEnd();
  printf("{\"type\": \"metrics\", \"Average Waiting Time\": %.2f}\n", metrics.avgWaitingTime);
  fflush(stdout);

  return 0;
}
