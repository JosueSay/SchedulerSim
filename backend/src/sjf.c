#include "simulator.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * Simula la planificación SJF (Shortest Job First) no expropiativa en tiempo real para un conjunto de procesos.
 *
 * @param processes    Arreglo de procesos a simular.
 * @param processCount Número de procesos en el arreglo.
 * @param events       Arreglo para registrar eventos de ejecución (TimelineEvent).
 * @param eventCount   Puntero a entero donde se actualizará el conteo de eventos generados.
 * @param control      Puntero a estructura de control de simulación (puede ser NULL).
 *
 * Esta función selecciona el proceso con el menor tiempo de ráfaga (burst time) entre los que ya han llegado
 * y lo ejecuta completamente antes de seleccionar el siguiente. Cada ejecución se simula ciclo a ciclo,
 * se exportan eventos en tiempo real y se generan métricas por proceso.
 * Se introduce un retardo con `usleep` para simular tiempo real.
 */
void simulateSJF(Process *processes, int processCount,
                 TimelineEvent *events, int *eventCount,
                 SimulationControl *control)
{
  int currentTime = 0;
  int completed = 0;
  *eventCount = 0;

  // Emitir NEW si llegaron en tiempo 0
  for (int i = 0; i < processCount; i++)
  {
    if (processes[i].arrivalTime == 0)
    {
      printEventForProcess(&processes[i], 0, STATE_NEW, events, eventCount);
    }
  }

  while (completed < processCount)
  {
    // Registrar procesos que llegan justo en este ciclo
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime == currentTime)
      {
        printEventForProcess(&processes[i], currentTime, STATE_NEW, events, eventCount);
      }
    }

    int shortestIdx = -1;

    // Buscar proceso con menor burst que ya haya llegado y no haya terminado
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime <= currentTime &&
          processes[i].state != STATE_TERMINATED)
      {
        if (shortestIdx == -1 || processes[i].burstTime < processes[shortestIdx].burstTime)
        {
          shortestIdx = i;
        }
      }
    }

    // No hay procesos listos aún
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

    // Ejecutar el proceso ciclo por ciclo
    for (int c = 0; c < p->burstTime; c++)
    {
      // Registrar procesos en WAITING
      for (int i = 0; i < processCount; i++)
      {
        if (i != shortestIdx &&
            processes[i].arrivalTime <= currentTime &&
            processes[i].state != STATE_TERMINATED)
        {
          printEventForProcess(&processes[i], currentTime, STATE_WAITING, events, eventCount);
        }
      }

      // Ejecutar proceso actual
      printEventForProcess(p, currentTime, STATE_ACCESSED, events, eventCount);
      currentTime++;
      usleep(SIMULATION_DELAY_US);

      // Registrar procesos que llegan justo ahora
      for (int i = 0; i < processCount; i++)
      {
        if (processes[i].arrivalTime == currentTime)
        {
          printEventForProcess(&processes[i], currentTime, STATE_NEW, events, eventCount);
        }
      }
    }

    // Termina el proceso
    p->state = STATE_TERMINATED;
    printEventForProcess(p, currentTime - 1, STATE_TERMINATED, events, eventCount);
    exportProcessMetric(p);
    completed++;
  }

  exportSimulationEnd();
}