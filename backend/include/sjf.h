#ifndef SJF_H
#define SJF_H

#include "simulator.h"

/**
 * Simula el algoritmo SJF (Shortest Job First) no expropiativo.
 * Los procesos se ejecutan uno a uno en orden de ráfaga más corta,
 * considerando solo los que han llegado hasta el ciclo actual.
 *
 * @param processes    Arreglo de procesos a simular.
 * @param processCount Número de procesos en el arreglo.
 * @param events       Arreglo donde se registran los eventos simulados.
 * @param eventCount   Puntero al contador de eventos registrados.
 * @param control      Estructura de configuración de simulación.
 */
void simulateSJF(Process *processes, int processCount,
                 TimelineEvent *events, int *eventCount,
                 SimulationControl *control);

#endif
