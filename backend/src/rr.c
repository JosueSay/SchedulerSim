#include "rr.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/**
 * Simula la planificación Round Robin (RR) en tiempo real para un conjunto de procesos.
 *
 * @param processes    Arreglo de procesos a simular.
 * @param processCount Número de procesos en el arreglo.
 * @param events       Arreglo para registrar eventos de ejecución (TimelineEvent).
 * @param eventCount   Puntero a entero donde se actualizará el conteo de eventos generados.
 * @param control      Puntero a estructura de control de simulación con parámetros como el quantum.
 *
 * Esta función simula la ejecución de procesos en un esquema Round Robin,
 * considerando el tiempo de llegada y un quantum fijo. Los procesos se encolan
 * conforme van llegando y se les asigna CPU por turnos de longitud máxima igual al quantum.
 *
 * Durante la ejecución, se registran eventos ciclo a ciclo para su visualización en tiempo real.
 * Se exportan los eventos y métricas individuales de cada proceso, y se finaliza con una marca de fin de simulación.
 */
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
  int quantumCounter = 0;
  int currentProcess = -1;
  *eventCount = 0;

  printf("Quantum recibido: %d\n", quantum);

  for (int i = 0; i < processCount; i++)
  {
    remainingBurst[i] = processes[i].burstTime;
  }

  while (completed < processCount)
  {
    // Agregar procesos nuevos al queue
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime == currentTime && processes[i].state == STATE_NEW)
      {
        processes[i].state = STATE_WAITING;
        queue[queueEnd++] = i;
      }
    }

    // Si no hay proceso actual o se agotó el quantum
    if (currentProcess == -1 || quantumCounter == quantum)
    {
      // Si el proceso actual aún no ha terminado, reencolarlo
      if (currentProcess != -1 && remainingBurst[currentProcess] > 0)
      {
        queue[queueEnd++] = currentProcess;
      }

      // Obtener siguiente proceso de la cola
      if (queueStart < queueEnd)
      {
        currentProcess = queue[queueStart++];
        quantumCounter = 0;

        if (processes[currentProcess].startTime == -1)
        {
          processes[currentProcess].startTime = currentTime;
        }
      }
      else
      {
        currentProcess = -1; // no hay proceso actual
      }
    }

    if (currentProcess != -1)
    {
      // Ejecutar el proceso actual por 1 ciclo
      snprintf(events[*eventCount].pid, PID_MAX_LEN, "%s", processes[currentProcess].pid);
      events[*eventCount].startCycle = currentTime;
      events[*eventCount].endCycle = currentTime + 1;
      events[*eventCount].state = STATE_ACCESSED;
      exportEventRealtime(&events[*eventCount]);
      (*eventCount)++;

      remainingBurst[currentProcess]--;
      quantumCounter++;

      if (remainingBurst[currentProcess] == 0)
      {
        processes[currentProcess].finishTime = currentTime + 1;
        processes[currentProcess].waitingTime =
            processes[currentProcess].finishTime - processes[currentProcess].arrivalTime - processes[currentProcess].burstTime;
        processes[currentProcess].state = STATE_TERMINATED;
        exportProcessMetric(&processes[currentProcess]);
        completed++;
        currentProcess = -1;
        quantumCounter = 0;
      }
    }

    currentTime++;
    usleep(SIMULATION_DELAY_US);
  }

  exportSimulationEnd();
}
