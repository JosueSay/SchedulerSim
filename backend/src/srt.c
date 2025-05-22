#include "srt.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>

/**
 * Simula la planificación SRT (Shortest Remaining Time) en tiempo real para un conjunto de procesos.
 *
 * @param processes     Arreglo de procesos a simular.
 * @param processCount  Número total de procesos en el arreglo.
 * @param events        Arreglo para registrar eventos de ejecución (TimelineEvent).
 * @param eventCount    Puntero a entero donde se almacenará la cantidad de eventos generados.
 * @param control       Puntero a estructura de control de simulación (puede ser NULL si no se utiliza).
 *
 * Esta función implementa el algoritmo SRT, una versión con desalojo del algoritmo SJF, donde
 * en cada ciclo se elige el proceso con el menor tiempo restante de ejecución.
 * La simulación avanza ciclo a ciclo, registrando eventos de llegada, espera, ejecución y finalización.
 *
 * Características:
 * - Se simula segundo a segundo con `usleep` para imitar tiempo real.
 * - Registra eventos de tipo NEW cuando un proceso llega.
 * - Selecciona en cada ciclo el proceso con el menor tiempo restante (que haya llegado).
 * - Si no hay proceso ejecutable, simplemente avanza el tiempo.
 * - Actualiza los estados de los procesos y exporta métricas al final de su ejecución.
 * - Se consideran eventos de espera (WAITING) para procesos activos que no están ejecutando.
 * - Al completar todos los procesos, exporta el fin de la simulación.
 *
 * Criterios de desempate:
 * - En caso de empate en `remainingTime`, se continúa ejecutando el proceso que ya estaba en ejecución.
 * - Si ninguno estaba en ejecución, se elige el de mayor prioridad (valor `priority` menor).
 */
void simulateSRT(Process *processes, int processCount,
                 TimelineEvent *events, int *eventCount,
                 SimulationControl *control)
{
  int currentTime = 0;
  int completed = 0;
  int remainingTime[MAX_PROCESSES];
  *eventCount = 0;
  bool newPrinted[MAX_PROCESSES] = {false};
  int lastExecutedIdx = -1;

  for (int i = 0; i < processCount; i++)
  {
    remainingTime[i] = processes[i].burstTime;
    processes[i].startTime = -1;

    if (processes[i].arrivalTime == 0)
    {
      printEventForProcess(&processes[i], 0, STATE_NEW, events, eventCount);
      newPrinted[i] = true;
    }
  }

  while (completed < processCount)
  {
    // Registrar procesos que acaban de llegar
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime == currentTime && !newPrinted[i])
      {
        printEventForProcess(&processes[i], currentTime, STATE_NEW, events, eventCount);
        newPrinted[i] = true;
      }
    }

    int shortestIdx = -1;
    int minRemaining = INT_MAX;

    // Escoger el proceso con menor bt a ejecutar
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime <= currentTime &&
          processes[i].state != STATE_TERMINATED &&
          remainingTime[i] > 0)
      {
        if (remainingTime[i] < minRemaining)
        {
          minRemaining = remainingTime[i];
          shortestIdx = i;
        }
        else if (remainingTime[i] == minRemaining) // empate bt
        {
          // continuar con el proceso que ya estaba ejecutando
          if (i == lastExecutedIdx)
          {
            shortestIdx = i;
          }
          // Si ninguno estaba ejecutando, elegimos el de mayor prioridad (menor valor)
          else if (shortestIdx != lastExecutedIdx && processes[i].priority < processes[shortestIdx].priority)
          {
            shortestIdx = i;
          }
        }
      }
    }

    if (shortestIdx == -1)
    {
      currentTime++;
      usleep(SIMULATION_DELAY_US);
      continue;
    }

    // Registrar estados de espera para los otros procesos activos
    for (int i = 0; i < processCount; i++)
    {
      if (i != shortestIdx &&
          processes[i].arrivalTime <= currentTime &&
          processes[i].state != STATE_TERMINATED &&
          remainingTime[i] > 0)
      {
        printEventForProcess(&processes[i], currentTime, STATE_WAITING, events, eventCount);
      }
    }

    Process *p = &processes[shortestIdx];

    if (p->startTime == -1)
      p->startTime = currentTime;

    // Ejecutar 1 ciclo
    printEventForProcess(p, currentTime, STATE_ACCESSED, events, eventCount);
    currentTime++;
    remainingTime[shortestIdx]--;
    lastExecutedIdx = shortestIdx;
    usleep(SIMULATION_DELAY_US);

    // Si termina
    if (remainingTime[shortestIdx] == 0)
    {
      p->finishTime = currentTime;
      p->waitingTime = p->finishTime - p->arrivalTime - p->burstTime;
      p->state = STATE_TERMINATED;
      printEventForProcess(p, currentTime - 1, STATE_TERMINATED, events, eventCount);
      exportProcessMetric(p);
      completed++;
    }
  }
}