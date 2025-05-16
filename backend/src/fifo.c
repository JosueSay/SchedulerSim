#include "fifo.h"
#include <stdio.h>
#include <string.h>

/**
 * Simulación del algoritmo FIFO (First-In, First-Out)
 */
void simulateFIFO(Process *processes, int processCount,
                  TimelineEvent *events, int *eventCount,
                  SimulationControl *control)
{
  int currentTime = 0;
  *eventCount = 0;

  // Ordenar procesos por Arrival Time (AT)
  for (int i = 0; i < processCount - 1; i++)
  {
    for (int j = i + 1; j < processCount; j++)
    {
      if (processes[i].arrivalTime > processes[j].arrivalTime)
      {
        Process temp = processes[i];
        processes[i] = processes[j];
        processes[j] = temp;
      }
    }
  }

  for (int i = 0; i < processCount; i++)
  {
    if (currentTime < processes[i].arrivalTime)
    {
      currentTime = processes[i].arrivalTime;
    }

    processes[i].startTime = currentTime;
    processes[i].finishTime = currentTime + processes[i].burstTime;
    processes[i].waitingTime = processes[i].startTime - processes[i].arrivalTime;
    processes[i].state = STATE_TERMINATED;

    // Agregar evento a la línea de tiempo
    snprintf(events[*eventCount].pid, PID_MAX_LEN, "%s", processes[i].pid);
    events[*eventCount].startCycle = processes[i].startTime;
    events[*eventCount].endCycle = processes[i].finishTime;
    events[*eventCount].state = STATE_RUNNING;
    (*eventCount)++;

    currentTime = processes[i].finishTime;
  }

  // Actualizar control de simulación
  if (control)
  {
    control->currentCycle = currentTime;
    control->totalCycles = currentTime;
    control->state = SIMULATION_FINISHED;
  }
}
