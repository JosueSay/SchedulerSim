#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdio.h>

/**
 * Definiciones de tamaños estándar
 */
#define COMMON_MAX_LEN 10
#define MAX_PROCESSES 100
#define MAX_EVENTS 500
#define SIMULATION_DELAY_US 1000000

/**
 * Enumeración de Algoritmos de Calendarización
 */
typedef enum
{
  ALGO_FIFO,
  ALGO_PRIORITY,
  ALGO_RR,
  ALGO_SJF,
  ALGO_SRT,
  ALGO_NONE // Por defecto si no se ha definido
} SchedulingAlgorithm;

/**
 * Configuración de la simulación (parámetros generales)
 */
typedef struct
{
  SchedulingAlgorithm algorithm;
  int quantum;
  int isPreemptive; // 0: No preemptivo, 1: Preemptivo
} SimulationConfig;

/**
 * Estructura de Control de Simulación (posible implementación para controlar timeline de eventos)
 */
typedef struct
{
  SimulationConfig config;
} SimulationControl;

/**
 * Estados de Proceso y Tipos de Acción
 */
typedef enum
{
  STATE_NEW,
  STATE_WAITING,
  STATE_ACCESSED,
  STATE_TERMINATED
} ProcessState;

/**
 * Estructura para representar un Proceso
 */
typedef struct
{
  char pid[COMMON_MAX_LEN];
  int burstTime;
  int arrivalTime;
  int priority;

  // Métricas de eficiencia
  int startTime;
  int finishTime;
  int waitingTime;

  ProcessState state;
} Process;

/**
 * Estructura para representar un Recurso
 */
typedef struct
{
  char name[COMMON_MAX_LEN];
  int counter;
  int isLocked; // 0: libre, 1: bloqueado (para sincronización)
} Resource;

typedef enum
{
  ACTION_READ,
  ACTION_WRITE,
} ActionType;

/**
 * Estructura para representar una Acción de Sincronización
 */
typedef struct
{
  char pid[COMMON_MAX_LEN];
  ActionType action;
  char resourceName[COMMON_MAX_LEN];
  int cycle;
} Action;

/**
 * Estructura para eventos de la línea de tiempo (Gantt)
 */
typedef struct
{
  char pid[COMMON_MAX_LEN];
  int startCycle;
  int endCycle;
  ProcessState state; // Estado en ese rango de tiempo (RUNNING, WAITING, etc.)
} TimelineEvent;

/**
 * Estructura para Métricas de Simulación
 */
typedef struct
{
  float avgWaitingTime;
} SimulationMetrics;

/**
 * Funciones para Métricas y Resultados
 */

// Cálculo de métricas
SimulationMetrics calculateMetrics(Process *processes, int processCount);

// Exportación de resultados
void exportTimelineEvents(const char *filename, TimelineEvent *events, int eventCount);
void exportMetrics(const char *filename, SimulationMetrics metrics);
void exportProcessMetric(const Process *p);
void exportEventRealtime(TimelineEvent *event);
void exportSimulationEnd();

// Ver timelinelog de los eventos
void printEventForProcess(Process *process, int currentTime, ProcessState state, TimelineEvent *events, int *eventCount);

/**
 * Funciones para carga y manejo de datos
 */
int loadProcesses(const char *filename, Process *processes, int maxProcesses);
int loadResources(const char *filename, Resource *resources, int maxResources);
int loadActions(const char *filename, Action *actions, int maxActions);

/**
 * Utilidades de conversión enum <-> string
 */
const char *getProcessStateName(ProcessState state);
const char *getActionTypeName(ActionType action);
const char *getAlgorithmName(SchedulingAlgorithm algorithm);

#endif
