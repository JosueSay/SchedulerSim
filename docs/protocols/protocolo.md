# Protocolo de Comunicación – SchedulerSim (Backend/Frontend)

## 1. Estructuras de Entrada

### 1.1 Procesos (Archivo: `procesos.txt`)

- **Formato por línea:**  
  `<PID>, <BT>, <AT>, <Priority>`

- **Campos:**
  - `PID`: Identificador del proceso (ej. `P1`).
  - `BT`: Burst Time (tiempo de ejecución).
  - `AT`: Arrival Time (tiempo de llegada).
  - `Priority`: Prioridad del proceso (menor número = mayor prioridad).

- **Ejemplo:**

```bash
P1, 8, 0, 1
P2, 4, 2, 2
P3, 6, 4, 1
```

### 1.2 Recursos (Archivo: `recursos.txt`)

- **Formato por línea:**  
`<NOMBRE_RECURSO>, <CONTADOR>`

- **Campos:**
- `NOMBRE_RECURSO`: Nombre del recurso (ej. `R1`).
- `CONTADOR`: Cantidad disponible del recurso (ej. `1` para mutex).

- **Ejemplo:**

```bash
R1, 1
R2, 2
```

### 1.3 Acciones (Archivo: `acciones.txt`)

- **Formato por línea:**  
`<PID>, <ACCION>, <RECURSO>, <CICLO>`

- **Campos:**
- `PID`: Identificador del proceso.
- `ACCION`: Acción a realizar (`READ` o `WRITE`).
- `RECURSO`: Recurso al que se accede.
- `CICLO`: Ciclo de la línea de tiempo en el que ocurre la acción.

- **Ejemplo:**

```bash
P1, READ, R1, 0
P2, WRITE, R2, 3
P1, READ, R1, 5
```

## 2. Estructuras de Salida

### 2.1 Resultados de Simulación de Calendarización (Archivo: `resultado_scheduling.json`)

- **Formato JSON:**

```json
{
"algorithm": "Round Robin",
"quantum": 2,
"execution_order": [
  {"pid": "P1", "start_cycle": 0, "end_cycle": 2},
  {"pid": "P2", "start_cycle": 2, "end_cycle": 4},
  {"pid": "P1", "start_cycle": 4, "end_cycle": 6}
],
"metrics": {
  "average_waiting_time": 4.5,
  "average_turnaround_time": 8.0
}
}
```

- **Campos:**

  - `algorithm`: Algoritmo utilizado.
  - `quantum`: Valor de quantum (si aplica).
  - `execution_order`: Lista de ejecuciones por proceso.
  - `metrics`: Métricas de eficiencia calculadas.

### 2.2 Resultados de Simulación de Sincronización (Archivo: `resultado_sincronizacion.json`)

- **Formato JSON:**

```json
{
  "mechanism": "Semaphore",
  "actions_log": [
    {"pid": "P1", "action": "READ", "resource": "R1", "cycle": 0, "state": "ACCESSED"},
    {"pid": "P2", "action": "WRITE", "resource": "R1", "cycle": 1, "state": "WAITING"},
    {"pid": "P1", "action": "READ", "resource": "R1", "cycle": 2, "state": "ACCESSED"}
  ]
}
```

- **Campos:**

  - `mechanism`: Mecanismo de sincronización (`Mutex` o `Semaphore`).
  - `actions_log`: Detalle de las acciones y estados durante la simulación.

## 3. Protocolo de Comunicación C ↔ Python

- **Intercambio de Datos:**

  - Entrada: Archivos `.txt` según las estructuras definidas.
  - Salida: Archivos `.json` con resultados de simulación.

- **Flujo de Ejecución:**

  1. Python recibe los archivos `.txt` desde la GUI.
  2. Python invoca la ejecución de los módulos en C, pasando las rutas de los archivos de entrada.
  3. C procesa los archivos y genera los resultados en formato `.json`.
  4. Python lee los archivos `.json` y visualiza los resultados.
