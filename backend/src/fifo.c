#include "fifo.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

/**
 * Simula la planificación FIFO (First In, First Out) para un conjunto de procesos.
 *
 * @param processes    Arreglo de procesos a simular.
 * @param processCount Número de procesos en el arreglo.
 * @param events       Arreglo para registrar eventos de ejecución (TimelineEvent).
 * @param eventCount   Puntero a entero donde se actualizará el conteo de eventos generados.
 * @param control      Puntero a estructura de control de simulación para actualizar estado y ciclos (puede ser NULL).
 *
 * Esta función ejecuta una simulación del algoritmo FIFO, que procesa los trabajos en orden
 * estrictamente según su tiempo de llegada (arrivalTime). Los procesos se ordenan previamente
 * para asegurar este orden.
 *
 * La simulación registra eventos en cada ciclo temporal, reflejando los estados:
 * - NEW: cuando un proceso llega.
 * - WAITING: cuando un proceso está listo pero esperando su turno.
 * - ACCESSED: cuando un proceso está en ejecución.
 * - TERMINATED: cuando un proceso termina su ráfaga.
 *
 * Entre ciclos se utiliza un retardo (usleep) para simular el paso del tiempo en tiempo real.
 * Finalmente, se exportan las métricas de cada proceso y se indica el fin de la simulación.
 */
void simulateFIFO(Process *processes, int processCount,
                  TimelineEvent *events, int *eventCount,
                  SimulationControl *control)
{
  int currentTime = 0;
  int completed = 0;
  int executing = -1; // índice del proceso actual
  int remainingBurst[MAX_PROCESSES];
  *eventCount = 0;

  for (int i = 0; i < processCount; i++)
  {
    remainingBurst[i] = processes[i].burstTime;
    processes[i].startTime = -1;
  }

  while (completed < processCount)
  {
    // Marcar procesos que acaban de llegar
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime == currentTime)
      {
        printEventForProcess(&processes[i], currentTime, STATE_NEW, events, eventCount);
      }
    }

    // Si no hay proceso en ejecución, buscar el siguiente según orden FIFO
    if (executing == -1)
    {
      int minArrival = INT_MAX;
      for (int i = 0; i < processCount; i++)
      {
        if (processes[i].state != STATE_TERMINATED &&
            processes[i].startTime == -1 &&
            processes[i].arrivalTime <= currentTime)
        {
          if (processes[i].arrivalTime < minArrival)
          {
            minArrival = processes[i].arrivalTime;
            executing = i;
          }
        }
      }

      if (executing != -1 && processes[executing].startTime == -1)
      {
        processes[executing].startTime = currentTime;
      }
    }

    // Registrar WAITING para los demás procesos activos
    for (int i = 0; i < processCount; i++)
    {
      if (i != executing &&
          processes[i].arrivalTime <= currentTime &&
          processes[i].state != STATE_TERMINATED &&
          remainingBurst[i] > 0)
      {
        printEventForProcess(&processes[i], currentTime, STATE_WAITING, events, eventCount);
      }
    }

    if (executing != -1)
    {
      printEventForProcess(&processes[executing], currentTime, STATE_ACCESSED, events, eventCount);
      remainingBurst[executing]--;

      if (remainingBurst[executing] == 0)
      {
        processes[executing].finishTime = currentTime + 1;
        processes[executing].waitingTime =
            processes[executing].finishTime - processes[executing].arrivalTime - processes[executing].burstTime;
        processes[executing].state = STATE_TERMINATED;
        printEventForProcess(&processes[executing], currentTime, STATE_TERMINATED, events, eventCount);
        exportProcessMetric(&processes[executing]);
        completed++;
        executing = -1; // liberar CPU
      }
    }

    currentTime++;
    usleep(SIMULATION_DELAY_US);
  }

  exportSimulationEnd();
}
