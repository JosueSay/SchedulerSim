# Simulación de Planificación de Procesos y Mecanismos de Sincronización

## Descripción del Proyecto

<!-- *Por completar: Resumen del objetivo, alcance y propósito del simulador.* -->

## Organización de Carpetas

```bash
SchedulerSim/
├── backend/                # Código de backend en C
│   ├── include/            # Archivos de cabecera (.h)
│   ├── src/                # Archivos fuente (.c)
│   └── Makefile            # Compilación de módulos C
├── frontend/               # Interfaz gráfica en Python
│   ├── assets/             # Recursos gráficos (íconos, estilos)
│   ├── controllers/        # Manejo de lógica de simulaciones y comunicación C
│   ├── views/              # Código de la GUI (Tkinter, PyQt)
│   └── main.py             # Punto de entrada de la aplicación
├── data/                   # Archivos de entrada y salida
│   ├── input/              # Archivos de entrada (.txt)
│   └── output/             # Resultados de simulación (.json)
├── docs/                   # Documentación técnica
│   ├── protocolo.md        # Protocolo de comunicación C ↔ Python
│   ├── ...
│   └── context.md          # Descripción del proyecto, requerimientos, etc.
├── images/                 # Diagramas y recursos de documentación
├── Makefile                # General (puede invocar al backend/Makefile)
└── README.md
```

## Instalación de Librerías

<!-- *Por completar: Listado de librerías de Python y herramientas de compilación necesarias.* -->

## Instrucciones de Instalación

<!-- *Por completar: Pasos para preparar el entorno de desarrollo y ejecutar el proyecto.* -->

## Consultar Documentación de Protocolo

* Revisar `docs/protocolo.md` para conocer los formatos de entrada y salida de datos.
* Detalles de la comunicación entre los módulos de C y Python.

## Entradas del Sistema

<!-- *Por completar: Descripción de los archivos de entrada requeridos (procesos, recursos, acciones).* -->

## Salidas del Sistema

<!-- *Por completar: Formato y ubicación de los resultados de simulación.* -->

## Ejecución del Programa

<!-- *Por completar: Comandos para compilar los módulos de C y ejecutar la interfaz gráfica.* -->

### Comando de Compilación (Backend en C)

<!-- ```bash
make -C backend/
``` -->

### Comando de Ejecución (Frontend en Python)

<!-- ```bash
cd frontend/
python main.py
``` -->

## Casos de Uso

<!-- *Por completar: Ejemplos de simulaciones y resultados esperados.* -->

## Créditos

<!-- *Por completar: Integrantes del equipo y roles asignados.* -->
