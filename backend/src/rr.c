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
 * Esta función implementa la planificación Round Robin, que asigna tiempo de CPU a los procesos
 * por turnos de duración fija (quantum), en orden de llegada. El algoritmo considera el tiempo de
 * llegada de cada proceso, y los coloca en una cola a medida que van llegando.
 *
 * En cada ciclo de reloj:
 *  - Se agregan nuevos procesos a la cola si han llegado en ese tiempo.
 *  - Se registra el estado de espera (STATE_WAITING) para los procesos en cola que no están ejecutando.
 *  - Se ejecuta el proceso en turno por hasta `quantum` ciclos o hasta que termine su ráfaga.
 *  - Si el proceso no termina dentro del quantum, se reencola al final.
 *
 * Durante la simulación:
 *  - Se generan eventos STATE_NEW, STATE_WAITING, STATE_ACCESSED y STATE_TERMINATED.
 *  - Se actualizan métricas como tiempo de inicio, finalización y espera.
 *  - Se introduce una pausa con `usleep` para simular el paso del tiempo.
 *
 * Una vez que todos los procesos han terminado, se exporta el fin de la simulación.
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
    // Agregar procesos nuevos al queue y registrar NEW
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime == currentTime && processes[i].state == STATE_NEW)
      {
        printEventForProcess(&processes[i], currentTime, STATE_NEW, events, eventCount);
        processes[i].state = STATE_WAITING;
        queue[queueEnd++] = i;
      }
    }

    // Registrar WAITING para procesos en cola que no están ejecutando
    for (int i = queueStart; i < queueEnd; i++)
    {
      if (queue[i] != currentProcess)
      {
        printEventForProcess(&processes[queue[i]], currentTime, STATE_WAITING, events, eventCount);
      }
    }

    // Si no hay proceso actual o se agotó el quantum
    if (currentProcess == -1 || quantumCounter == quantum)
    {
      if (currentProcess != -1 && remainingBurst[currentProcess] > 0)
      {
        queue[queueEnd++] = currentProcess;
      }

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
        currentProcess = -1;
      }
    }

    // Espera implícita para procesos activos pero no ejecutando en este ciclo
    for (int i = 0; i < processCount; i++)
    {
      if (i != currentProcess &&
          processes[i].arrivalTime <= currentTime &&
          processes[i].startTime != -1 &&
          processes[i].state != STATE_TERMINATED &&
          remainingBurst[i] > 0)
      {
        printEventForProcess(&processes[i], currentTime, STATE_WAITING, events, eventCount);
      }
    }

    if (currentProcess != -1)
    {
      // Ejecutar proceso
      printEventForProcess(&processes[currentProcess], currentTime, STATE_ACCESSED, events, eventCount);
      remainingBurst[currentProcess]--;
      quantumCounter++;

      if (remainingBurst[currentProcess] == 0)
      {
        processes[currentProcess].finishTime = currentTime + 1;
        processes[currentProcess].waitingTime =
            processes[currentProcess].finishTime - processes[currentProcess].arrivalTime - processes[currentProcess].burstTime;
        processes[currentProcess].state = STATE_TERMINATED;
        printEventForProcess(&processes[currentProcess], currentTime, STATE_TERMINATED, events, eventCount);
        exportProcessMetric(&processes[currentProcess]);
        completed++;
        currentProcess = -1;
        quantumCounter = 0;
      }
    }

    currentTime++;
    usleep(SIMULATION_DELAY_US);
  }
}
