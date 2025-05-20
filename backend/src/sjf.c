#include "simulator.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Configuración de Simulación
 */
#define SIMULATION_DELAY_US 100000

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

  while (completed < processCount)
  {
    int shortestIdx = -1;

    // Buscar el proceso con menor burstTime que haya llegado y no se haya ejecutado
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

    // Si no hay procesos listos, avanzar el tiempo
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

    // Simular ejecución del proceso ciclo por ciclo
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
