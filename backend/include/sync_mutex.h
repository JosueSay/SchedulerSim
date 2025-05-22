#ifndef SYNC_MUTEX_H
#define SYNC_MUTEX_H

#include "simulator.h"

int acquireMutex(Resource *resource);
void releaseMutex(Resource *resource);

#endif
