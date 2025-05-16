#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include "simulator.h"

// Inicializar los recursos para la simulación
void initializeResources(Resource *resources, int resourceCount);

// Manejo de Mutex
int acquireMutex(Resource *resource, const char *pid);
void releaseMutex(Resource *resource);

// Manejo de Semáforos
int acquireSemaphore(Resource *resource, const char *pid);
void releaseSemaphore(Resource *resource);

#endif
