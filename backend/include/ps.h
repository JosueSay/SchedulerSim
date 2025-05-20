#ifndef PS_H
#define PS_H

#include "simulator.h"

/**
 * Simula el algoritmo Priority Scheduling (PS), con soporte para modo preventivo y no preventivo.
 *
 * @param processes    Arreglo de procesos a simular.
 * @param processCount Número de procesos en el arreglo.
 * @param events       Arreglo donde se registran eventos simulados.
 * @param eventCount   Puntero al contador de eventos registrados.
 * @param control      Estructura con la configuración (incluye isPreemptive).
 */
void simulatePS(Process *processes, int processCount,
                TimelineEvent *events, int *eventCount,
                SimulationControl *control);

#endif
