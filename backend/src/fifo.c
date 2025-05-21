#include "fifo.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/**
 * Simula la planificación FIFO (First In, First Out) en tiempo real para un conjunto de procesos.
 *
 * @param processes    Arreglo de procesos a simular.
 * @param processCount Número de procesos en el arreglo.
 * @param events       Arreglo para registrar eventos de ejecución (TimelineEvent).
 * @param eventCount   Puntero a entero donde se actualizará el conteo de eventos generados.
 * @param control      Puntero a estructura de control de simulación para actualizar estado y ciclos (puede ser NULL).
 *
 * Esta función ordena los procesos por tiempo de llegada, simula su ejecución uno a uno,
 * registra eventos ciclo a ciclo con retardo para simular tiempo real (usleep),
 * exporta eventos en formato JSON y al final actualiza el control de simulación.
 */
void simulateFIFO(Process *processes, int processCount,
                  TimelineEvent *events, int *eventCount,
                  SimulationControl *control)
{
  int currentTime = 0;
  *eventCount = 0;

  // Ordenar por tiempo de llegada (Arrival Time)
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

  // Ejecutar procesos en orden FIFO
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

    // Simular ciclo por ciclo
    for (int c = 0; c < processes[i].burstTime; c++)
    {
      snprintf(events[*eventCount].pid, COMMON_MAX_LEN, "%s", processes[i].pid);
      events[*eventCount].startCycle = currentTime;
      events[*eventCount].endCycle = currentTime + 1;
      events[*eventCount].state = STATE_ACCESSED;

      exportEventRealtime(&events[*eventCount]);

      (*eventCount)++;
      currentTime++;

      usleep(SIMULATION_DELAY_US); // delay para frontend
    }

    // Enviar métrica individual del proceso
    exportProcessMetric(&processes[i]);
  }

  exportSimulationEnd();
}