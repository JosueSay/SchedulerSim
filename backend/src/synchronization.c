#include "synchronization.h"
#include "sync_mutex.h"
#include "sync_semaphore.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

void initializeResources(Resource *resources, int resourceCount)
{
  for (int i = 0; i < resourceCount; i++)
  {
    resources[i].isLocked = 0;
  }
}

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

  for (int i = 0; i < processCount; i++)
  {
    firstActionCycle[i] = -1;
    lastActionCycle[i] = -1;
  }

  initializeResources(resources, resourceCount);

  while (completed < processCount)
  {
    // Marcar procesos nuevos
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime == currentCycle && !newPrinted[i])
      {
        printEventForProcess(&processes[i], currentCycle, STATE_NEW, events, eventCount);
        newPrinted[i] = true;
      }
    }

    // Limpiar uso de recursos del ciclo anterior
    memset(resourceUsedThisCycle, 0, sizeof(bool) * COMMON_MAX_LEN);

    // Primera fase: ejecución de acciones
    for (int i = 0; i < actionCount; i++)
    {
      Action *a = &actions[i];
      if (a->cycle == currentCycle)
      {
        Process *p = NULL;
        int pIndex = -1;
        for (int j = 0; j < processCount; j++)
        {
          if (strcmp(processes[j].pid, a->pid) == 0)
          {
            p = &processes[j];
            pIndex = j;
            break;
          }
        }

        int resIndex = -1;
        Resource *r = NULL;
        for (int j = 0; j < resourceCount; j++)
        {
          if (strcmp(resources[j].name, a->resourceName) == 0)
          {
            r = &resources[j];
            resIndex = j;
            break;
          }
        }

        if (p && r && resIndex != -1)
        {
          if (!resourceUsedThisCycle[resIndex])
          {
            int granted = useMutex ? acquireMutex(r) : acquireSemaphore(r);
            if (granted)
            {
              resourceUsedThisCycle[resIndex] = true;
              printEventForProcess(p, currentCycle, STATE_ACCESSED, events, eventCount);
              if (!started[pIndex])
              {
                firstActionCycle[pIndex] = currentCycle;
                started[pIndex] = true;
              }
              lastActionCycle[pIndex] = currentCycle;
              continue; // no marcar como waiting
            }
          }
          waitingCounters[pIndex]++;
          printEventForProcess(p, currentCycle, STATE_WAITING, events, eventCount);
        }
      }
    }

    // Segunda fase: liberar recursos
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
      if (processes[i].state != STATE_TERMINATED)
      {
        bool hasPending = false;
        for (int j = 0; j < actionCount; j++)
        {
          if (strcmp(actions[j].pid, processes[i].pid) == 0 &&
              actions[j].cycle > currentCycle)
          {
            hasPending = true;
            break;
          }
        }

        if (!hasPending)
        {
          processes[i].state = STATE_TERMINATED;
          processes[i].waitingTime = waitingCounters[i];
          processes[i].startTime = firstActionCycle[i];
          processes[i].finishTime = lastActionCycle[i] + 1;

          printEventForProcess(&processes[i], currentCycle, STATE_TERMINATED, events, eventCount);
          exportProcessMetric(&processes[i]);
          completed++;
        }
      }
    }

    currentCycle++;
    usleep(SIMULATION_DELAY_US);
  }
}
