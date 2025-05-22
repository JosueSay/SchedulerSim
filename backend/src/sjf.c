#include "simulator.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Simula la planificación SJF (Shortest Job First) no expropiativa en tiempo real para un conjunto de procesos.
 *
 * @param processes    Arreglo de procesos a simular.
 * @param processCount Número de procesos en el arreglo.
 * @param events       Arreglo para registrar eventos de ejecución (TimelineEvent).
 * @param eventCount   Puntero a entero donde se actualizará el conteo de eventos generados.
 * @param control      Puntero a estructura de control de simulación (puede ser NULL).
 *
 * Esta función implementa la planificación SJF no expropiativa, donde en cada ciclo se selecciona el proceso
 * con el menor tiempo de ráfaga (burstTime) entre los que han llegado y aún no han terminado.
 *  - En caso de empate entre procesos con el mismo `burstTime`, se selecciona aquel que haya llegado **antes** (`arrivalTime` menor).
 *
 * Una vez seleccionado, el proceso se ejecuta completamente sin ser interrumpido. Durante su ejecución:
 *  - Se registran los eventos de espera (STATE_WAITING) para los demás procesos que ya llegaron.
 *  - Se registran los eventos de acceso (STATE_ACCESSED) ciclo a ciclo para el proceso en ejecución.
 *  - Se detectan nuevos procesos que llegan y se marcan como STATE_NEW en su ciclo exacto de llegada.
 *
 * Al finalizar la ejecución de un proceso, se actualiza su tiempo de finalización, tiempo de espera,
 * se marca como terminado (STATE_TERMINATED) y se exportan sus métricas.
 *
 * Se introduce un retardo artificial con `usleep` para simular ejecución en tiempo real y generar
 * una traza animada de la planificación. Al finalizar todos los procesos, se exporta el fin de simulación.
 */
void simulateSJF(Process *processes, int processCount,
                 TimelineEvent *events, int *eventCount,
                 SimulationControl *control)
{
  int currentTime = 0;
  int completed = 0;
  *eventCount = 0;
  bool newPrinted[MAX_PROCESSES] = {false};

  // NEW si llegaron en tiempo 0
  for (int i = 0; i < processCount; i++)
  {
    if (processes[i].arrivalTime == 0)
    {
      printEventForProcess(&processes[i], 0, STATE_NEW, events, eventCount);
      newPrinted[i] = true;
    }
  }
  while (completed < processCount)
  {
    // Registrar procesos que llegan justo en este ciclo
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime == currentTime && !newPrinted[i])
      {
        printEventForProcess(&processes[i], currentTime, STATE_NEW, events, eventCount);
        newPrinted[i] = true;
      }
    }

    int shortestIdx = -1;

    // Buscar proceso con menor burst que ya haya llegado y no haya terminado
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime <= currentTime &&
          processes[i].state != STATE_TERMINATED)
      {
        if (shortestIdx == -1 ||
            processes[i].burstTime < processes[shortestIdx].burstTime ||
            (processes[i].burstTime == processes[shortestIdx].burstTime &&
             processes[i].arrivalTime < processes[shortestIdx].arrivalTime))
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
}