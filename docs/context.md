
# Proyecto de Simulación de Planificación de Procesos y Mecanismos de Sincronización

## Contexto del Proyecto

Este proyecto tiene como objetivo reforzar los conocimientos sobre planificación de procesos, algoritmos de scheduling, concurrencia y mecanismos de sincronización (mutex y semáforos). Se desarrollará una aplicación visual con backend en C y Python, permitiendo la simulación de algoritmos de calendarización y escenarios de sincronización.

## Alcance

- Simulador de algoritmos de calendarización de procesos.
- Simulador de mecanismos de sincronización utilizando mutex y semáforos.
- Interfaz gráfica para la visualización dinámica de simulaciones.
- Carga de procesos, recursos y acciones desde archivos de texto.
- Visualización de métricas de eficiencia de los algoritmos implementados.

## Requerimientos

### Tecnológicos

- **Backend:** C y Python.
- **Frontend:** Exclusivamente para visualización, desarrollado en Python (Tkinter, PyQt o similar).
- **Prohibido:** Implementar lógica de mutex o calendarización directamente en Python.

### Funcionalidades

### A. Simulador de Algoritmos de Calendarización

1. **Algoritmos Soportados:**
   - First In First Out (FIFO)
   - Shortest Job First (SJF)
   - Shortest Remaining Time (SRT)
   - Round Robin (Quantum configurable)
   - Priority Scheduling

2. **Carga Dinámica de Procesos:**
   - Archivo `.txt` con el siguiente formato por línea:

     ```bash
     <PID>, <BT>, <AT>, <Priority>
     ```

     Ejemplo:

     ```bash
     P1, 8, 0, 1
     ```

3. **Visualización Dinámica:**
   - Diagrama de Gantt con scroll horizontal.
   - Número de ciclo visible en todo momento.
   - Pintado dinámico de la ejecución de procesos.
   - Diferenciación visual por nombre y color de cada proceso.

4. **Métricas de Eficiencia:**
   - Cálculo y visualización del tiempo promedio de espera (Avg Waiting Time).

### B. Simulador de Mecanismos de Sincronización

1. **Mecanismos Soportados:**
   - Mutex Locks
   - Semáforos

2. **Carga Dinámica de Archivos:**
   - **Procesos:**  

     ```bash
     <PID>, <BT>, <AT>, <Priority>
     ```

   - **Recursos:**  

     ```bash
     <NOMBRE RECURSO>, <CONTADOR>
     ```

     Ejemplo:

     ```bash
     R1, 1
     ```

   - **Acciones:**  

     ```bash
     <PID>, <ACCION>, <RECURSO>, <CICLO>
     ```

     Ejemplo:

     ```bash
     P1, READ, R1, 0
     ```

3. **Visualización Dinámica:**
   - Diagrama de Gantt con scroll horizontal.
   - Número de ciclo visible en todo momento.
   - Estados visuales diferenciados: ACCESSED (acceso exitoso) y WAITING (espera).
   - Diferenciación visual entre accesos exitosos y bloqueos.

### Interfaz Gráfica (GUI)

- Selección de tipo de simulación (Calendarización o Sincronización).
- Configuración de algoritmos de calendarización y quantum.
- Visualización de métricas de eficiencia.
- Carga de archivos de procesos, recursos y acciones.
- Ejecución y visualización dinámica de simulaciones.
- Consulta de información cargada.
- Opción para limpiar y reiniciar simulaciones.

## Entregables

- Código fuente completamente documentado.
- Definición de protocolos de comunicación entre módulos en C y Python.
- Investigaciones realizadas sobre algoritmos y mecanismos implementados.
- README con instrucciones de ejecución claras para el simulador.
- Archivos de ejemplo para la carga de procesos, recursos y acciones.

## Instrucciones de Ejecución

1. **Clonar el Repositorio:**

   ```bash
   git clone <URL_DEL_REPOSITORIO>
   cd <NOMBRE_DEL_PROYECTO>
   ```

2. **Compilar Módulos en C:**

   ```bash
   cd backend/c
   make
   ```

3. **Ejecutar el Simulador:**

   ```bash
   cd ../../frontend
   python simulador.py
   ```

4. **Formato de Archivos de Entrada:**

   - **Procesos:**

     ```bash
     <PID>, <BT>, <AT>, <Priority>
     ```

   - **Recursos:**

     ```bash
     <NOMBRE RECURSO>, <CONTADOR>
     ```

   - **Acciones:**

     ```bash
     <PID>, <ACCION>, <RECURSO>, <CICLO>
     ```

5. **Ejemplo de Uso:**

   - Cargar los archivos desde la interfaz gráfica.
   - Seleccionar el tipo de simulación.
   - Configurar parámetros y ejecutar.

## Consideraciones Adicionales

- Todo el código debe incluir comentarios explicativos.
- Se debe mantener un registro de investigaciones realizadas.
- La visualización debe ser clara, responsiva y dinámica.
