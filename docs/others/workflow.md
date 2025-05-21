# Flujo General del Simulador de Sistemas Operativos

El simulador es una estructura que funciona por **ciclos** y que, una vez iniciado, comienza su conteo desde 0, simulando el paso del tiempo y el flujo de procesos y recursos.

## 1. Inicio de la simulación

- El simulador arranca su ciclo desde 0.
- El tiempo se maneja en **ciclos**, que representan unidades de tiempo discretas dentro de la simulación.

## 2. Carga de procesos

- Se leen los procesos desde un archivo, cada proceso tiene:
  - **PID**: Identificador único.
  - **BT (Burst Time)**: Tiempo que el proceso requiere para ejecutarse (adaptado a los ciclos del simulador).
  - **AT (Arrival Time)**: Momento en que el proceso llega a la simulación (en ciclos).
  - **Prioridad**: Para los algoritmos que la usan en la calendarización.

- Los procesos entran a la simulación conforme llega su ciclo de llegada (AT).

## 3. Carga de recursos

- Se leen los recursos desde otro archivo, cada recurso tiene:
  - **Nombre del recurso**.
  - **Contador**: Número de instancias disponibles que indican cuántos procesos pueden usar ese recurso simultáneamente.

## 4. Carga de acciones

- Se leen las acciones que los procesos realizan sobre los recursos, con datos:
  - **PID**: Qué proceso realiza la acción.
  - **Acción**: Tipo de acceso (por ejemplo, lectura o escritura).
  - **Recurso**: Sobre qué recurso se realiza la acción.
  - **Ciclo**: Momento en la simulación cuando se quiere realizar esa acción.

## 5. Ejecución del flujo en cada ciclo

- El simulador avanza ciclo a ciclo y hace lo siguiente:

  1. **Verifica qué procesos han llegado** (según su AT).
  
  2. **Aplica un algoritmo de calendarización** para decidir qué proceso se ejecuta en el CPU durante ese ciclo.  
     - El simulador soporta varios algoritmos, como FIFO, SJF, Round Robin, Prioridad, etc.  
     - Cada algoritmo usa diferentes estrategias para decidir el orden de ejecución.

  3. **Verifica las acciones programadas para ese ciclo**:  
     - Si un proceso quiere usar un recurso, el simulador verifica la disponibilidad (según el contador de instancias).  
     - Si el recurso está disponible, el proceso accede y el contador disminuye.  
     - Si no está disponible, el proceso pasa a estado de espera.

  4. **Gestión de recursos**:  
     - Cuando un proceso termina con un recurso, este se libera y el contador se incrementa.  
     - Los procesos que estaban esperando se revisan para pasar a estado listo si el recurso queda disponible.

  5. **Actualización de estados de procesos**:  
     - Los procesos pueden estar en estados como:  
       - Nuevo, Listo, Ejecutando, Esperando, Terminado.

  6. **Visualización dinámica**:  
     - El simulador muestra en una línea de tiempo (diagrama de Gantt) la ejecución de los procesos, estados y acciones.  
     - Se diferencia cada proceso con colores y nombres.  
     - Se puede hacer scroll horizontal si hay muchos eventos.  
     - Se muestran métricas como tiempo promedio de espera.

## 6. Finalización

- La simulación termina cuando todos los procesos han completado su ejecución y todas las acciones sobre recursos han sido realizadas.
