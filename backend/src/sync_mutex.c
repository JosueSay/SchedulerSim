#include "sync_mutex.h"

int acquireMutex(Resource *resource)
{
  if (resource->isLocked == 0)
  {
    resource->isLocked = 1;
    return 1;
  }
  return 0;
}

void releaseMutex(Resource *resource)
{
  resource->isLocked = 0;
}
