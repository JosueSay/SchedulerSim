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

// Simula sincronización usando mutex o semáforo con distinción entre READ y WRITE
void simulateSynchronization(Process *processes, int processCount,
                             Resource *resources, int resourceCount,
                             Action *actions, int actionCount,
                             TimelineEvent *events, int *eventCount,
                             int useMutex)
{

  int currentCycle = 0;
  int completed = 0;
  *eventCount = 0;

  bool newPrinted[MAX_PROCESSES] = {false};
  int waitingCounters[MAX_PROCESSES] = {0};
  int firstActionCycle[MAX_PROCESSES];
  int lastActionCycle[MAX_PROCESSES];
  bool started[MAX_PROCESSES] = {false};
  bool resourceUsedThisCycle[COMMON_MAX_LEN] = {false};

  int starvationLimit = 20;
  int noProgressCycles = 0;

  for (int i = 0; i < processCount; i++)
  {
    firstActionCycle[i] = -1;
    lastActionCycle[i] = -1;
  }

  initializeResources(resources, resourceCount);

  while (completed < processCount)
  {
    int progressMade = 0;

    // Imprimir eventos NEW para procesos que llegan
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime == currentCycle && !newPrinted[i])
      {
        printEventForSyncProcess(&processes[i], currentCycle, STATE_NEW, events, eventCount, ACTION_NONE);
        newPrinted[i] = true;
      }
    }

    memset(resourceUsedThisCycle, 0, sizeof(bool) * resourceCount);

    // Procesar acciones de cada proceso
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].state == STATE_TERMINATED || processes[i].arrivalTime > currentCycle)
        continue;

      for (int j = 0; j < actionCount; j++)
      {
        Action *a = &actions[j];
        if (strcmp(a->pid, processes[i].pid) != 0)
          continue;
        if (a->cycle > currentCycle)
          continue;

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
          continue;
        Resource *r = &resources[resIndex];

        int granted = 0;
        granted = useMutex ? acquireMutex(r) : acquireSemaphore(r);

        if (granted)
        {
          resourceUsedThisCycle[resIndex] = true;

          printEventForSyncProcess(&processes[i], currentCycle, STATE_ACCESSED, events, eventCount, a->action);

          if (!started[i])
          {
            firstActionCycle[i] = currentCycle;
            started[i] = true;
          }
          lastActionCycle[i] = currentCycle;

          processes[i].burstTime--;
          progressMade++;
          break; // Solo una acción por ciclo
        }

        waitingCounters[i]++;
        printEventForSyncProcess(&processes[i], currentCycle, STATE_WAITING, events, eventCount, ACTION_NONE);
        break;
      }
    }

    // Liberar recursos utilizados en este ciclo
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

    // Verificar procesos terminados
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].state != STATE_TERMINATED && processes[i].burstTime == 0)
      {
        processes[i].state = STATE_TERMINATED;
        processes[i].waitingTime = waitingCounters[i];
        processes[i].startTime = firstActionCycle[i];
        processes[i].finishTime = lastActionCycle[i] + 1;

        printEventForSyncProcess(&processes[i], currentCycle, STATE_TERMINATED, events, eventCount, ACTION_NONE);
        exportProcessMetric(&processes[i]);
        completed++;
      }
    }

    // Detectar estancamiento
    if (progressMade == 0)
    {
      noProgressCycles++;
      if (noProgressCycles > starvationLimit)
      {
        fprintf(stderr, "\nPosible deadlock: sin progreso por %d ciclos.\n", starvationLimit);
        break;
      }
    }
    else
    {
      noProgressCycles = 0;
    }

    currentCycle++;
    usleep(SIMULATION_DELAY_US);
  }
}
