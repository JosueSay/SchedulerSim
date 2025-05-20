#include "ps.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

void simulatePS(Process *processes, int processCount,
                TimelineEvent *events, int *eventCount,
                SimulationControl *control)
{
  int currentTime = 0;
  int completed = 0;
  int isPreemptive = control->config.isPreemptive;
  int remainingBurst[MAX_PROCESSES];
  *eventCount = 0;

  for (int i = 0; i < processCount; i++)
  {
    remainingBurst[i] = processes[i].burstTime;
  }

  while (completed < processCount)
  {
    int highestPriority = INT_MAX;
    int selectedIdx = -1;

    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime <= currentTime && processes[i].state != STATE_TERMINATED && remainingBurst[i] > 0)
      {
        if (processes[i].priority < highestPriority ||
            (processes[i].priority == highestPriority && processes[i].arrivalTime < processes[selectedIdx].arrivalTime))
        {
          highestPriority = processes[i].priority;
          selectedIdx = i;
        }
      }
    }

    if (selectedIdx == -1)
    {
      currentTime++;
      usleep(SIMULATION_DELAY_US);
      continue;
    }

    Process *p = &processes[selectedIdx];
    if (p->startTime == -1)
    {
      p->startTime = currentTime;
    }

    snprintf(events[*eventCount].pid, PID_MAX_LEN, "%s", p->pid);
    events[*eventCount].startCycle = currentTime;
    events[*eventCount].state = STATE_ACCESSED;

    if (isPreemptive)
    {
      // Ejecuta 1 ciclo (modo preventivo)
      currentTime++;
      remainingBurst[selectedIdx]--;

      events[*eventCount].endCycle = currentTime;
      exportEventRealtime(&events[*eventCount]);
      (*eventCount)++;
      usleep(SIMULATION_DELAY_US);

      if (remainingBurst[selectedIdx] == 0)
      {
        p->finishTime = currentTime;
        p->waitingTime = p->finishTime - p->arrivalTime - p->burstTime;
        p->state = STATE_TERMINATED;
        exportProcessMetric(p);
        completed++;
      }
    }

    else
    {
      // Ejecuta completamente (modo no preventivo)
      for (int c = 0; c < remainingBurst[selectedIdx]; c++)
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

      p->finishTime = currentTime;
      p->waitingTime = p->finishTime - p->arrivalTime - p->burstTime;
      p->state = STATE_TERMINATED;
      exportProcessMetric(p);
      completed++;
      remainingBurst[selectedIdx] = 0;
    }

    exportEventRealtime(&events[*eventCount - 1]);
  }

  exportSimulationEnd();
}
