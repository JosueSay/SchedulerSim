#include "ps.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

/**
 * Simula la planificación por Prioridad (PS) para un conjunto de procesos.
 *
 * @param processes    Arreglo de procesos a simular.
 * @param processCount Número de procesos en el arreglo.
 * @param events       Arreglo para registrar eventos de ejecución (TimelineEvent).
 * @param eventCount   Puntero a entero donde se actualizará el conteo de eventos generados.
 * @param control      Puntero a estructura de control de simulación, que incluye si la planificación es preventiva.
 *
 * Esta función simula la ejecución de procesos usando un esquema de planificación por prioridad.
 * Los procesos son seleccionados en cada ciclo en base a su prioridad: a menor valor, mayor prioridad.
 * En caso de empate, se considera el tiempo de llegada y la duración de la ráfaga.
 *
 * El algoritmo puede funcionar en modo preventivo o no preventivo:
 * - Si es **preventivo**, puede interrumpir un proceso en ejecución si otro con mayor prioridad llega.
 * - Si es **no preventivo**, el proceso seleccionado se ejecuta hasta completar su ráfaga.
 *
 * Durante la simulación, se registran los eventos de cada proceso (NEW, WAITING, ACCESSED, TERMINATED)
 * para representar visualmente su evolución temporal.
 * Al finalizar, se exportan las métricas individuales de cada proceso, y se marca el fin de simulación.
 */
void simulatePS(Process *processes, int processCount,
                TimelineEvent *events, int *eventCount,
                SimulationControl *control)
{
  int currentTime = 0;
  int completed = 0;
  int isPreemptive = control->config.isPreemptive;
  int remainingBurst[MAX_PROCESSES];
  *eventCount = 0;

  // Inicializar ráfagas restantes y emitir eventos NEW para procesos con llegada en 0
  for (int i = 0; i < processCount; i++)
  {
    remainingBurst[i] = processes[i].burstTime;
    if (processes[i].arrivalTime == 0)
    {
      printEventForProcess(&processes[i], 0, STATE_NEW, events, eventCount);
    }
  }

  while (completed < processCount)
  {
    // Registrar procesos que acaban de llegar (NEW)
    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime == currentTime)
      {
        printEventForProcess(&processes[i], currentTime, STATE_NEW, events, eventCount);
      }
    }

    int selectedIdx = -1;

    for (int i = 0; i < processCount; i++)
    {
      if (processes[i].arrivalTime <= currentTime &&
          processes[i].state != STATE_TERMINATED &&
          remainingBurst[i] > 0)
      {
        if (selectedIdx == -1 ||
            processes[i].priority < processes[selectedIdx].priority ||
            (processes[i].priority == processes[selectedIdx].priority &&
             (processes[i].arrivalTime < processes[selectedIdx].arrivalTime ||
              (processes[i].arrivalTime == processes[selectedIdx].arrivalTime &&
               processes[i].burstTime < processes[selectedIdx].burstTime))))
        {
          selectedIdx = i;
        }
      }
    }

    if (selectedIdx == -1)
    {
      currentTime++;
      usleep(SIMULATION_DELAY_US);
      continue;
    }

    // Registrar WAITING para todos los demás procesos activos
    for (int i = 0; i < processCount; i++)
    {
      if (i != selectedIdx &&
          processes[i].arrivalTime <= currentTime &&
          processes[i].state != STATE_TERMINATED &&
          remainingBurst[i] > 0)
      {
        printEventForProcess(&processes[i], currentTime, STATE_WAITING, events, eventCount);
      }
    }

    Process *p = &processes[selectedIdx];

    if (p->startTime == -1)
    {
      p->startTime = currentTime;
    }

    if (isPreemptive)
    {
      // Ejecuta 1 ciclo
      printEventForProcess(p, currentTime, STATE_ACCESSED, events, eventCount);
      currentTime++;
      remainingBurst[selectedIdx]--;
      usleep(SIMULATION_DELAY_US);

      if (remainingBurst[selectedIdx] == 0)
      {
        p->finishTime = currentTime;
        p->waitingTime = p->finishTime - p->arrivalTime - p->burstTime;
        p->state = STATE_TERMINATED;
        printEventForProcess(p, currentTime - 1, STATE_TERMINATED, events, eventCount);
        exportProcessMetric(p);
        completed++;
      }
    }
    else
    {
      // Ejecutar hasta terminar (no preventivo)
      for (int c = 0; c < remainingBurst[selectedIdx]; c++)
      {
        // Verificar si algún proceso llega justo en este ciclo
        for (int i = 0; i < processCount; i++)
        {
          if (processes[i].arrivalTime == currentTime)
          {
            printEventForProcess(&processes[i], currentTime, STATE_NEW, events, eventCount);
          }
        }

        // Registrar WAITING para otros procesos en este ciclo
        for (int i = 0; i < processCount; i++)
        {
          if (i != selectedIdx &&
              processes[i].arrivalTime <= currentTime &&
              processes[i].state != STATE_TERMINATED &&
              remainingBurst[i] > 0)
          {
            printEventForProcess(&processes[i], currentTime, STATE_WAITING, events, eventCount);
          }
        }

        // Ejecutar proceso actual
        printEventForProcess(p, currentTime, STATE_ACCESSED, events, eventCount);
        currentTime++;
        usleep(SIMULATION_DELAY_US);
      }

      p->finishTime = currentTime;
      p->waitingTime = p->finishTime - p->arrivalTime - p->burstTime;
      p->state = STATE_TERMINATED;
      printEventForProcess(p, currentTime - 1, STATE_TERMINATED, events, eventCount);
      exportProcessMetric(p);
      completed++;
      remainingBurst[selectedIdx] = 0;
    }
  }

  exportSimulationEnd();
}
