#include "sync_semaphore.h"

int acquireSemaphore(Resource *resource)
{
  if (resource->counter > 0)
  {
    resource->counter--;
    return 1;
  }
  return 0;
}

void releaseSemaphore(Resource *resource)
{
  resource->counter++;
}
