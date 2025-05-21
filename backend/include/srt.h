#ifndef SRT_H
#define SRT_H

#include "simulator.h"

/**
 * Simula el algoritmo SRT (Shortest Remaining Time).
 * Es una versión preventiva del algoritmo SJF.
 *
 * @param processes    Arreglo de procesos a simular.
 * @param processCount Número de procesos.
 * @param events       Arreglo para registrar eventos ciclo a ciclo.
 * @param eventCount   Puntero al contador de eventos registrados.
 * @param control      Configuración de la simulación.
 */
void simulateSRT(Process *processes, int processCount,
                 TimelineEvent *events, int *eventCount,
                 SimulationControl *control);

#endif
