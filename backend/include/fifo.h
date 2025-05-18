#ifndef FIFO_H
#define FIFO_H

#include "simulator.h"

// Función principal para simular FIFO en tiempo real (ciclo por ciclo)
void simulateFIFO(Process *processes, int processCount,
                  TimelineEvent *events, int *eventCount,
                  SimulationControl *control);

// Exporta un evento en formato JSON en tiempo real
void exportEventRealtime(TimelineEvent *event);

// Notifica el fin de la simulación en tiempo real
void exportSimulationEnd();

#endif
