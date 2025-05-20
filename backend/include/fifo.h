#ifndef FIFO_H
#define FIFO_H

#include "simulator.h"

// Función principal para simular FIFO en tiempo real (ciclo por ciclo)
void simulateFIFO(Process *processes, int processCount,
                  TimelineEvent *events, int *eventCount,
                  SimulationControl *control);

#endif
