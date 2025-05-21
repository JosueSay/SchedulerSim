#ifndef FIFO_H
#define FIFO_H

#include "simulator.h"

/**
 * Simula el algoritmo de planificación FIFO (First-In, First-Out).
 * Ejecuta los procesos en el orden en que llegan, sin expropiación.
 * Cada proceso se ejecuta completamente antes de pasar al siguiente.
 *
 * @param processes    Arreglo de procesos a simular.
 * @param processCount Cantidad total de procesos en la simulación.
 * @param events       Arreglo donde se registran los eventos del ciclo de simulación.
 * @param eventCount   Puntero al contador total de eventos registrados.
 * @param control      Estructura de configuración de la simulación (tiempo actual, flags, etc).
 */
void simulateFIFO(Process *processes, int processCount,
                  TimelineEvent *events, int *eventCount,
                  SimulationControl *control);

#endif
