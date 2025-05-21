# Plan de Tareas y Módulos del Proyecto

## Módulos Principales

### 1. Definición del Proyecto

- 1.1 Redactar README inicial con:
  - Descripción del proyecto.
  - Instrucciones de instalación y ejecución.
  - Ejemplos de archivos de entrada.
  - Formatos de carga de archivos.
  - Descripción de las funcionalidades.

- 1.2 Definir protocolo de comunicación entre C y Python:
  - Estructura de datos compartidos (archivos o pipes).
  - Definir formato de mensajes o intercambio de archivos.
  - Documentar en Markdown con ejemplos.

- 1.3 Definir estructura de carpetas del proyecto.

### 2. Backend: Simulador de Algoritmos de Calendarización (C)

- 2.1 Implementar algoritmos de planificación:
  - FIFO (First In First Out).
  - SJF (Shortest Job First).
  - SRT (Shortest Remaining Time).
  - Round Robin (Quantum configurable).
  - Priority Scheduling.

- 2.2 Cargar procesos desde archivos `.txt`:
  - Implementar parser de archivos.
  - Validar formatos y manejar errores.

- 2.3 Generar resultados de simulación:
  - Lista ordenada de ejecución de procesos.
  - Cálculo de métricas: Avg Waiting Time, Turnaround Time.

- 2.4 Exportar resultados en formato legible por Python:
  - JSON, CSV o archivos de texto estructurados.

- 2.5 Documentar el código fuente y uso de cada algoritmo.

### 3. Backend: Simulador de Mecanismos de Sincronización (C)

- 3.1 Implementar mecanismos:
  - Mutex Locks.
  - Semáforos (Counting Semaphores).

- 3.2 Cargar archivos `.txt` de procesos, recursos y acciones:
  - Implementar parser de archivos.
  - Validar formatos y manejar errores.

- 3.3 Simular acciones sobre recursos:
  - Gestionar accesos READ/WRITE.
  - Manejar bloqueos y estados de espera.

- 3.4 Exportar resultados de simulación para visualización.

- 3.5 Documentar el código fuente.

### 4. Frontend: Interfaz Gráfica (Python)

- 4.1 Definir librería gráfica a utilizar (Tkinter, PyQt, etc.).

- 4.2 Implementar interfaz de usuario:
  - Cargar archivos de entrada.
  - Seleccionar tipo de simulación.
  - Configurar parámetros (algoritmos, quantum, etc.).
  - Iniciar simulaciones.

- 4.3 Visualización dinámica:
  - Implementar línea de tiempo (Diagrama de Gantt).
  - Scroll horizontal para eventos extensos.
  - Visualización de ciclos en tiempo real.
  - Diferenciación por colores y nombres de procesos.
  - Mostrar métricas de eficiencia.

- 4.4 Integrar resultados desde backend:
  - Leer archivos de salida generados por C.
  - Procesar datos para la visualización.

- 4.5 Documentar el uso de la interfaz.

### 5. Integración Backend y Frontend

- 5.1 Definir mecanismos de intercambio de datos:
  - Archivos intermedios, sockets o pipes.

- 5.2 Implementar controladores de integración en Python.

- 5.3 Validar y probar la comunicación entre módulos.

### 6. Documentación General

- 6.1 Documentar cada módulo y componente en Markdown.

- 6.2 Crear diagramas de flujo de algoritmos y procesos.

- 6.3 Crear un documento de protocolo de integración C-Python.

- 6.4 Preparar ejemplos de archivos `.txt` para pruebas.

- 6.5 Redactar un README final con:
  - Instrucciones de uso.
  - Ejemplos de ejecución.
  - Formato de carga de archivos.
  - Resultados de ejemplo.

### 7. Pruebas y Validaciones

- 7.1 Preparar casos de prueba para algoritmos de calendarización.

- 7.2 Preparar casos de prueba para sincronización.

- 7.3 Verificar métricas de eficiencia calculadas.

- 7.4 Validar resultados de visualización.

- 7.5 Documentar resultados de pruebas.
