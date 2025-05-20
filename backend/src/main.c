#include "simulator.h"
#include "fifo.h"
#include "sjf.h"
#include "rr.h"
#include "ps.h"
#include <stdio.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <stdlib.h>

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

// Lee la configuración desde stdin (en formato JSON) usando cJSON
void readConfigFromStdin(SimulationControl *control)
{
  char buffer[512];
  if (fgets(buffer, sizeof(buffer), stdin))
  {
    cJSON *json = cJSON_Parse(buffer);
    if (json == NULL)
    {
      fprintf(stderr, "Error al parsear configuración JSON.\n");
      exit(EXIT_FAILURE);
    }

    const cJSON *alg = cJSON_GetObjectItemCaseSensitive(json, "algorithm");
    const cJSON *quant = cJSON_GetObjectItemCaseSensitive(json, "quantum");
    const cJSON *preempt = cJSON_GetObjectItemCaseSensitive(json, "isPreemptive");

    if (cJSON_IsString(alg) && alg->valuestring)
      control->config.algorithm = parseAlgorithm(alg->valuestring);
    else
      control->config.algorithm = ALGO_FIFO;

    if (cJSON_IsNumber(quant))
      control->config.quantum = quant->valueint;
    else
      control->config.quantum = 0;

    if (cJSON_IsNumber(preempt))
      control->config.isPreemptive = preempt->valueint;
    else
      control->config.isPreemptive = 0;

    printf("Algoritmo: %s | Quantum: %d | Preemptivo: %s\n",
           alg && alg->valuestring ? alg->valuestring : "(none)",
           control->config.quantum,
           control->config.isPreemptive ? "Sí" : "No");

    cJSON_Delete(json);
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

  readConfigFromStdin(&control);

  int processCount = loadProcesses("../data/input/procesos.txt", processes, MAX_PROCESSES);

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

  switch (control.config.algorithm)
  {
  case ALGO_FIFO:
    simulateFIFO(processes, processCount, timelineEvents, &eventCount, &control);
    break;
  case ALGO_SJF:
    simulateSJF(processes, processCount, timelineEvents, &eventCount, &control);
    break;
  case ALGO_RR:
    simulateRR(processes, processCount, timelineEvents, &eventCount, &control);
    break;
  case ALGO_PRIORITY:
    simulatePS(processes, processCount, timelineEvents, &eventCount, &control);
    break;
  default:
    printf("Algoritmo no soportado.\n");
    return 1;
  }

  SimulationMetrics metrics = calculateMetrics(processes, processCount);
  exportMetrics("../data/output/metrics.txt", metrics);

  exportSimulationEnd();
  printf("{\"type\": \"metrics\", \"Average Waiting Time\": %.2f}\n", metrics.avgWaitingTime);
  fflush(stdout);

  return 0;
}
