#include "synchronization.h"
#include "sync_mutex.h"
#include "sync_semaphore.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

// Inicializa recursos con valores desbloqueados
void initializeResources(Resource *resources, int resourceCount)
{
  for (int i = 0; i < resourceCount; i++)
  {
    resources[i].isLocked = 0;
  }
}

// Función principal que simula la sincronización con mutex o semáforo
void simulateSynchronization(Process *processes, int processCount,
                             Resource *resources, int resourceCount,
                             Action *actions, int actionCount,
                             TimelineEvent *events, int *eventCount,
                             int useMutex)
{
  int currentCycle = 0;
  int completed = 0;
  *eventCount = 0;

  // Variables de estado para control y métricas
  bool newPrinted[MAX_PROCESSES] = {false};
  int waitingCounters[MAX_PROCESSES] = {0};
  int firstActionCycle[MAX_PROCESSES];   // Primer ciclo donde proceso realizó acción
  int lastActionCycle[MAX_PROCESSES];    // Último ciclo con acción realizada
  bool started[MAX_PROCESSES] = {false}; // Marca si proceso inició ejecución
  bool resourceUsedThisCycle[COMMON_MAX_LEN] = {false};
  int originalBurstTimes[MAX_PROCESSES];
  bool actionProcessed[COMMON_MAX_LEN] = {false};

  // Inicializar arrays para control de ciclos y burst times originales
  for (int i = 0; i < processCount; i++)
  {
    firstActionCycle[i] = -1;
    lastActionCycle[i] = -1;
    originalBurstTimes[i] = processes[i].burstTime;
  }

  // Inicializar estado de los recursos
  initializeResources(resources, resourceCount);

  // Bucle principal de simulación, avanza ciclo por ciclo hasta completar todos los procesos
  while (completed < processCount)
  {
    int progressMade = 0; // Marca si se hizo progreso en este ciclo

    // Detectar procesos que llegan en el ciclo actual y mostrar estado NEW
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime == currentCycle && !newPrinted[i])
      {
        printEventForSyncProcess(&processes[i], currentCycle, STATE_NEW, events, eventCount, ACTION_NONE);
        newPrinted[i] = true;
      }
    }

    // Resetear marcas de recursos usados en este ciclo
    memset(resourceUsedThisCycle, 0, sizeof(bool) * resourceCount);

    // Evaluar acciones de cada proceso para el ciclo actual
    for (int i = 0; i < processCount; i++)
    {
      // Saltar procesos terminados o que aún no llegaron
      if (processes[i].state == STATE_TERMINATED || processes[i].arrivalTime > currentCycle)
        continue;

      for (int j = 0; j < actionCount; j++)
      {
        Action *a = &actions[j];

        // Saltar acciones ya procesadas o que no corresponden al proceso o ciclo actual
        if (actionProcessed[j] || strcmp(a->pid, processes[i].pid) != 0 || a->cycle != currentCycle)
          continue;

        // Buscar índice del recurso requerido
        int resIndex = -1;
        for (int k = 0; k < resourceCount; k++)
        {
          if (strcmp(resources[k].name, a->resourceName) == 0)
          {
            resIndex = k;
            break;
          }
        }
        if (resIndex == -1)
          continue; // Recurso no encontrado, saltar acción

        Resource *r = &resources[resIndex];
        int granted = useMutex ? acquireMutex(r) : acquireSemaphore(r);

        if (granted)
        {
          // Recurso concedido, marcar uso y registrar evento
          resourceUsedThisCycle[resIndex] = true;
          printEventForSyncProcess(&processes[i], currentCycle, STATE_ACCESSED, events, eventCount, a->action);

          // Reducir burst time y registrar progreso
          processes[i].burstTime--;
          progressMade++;

          // Registrar primer ciclo y último ciclo de acción para métricas
          if (!started[i])
          {
            firstActionCycle[i] = currentCycle;
            started[i] = true;
          }
          lastActionCycle[i] = currentCycle;
        }
        else
        {
          // No pudo adquirir recurso, proceso pasa a WAITING
          waitingCounters[i]++;
          printEventForSyncProcess(&processes[i], currentCycle, STATE_WAITING, events, eventCount, a->action);
        }

        // Marcar acción como procesada y salir del loop de acciones por proceso
        actionProcessed[j] = true;
        break;
      }
    }

    // Liberar recursos usados en este ciclo
    for (int i = 0; i < resourceCount; i++)
    {
      if (resourceUsedThisCycle[i])
      {
        if (useMutex)
          releaseMutex(&resources[i]);
        else
          releaseSemaphore(&resources[i]);
      }
    }

    // Verificar procesos terminados (burstTime == 0)
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].state != STATE_TERMINATED && processes[i].burstTime == 0)
      {
        processes[i].state = STATE_TERMINATED;
        processes[i].waitingTime = waitingCounters[i];
        processes[i].startTime = firstActionCycle[i];
        processes[i].finishTime = lastActionCycle[i] + 1;

        printEventForSyncProcess(&processes[i], currentCycle, STATE_TERMINATED, events, eventCount, ACTION_NONE);
        exportProcessMetricWithOriginalBT(&processes[i], originalBurstTimes[i]);
        completed++;
      }
    }

    // Detectar posibles deadlocks o bloqueos: no hay progreso y hay procesos esperando
    bool waitingFound = false;
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].state != STATE_TERMINATED && waitingCounters[i] > 0)
      {
        waitingFound = true;
        break;
      }
    }

    if (waitingFound && progressMade == 0)
    {
      fprintf(stderr, "\nDeadlock o bloqueo detectado: procesos en WAITING sin progreso. Omitiendo procesos bloqueados.\n");

      // Finalizar procesos bloqueados con estado OMITED en lugar de TERMINATED si burstTime > 0
      for (int i = 0; i < processCount; i++)
      {
        if (processes[i].state != STATE_TERMINATED && waitingCounters[i] > 0)
        {
          if (processes[i].burstTime > 0)
          {
            processes[i].state = STATE_OMITED;
          }
          else
          {
            processes[i].state = STATE_TERMINATED;
          }
          processes[i].waitingTime = waitingCounters[i];
          processes[i].startTime = started[i] ? firstActionCycle[i] : -1;
          processes[i].finishTime = currentCycle;

          printEventForSyncProcess(&processes[i], currentCycle, processes[i].state, events, eventCount, ACTION_NONE);
          exportProcessMetricWithOriginalBT(&processes[i], originalBurstTimes[i]);
          completed++;
        }
      }
      break; // Salir del while ya que no hay más progreso posible
    }

    // --- Verificar procesos sin acciones pendientes pero con burstTime > 0 ---
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].state != STATE_TERMINATED && processes[i].state != STATE_OMITED)
      {
        // Revisar si el proceso tiene acciones pendientes
        bool hasPendingAction = false;
        for (int j = 0; j < actionCount; j++)
        {
          if (!actionProcessed[j] && strcmp(actions[j].pid, processes[i].pid) == 0)
          {
            hasPendingAction = true;
            break;
          }
        }

        // Si no hay acciones pendientes y el proceso aún no terminó, omitirlo
        if (!hasPendingAction)
        {
          processes[i].state = STATE_OMITED;
          processes[i].waitingTime = waitingCounters[i];
          processes[i].startTime = started[i] ? firstActionCycle[i] : -1;
          processes[i].finishTime = currentCycle;

          printEventForSyncProcess(&processes[i], currentCycle, processes[i].state, events, eventCount, ACTION_NONE);
          exportProcessMetricWithOriginalBT(&processes[i], originalBurstTimes[i]);
          completed++;
        }
      }
    }

    // Comprobar si todos los procesos están TERMINATED o OMITED y no hay acciones pendientes para terminar la simulación
    bool allDone = true;
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].state != STATE_TERMINATED && processes[i].state != STATE_OMITED)
      {
        allDone = false;
        break;
      }
    }
    bool pendingActions = false;
    for (int j = 0; j < actionCount; j++)
    {
      if (!actionProcessed[j])
      {
        pendingActions = true;
        break;
      }
    }

    if (allDone && !pendingActions)
    {
      fprintf(stderr, "[DEBUG] Todos los procesos terminados u omitidos y sin acciones pendientes. Finalizando simulación.\n");
      break;
    }

    currentCycle++;
    usleep(SIMULATION_DELAY_US);
  }

  // Calcular métricas de la simulación
  SimulationMetrics metrics = calculateMetrics(processes, processCount);
  printf("{\"type\": \"metrics\", \"Average Waiting Time\": %.2f}\n", metrics.avgWaitingTime);
  exportSimulationEnd();
}
