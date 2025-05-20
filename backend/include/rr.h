#ifndef RR_H
#define RR_H

#include "simulator.h"

/**
 * Simula el algoritmo Round Robin (RR) no expropiativo.
 * Usa un quantum fijo para ejecutar procesos en ciclos.
 *
 * @param processes    Arreglo de procesos a simular.
 * @param processCount Cantidad total de procesos.
 * @param events       Arreglo donde se registran eventos simulados.
 * @param eventCount   Puntero al contador de eventos registrados.
 * @param control      Estructura de configuración con el quantum.
 */
void simulateRR(Process *processes, int processCount,
                TimelineEvent *events, int *eventCount,
                SimulationControl *control);

#endif
