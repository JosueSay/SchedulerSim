#include "rr.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void simulateRR(Process *processes, int processCount,
                TimelineEvent *events, int *eventCount,
                SimulationControl *control)
{
  int currentTime = 0;
  int completed = 0;
  int quantum = control->config.quantum;
  int queue[MAX_PROCESSES];
  int queueStart = 0, queueEnd = 0;
  int remainingBurst[MAX_PROCESSES];
  *eventCount = 0;

  printf("Quantum recibido: %d\n", quantum);

  for (int i = 0; i < processCount; i++)
  {
    remainingBurst[i] = processes[i].burstTime;
  }

  while (completed < processCount)
  {
    // Agregar procesos listos a la cola
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime <= currentTime && processes[i].state == STATE_NEW)
      {
        processes[i].state = STATE_WAITING;
        queue[queueEnd++] = i;
      }
    }

    if (queueStart == queueEnd)
    {
      currentTime++;
      usleep(SIMULATION_DELAY_US);
      continue;
    }

    int idx = queue[queueStart++];
    Process *p = &processes[idx];

    if (p->startTime == -1)
    {
      p->startTime = currentTime;
    }

    int execTime = remainingBurst[idx] > quantum ? quantum : remainingBurst[idx];

    for (int c = 0; c < execTime; c++)
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

    remainingBurst[idx] -= execTime;

    if (remainingBurst[idx] == 0)
    {
      p->finishTime = currentTime;
      p->waitingTime = p->finishTime - p->arrivalTime - p->burstTime;
      p->state = STATE_TERMINATED;
      exportProcessMetric(p);
      completed++;
    }
    else
    {
      queue[queueEnd++] = idx;
    }
  }

  exportSimulationEnd();
}
