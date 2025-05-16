#include "synchronization.h"
#include <string.h>

void initializeResources(Resource *resources, int resourceCount)
{
  for (int i = 0; i < resourceCount; i++)
  {
    resources[i].isLocked = 0;
  }
}

int acquireMutex(Resource *resource, const char *pid)
{
  if (resource->isLocked == 0)
  {
    resource->isLocked = 1;
    return 1; // Éxito
  }
  return 0; // Fallo, recurso ocupado
}

void releaseMutex(Resource *resource)
{
  resource->isLocked = 0;
}

int acquireSemaphore(Resource *resource, const char *pid)
{
  if (resource->counter > 0)
  {
    resource->counter--;
    return 1; // Éxito
  }
  return 0; // Fallo, no hay instancias disponibles
}

void releaseSemaphore(Resource *resource)
{
  resource->counter++;
}
