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

  // Ordenar por arrivalTime
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

  // Inicializar NEW en su ciclo de llegada para todos los procesos
  for (int i = 0; i < processCount; i++)
  {
    printEventForProcess(&processes[i], processes[i].arrivalTime, STATE_NEW, events, eventCount);
  }

  for (int i = 0; i < processCount; i++)
  {
    // Si el proceso aún no ha llegado, avanza el tiempo registrando espera para otros
    while (currentTime < processes[i].arrivalTime)
    {
      for (int j = 0; j < processCount; j++)
      {
        if (j != i &&
            processes[j].arrivalTime <= currentTime &&
            processes[j].startTime == -1)
        {
          printEventForProcess(&processes[j], currentTime, STATE_WAITING, events, eventCount);
        }
      }
      currentTime++;
    }

    // Empieza ejecución
    processes[i].startTime = currentTime;
    processes[i].finishTime = currentTime + processes[i].burstTime;
    processes[i].waitingTime = processes[i].startTime - processes[i].arrivalTime;

    // Si el proceso ya estaba esperando (llegó antes de currentTime), registramos esos ciclos como WAITING
    for (int w = processes[i].arrivalTime; w < processes[i].startTime; w++)
    {
      printEventForProcess(&processes[i], w, STATE_WAITING, events, eventCount);
    }

    // Ciclos de ejecución
    for (int c = 0; c < processes[i].burstTime; c++)
    {
      printEventForProcess(&processes[i], currentTime, STATE_ACCESSED, events, eventCount);
      currentTime++;
      usleep(SIMULATION_DELAY_US);
    }

    processes[i].state = STATE_TERMINATED;
    printEventForProcess(&processes[i], currentTime - 1, STATE_TERMINATED, events, eventCount);
    exportProcessMetric(&processes[i]);
  }

  exportSimulationEnd();
}
