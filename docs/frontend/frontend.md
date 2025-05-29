# Frontend del Simulador de Algoritmos de Calendarización y Sincronización

Este módulo contiene el frontend web de un simulador interactivo que permite visualizar algoritmos de planificación de procesos y mecanismos de sincronización.

## Tecnologías Utilizadas

- **HTML5**: Estructura de las páginas, con plantillas Jinja2 en `templates/` para cada vista del simulador.
- **CSS3**: Estilos personalizados para cada vista, ubicados en `assets/css/`.
- **JavaScript**: Lógica del lado del cliente para validaciones, navegación, WebSockets y visualización dinámica.

## Estructura General

- **Templates HTML**:
  - `index.html`: Página de inicio
  - `upload-scheduling.html` y `upload-synchronization.html`: Carga de archivos
  - `config-scheduling.html` y `config-synchronization.html`: Configuración de simulaciones
  - `simulation-scheduling.html` y `simulation-synchronization.html`: Visualización del diagrama de Gantt y métricas

- **CSS**:
  - `common.css`: Estilos base reutilizables
  - `config.css`, `scheduling.css`, `synchronization.css`, `simulation.css`: Estilos específicos por vista

- **JavaScript**:
  - `common.js`: Funciones compartidas, como manejo de cookies, entorno y navegación
  - `alerts.js`: Utiliza SweetAlert2 para mostrar mensajes de validación o confirmación
  - `scheduling.js`, `synchronization.js`: Validación y subida de archivos
  - `config-scheduling.js`, `config-synchronization.js`: Manejo de configuración
  - `simulation.js`: WebSocket, renderizado de Gantt, métricas y tablas

## Funcionalidades Clave

- **Protección de rutas**: Se emplea una bandera `allowRouteJump` en cookies y `localStorage` para controlar el acceso en entorno de producción. Si el acceso no está permitido, se redirige a la raíz.
- **Visualización dinámica**: Las simulaciones se muestran como diagramas de Gantt interactivos con información por ciclo, métricas y acciones.
- **Compatibilidad responsiva**: Las vistas se adaptan a distintos tamaños de pantalla para facilitar la visualización.
- **Captura de resultados**: Es posible descargar una imagen del reporte de simulación usando `html2canvas`.

## Explicación Arquitectura

### 1. **Inicialización y Configuración Global**

```python
from fastapi import ...
from fastapi.responses import ...
from fastapi.templating import ...
import os, shutil, json, asyncio
from dotenv import load_dotenv
```

- Se cargan librerías necesarias para manejo de archivos, peticiones, plantillas, WebSockets y entorno.
- Se carga `.env` con `load_dotenv()`.
- Se define la instancia `app` de FastAPI.
- Se monta la carpeta `assets` como `/static` para servir CSS y JS.
- Se configura Jinja2 con las plantillas HTML.
- Se definen rutas y nombres de binarios, directorios de datos, logs, etc.

### 2. **Rutas HTML (Frontend)**

```python
@app.get("/")
...
@app.get("/upload-scheduling")
...
@app.get("/upload-synchronization")
...
```

- Son rutas `GET` que devuelven páginas HTML para el frontend.
- Usan plantillas desde `templates/`.
- Controlan vistas como: inicio, carga de archivos, configuración, simulación.
- En `prod`, algunas páginas requieren cookie `allowRouteJump` para navegar directamente.

### 3. **API REST para subir archivos**

```python
@app.post("/uploadFiles/")
...
```

- Permite al cliente subir archivos `.txt` al servidor.
- Guarda los archivos en `../data/input/`.
- Devuelve confirmación con nombres de archivo.

### 4. **WebSocket de Calendarización**

```python
@app.websocket("/ws/simulation-scheduling")
...
```

- Se conecta un cliente para iniciar simulación de scheduling.
- Recibe config JSON, lanza el binario `scheduling-simulator` por `stdin`.
- Lee eventos `stdout`, los guarda en log, y los reenvía al frontend como JSON.
- Al final, reenvía evento `SIMULATION_END`.

### 5. **WebSocket de Sincronización**

```python
@app.websocket("/ws/simulation-synchronization")
...
```

- Similar al anterior pero usa el binario `synchronization-simulator`.
- La configuración JSON se transforma internamente en `{ "useMutex": 1 | 0 }`.
- El flujo de logs, eventos y finalización es igual al del simulador de calendarización.

## Inicio Rápido

1. Ejecuta el backend con FastAPI
2. Abre `http://localhost:8000` en el navegador
3. Carga los archivos `.txt`, configura la simulación y visualiza los resultados
