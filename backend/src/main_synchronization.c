#include "simulator.h"
#include "synchronization.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cjson/cJSON.h>

void printLoadedDataJSON(Process *processes, int processCount,
                         Resource *resources, int resourceCount,
                         Action *actions, int actionCount,
                         int useMutex)
{
  // Imprimir configuración del mecanismo
  printf("{\"event\":\"CONFIG\",\"mechanism\":\"%s\"}\n", useMutex ? "mutex" : "semaphore");

  // Procesos cargados
  for (int i = 0; i < processCount; i++)
  {
    printf(
        "{\"event\":\"PROCESS_LOADED\",\"pid\":\"%s\",\"burstTime\":%d,\"arrivalTime\":%d,\"priority\":%d}\n",
        processes[i].pid, processes[i].burstTime, processes[i].arrivalTime, processes[i].priority);
  }

  // Recursos cargados
  for (int i = 0; i < resourceCount; i++)
  {
    printf("{\"event\":\"RESOURCE_LOADED\",\"name\":\"%s\",\"counter\":%d}\n",
           resources[i].name, resources[i].counter);
  }

  // Acciones cargadas
  for (int i = 0; i < actionCount; i++)
  {
    printf(
        "{\"event\":\"ACTION_LOADED\",\"pid\":\"%s\",\"action\":\"%s\",\"resource\":\"%s\",\"cycle\":%d}\n",
        actions[i].pid, getActionTypeName(actions[i].action), actions[i].resourceName, actions[i].cycle);
  }
}

void printDataHumanReadable(Process *processes, int processCount,
                            Resource *resources, int resourceCount,
                            Action *actions, int actionCount)
{
  printf("\n=== PROCESOS (%d) ===\n", processCount);
  for (int i = 0; i < processCount; i++)
  {
    printf("PID: %s, BT: %d, AT: %d, Priority: %d, State: %s\n",
           processes[i].pid, processes[i].burstTime, processes[i].arrivalTime,
           processes[i].priority, getProcessStateName(processes[i].state));
  }

  printf("\n=== RECURSOS (%d) ===\n", resourceCount);
  for (int i = 0; i < resourceCount; i++)
  {
    printf("Nombre: %s, Disponible: %d, Bloqueado: %s\n",
           resources[i].name, resources[i].counter,
           resources[i].isLocked ? "Sí" : "No");
  }

  printf("\n=== ACCIONES (%d) ===\n", actionCount);
  for (int i = 0; i < actionCount; i++)
  {
    printf("PID: %s, Acción: %s, Recurso: %s, Ciclo: %d\n",
           actions[i].pid, getActionTypeName(actions[i].action),
           actions[i].resourceName, actions[i].cycle);
  }
}

int main()
{
  Process processes[MAX_PROCESSES];
  Resource resources[MAX_PROCESSES];
  Action actions[MAX_EVENTS];
  TimelineEvent events[MAX_EVENTS];
  int eventCount = 0;

  // Leer config desde stdin
  char buffer[512];
  int useMutex = 1;
  if (fgets(buffer, sizeof(buffer), stdin))
  {
    cJSON *json = cJSON_Parse(buffer);
    if (!json)
    {
      fprintf(stderr, "Error al parsear JSON de configuración.\n");
      exit(EXIT_FAILURE);
    }

    const cJSON *mutexField = cJSON_GetObjectItemCaseSensitive(json, "useMutex");
    if (cJSON_IsNumber(mutexField))
    {
      useMutex = mutexField->valueint;
    }
    cJSON_Delete(json);
  }

  // Cargar datos
  int processCount = loadProcesses("../data/input/procesos.txt", processes, MAX_PROCESSES);
  int resourceCount = loadResources("../data/input/recursos.txt", resources, MAX_PROCESSES);
  int actionCount = loadActions("../data/input/acciones.txt", actions, MAX_EVENTS);

  // Exportar JSON de elementos cargados
  printLoadedDataJSON(processes, processCount, resources, resourceCount, actions, actionCount, useMutex);
  printDataHumanReadable(processes, processCount, resources, resourceCount, actions, actionCount);
  printf("\n=== Ejecutando sincronización con: %s ===\n", useMutex ? "mutex" : "semaphore");

  // Ejecutar simulación
  simulateSynchronization(processes, processCount,
                          resources, resourceCount,
                          actions, actionCount,
                          events, &eventCount,
                          useMutex);

  return 0;
}
