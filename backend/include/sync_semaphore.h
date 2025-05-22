#ifndef SYNC_SEMAPHORE_H
#define SYNC_SEMAPHORE_H

#include "simulator.h"

int acquireSemaphore(Resource *resource);
void releaseSemaphore(Resource *resource);

#endif
