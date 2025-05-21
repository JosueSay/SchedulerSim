#include "srt.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

void simulateSRT(Process *processes, int processCount,
                 TimelineEvent *events, int *eventCount,
                 SimulationControl *control)
{
  int currentTime = 0;
  int completed = 0;
  int remainingTime[MAX_PROCESSES];
  *eventCount = 0;

  for (int i = 0; i < processCount; i++)
  {
    remainingTime[i] = processes[i].burstTime;
    processes[i].startTime = -1;
  }

  while (completed < processCount)
  {
    int shortestIdx = -1;
    int minRemaining = INT_MAX;

    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime <= currentTime &&
          processes[i].state != STATE_TERMINATED &&
          remainingTime[i] > 0 &&
          remainingTime[i] < minRemaining)
      {
        minRemaining = remainingTime[i];
        shortestIdx = i;
      }
    }

    if (shortestIdx == -1)
    {
      currentTime++;
      usleep(SIMULATION_DELAY_US);
      continue;
    }

    Process *p = &processes[shortestIdx];
    if (p->startTime == -1)
      p->startTime = currentTime;

    // Ejecutar 1 ciclo
    snprintf(events[*eventCount].pid, COMMON_MAX_LEN, "%s", p->pid);
    events[*eventCount].startCycle = currentTime;
    events[*eventCount].endCycle = currentTime + 1;
    events[*eventCount].state = STATE_ACCESSED;
    exportEventRealtime(&events[*eventCount]);
    (*eventCount)++;
    currentTime++;
    remainingTime[shortestIdx]--;
    usleep(SIMULATION_DELAY_US);

    if (remainingTime[shortestIdx] == 0)
    {
      p->finishTime = currentTime;
      p->waitingTime = p->finishTime - p->arrivalTime - p->burstTime;
      p->state = STATE_TERMINATED;
      exportProcessMetric(p);
      completed++;
    }
  }

  exportSimulationEnd();
}
