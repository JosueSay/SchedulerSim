#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include "simulator.h"

void initializeResources(Resource *resources, int resourceCount);

/**
 * Ejecuta la simulación sincronizada, usando mutex o semáforo.
 * @param useMutex 1 si es con mutex, 0 si es con semáforo
 */
void simulateSynchronization(Process *processes, int processCount,
                             Resource *resources, int resourceCount,
                             Action *actions, int actionCount,
                             TimelineEvent *events, int *eventCount,
                             int useMutex);

#endif
