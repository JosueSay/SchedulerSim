#include "fifo.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h> // Solo si quieres simular delay con sleep

/**
 * Exporta un solo evento en formato JSON (para streaming en tiempo real)
 */
void exportEventRealtime(TimelineEvent *event)
{
  printf("{\"pid\": \"%s\", \"startCycle\": %d, \"endCycle\": %d, \"state\": \"%s\"}\n",
         event->pid,
         event->startCycle,
         event->endCycle,
         getProcessStateName(event->state));
  fflush(stdout); // Asegura que el evento se envíe inmediatamente
}

/**
 * Exporta un evento de finalización de simulación
 */
void exportSimulationEnd()
{
  printf("{\"event\": \"SIMULATION_END\"}\n");
  fflush(stdout);
}

/**
 * Simulación del algoritmo FIFO (First-In, First-Out) en tiempo real
 */
void simulateFIFO(Process *processes, int processCount,
                  TimelineEvent *events, int *eventCount,
                  SimulationControl *control)
{
  int currentTime = 0;
  *eventCount = 0;

  // Ordenar procesos por tiempo de llegada (Arrival Time)
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

    // Generar eventos ciclo por ciclo para animación en tiempo real
    for (int c = 0; c < processes[i].burstTime; c++)
    {
      snprintf(events[*eventCount].pid, PID_MAX_LEN, "%s", processes[i].pid);
      events[*eventCount].startCycle = currentTime;
      events[*eventCount].endCycle = currentTime + 1;
      events[*eventCount].state = STATE_RUNNING;

      exportEventRealtime(&events[*eventCount]); // Emitir evento en tiempo real

      (*eventCount)++;
      currentTime++;

      // Simula delay real si quieres ver la animación en frontend en tiempo real
      // sleep(1); // Descomenta si deseas simular 1 segundo de espera por ciclo
    }
  }

  // Emitir fin de simulación para que el frontend pueda manejarlo
  exportSimulationEnd();

  // Actualizar control de simulación
  if (control)
  {
    control->currentCycle = currentTime;
    control->totalCycles = currentTime;
    control->state = SIMULATION_FINISHED;
  }
}
