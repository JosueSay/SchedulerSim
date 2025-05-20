#include "simulator.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define SIMULATION_DELAY_US 100000

void simulateSJF(Process *processes, int processCount,
                 TimelineEvent *events, int *eventCount,
                 SimulationControl *control)
{
  int currentTime = 0;
  int completed = 0;
  *eventCount = 0;

  while (completed < processCount)
  {
    int shortestIdx = -1;

    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime <= currentTime && processes[i].state == STATE_NEW)
      {
        if (shortestIdx == -1 || processes[i].burstTime < processes[shortestIdx].burstTime)
        {
          shortestIdx = i;
        }
      }
    }

    if (shortestIdx == -1)
    {
      currentTime++;
      usleep(SIMULATION_DELAY_US);
      continue;
    }

    Process *p = &processes[shortestIdx];
    p->startTime = currentTime;
    p->finishTime = currentTime + p->burstTime;
    p->waitingTime = p->startTime - p->arrivalTime;
    p->state = STATE_TERMINATED;

    for (int c = 0; c < p->burstTime; c++)
    {
      snprintf(events[*eventCount].pid, PID_MAX_LEN, "%s", p->pid);
      events[*eventCount].startCycle = currentTime;
      events[*eventCount].endCycle = currentTime + 1;
      events[*eventCount].state = STATE_ACCESSED;
      exportEventRealtime(&events[*eventCount]);
      (*eventCount)++;
      currentTime++;
      usleep(SIMULATION_DELAY_US);
    }

    exportProcessMetric(p);
    completed++;
  }

  exportSimulationEnd();
}
