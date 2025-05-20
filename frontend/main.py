# main.py - Backend principal para SchedulerSim

from fastapi import FastAPI, Request, UploadFile, File, Cookie, WebSocket, WebSocketDisconnect
from fastapi.responses import HTMLResponse, JSONResponse, RedirectResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
import asyncio
import os
import shutil
import json
from dotenv import load_dotenv

# Cargar variables de entorno desde archivo .env
load_dotenv()

# Inicializar aplicación FastAPI
app = FastAPI()

# Montar carpeta de archivos estáticos
app.mount("/static", StaticFiles(directory="assets"), name="static")

# Configurar directorio de plantillas HTML
templates = Jinja2Templates(directory="templates")

# Directorios para archivos de entrada/salida
DATA_INPUT_DIR = "../data/input/"
DATA_OUTPUT_DIR = "../data/output/"
ENVIRONMENT = os.getenv("ENVIRONMENT", "prod")

# ======================== RUTAS DE PÁGINAS ========================

@app.get("/", response_class=HTMLResponse)
async def home(request: Request):
    """Página principal: selección de tipo de simulación"""
    return templates.TemplateResponse("index.html", {"request": request})


@app.get("/upload-scheduling", response_class=HTMLResponse)
async def uploadSchedulingPage(request: Request):
    """Página para subir archivo de procesos (calendarización)"""
    return templates.TemplateResponse("upload-scheduling.html", {"request": request})


@app.get("/upload-synchronization", response_class=HTMLResponse)
async def uploadSynchronizationPage(request: Request):
    """Página para subir procesos, recursos y acciones (sincronización)"""
    return templates.TemplateResponse("upload-synchronization.html", {"request": request})


@app.get("/config-scheduling", response_class=HTMLResponse)
async def configSchedulingPage(request: Request, allowRouteJump: str = Cookie(default="false")):
    """Página de configuración de simulación de calendarización"""
    if ENVIRONMENT == "prod" and allowRouteJump != "true":
        return RedirectResponse(url="/")
    return templates.TemplateResponse("config-scheduling.html", {"request": request})


@app.get("/simulation-scheduling", response_class=HTMLResponse)
async def simulationSchedulingPage(request: Request):
    """Página de simulación tipo Gantt para calendarización"""
    return templates.TemplateResponse("simulation-scheduling.html", {"request": request})


@app.get("/config-synchronization", response_class=HTMLResponse)
async def configSynchronizationPage(request: Request):
    """Página de configuración de sincronización (placeholder)"""
    return templates.TemplateResponse("config-synchronization.html", {"request": request})


@app.get("/simulation-synchronization", response_class=HTMLResponse)
async def simulationSynchronizationPage(request: Request):
    """Página de simulación tipo Gantt para sincronización (placeholder)"""
    return templates.TemplateResponse("simulation-synchronization.html", {"request": request})


# ======================== API FUNCIONAL ========================

@app.get("/environment/")
async def getEnvironment():
    """Devuelve el entorno actual (dev/prod)"""
    return {"environment": ENVIRONMENT}


@app.post("/uploadFiles/")
async def uploadFiles(files: list[UploadFile] = File(...)):
    """Guarda archivos .txt en el directorio de entrada"""
    os.makedirs(DATA_INPUT_DIR, exist_ok=True)
    for file in files:
        filePath = os.path.join(DATA_INPUT_DIR, file.filename)
        with open(filePath, "wb") as buffer:
            shutil.copyfileobj(file.file, buffer)
    return {"status": "Archivos subidos exitosamente", "archivos": [file.filename for file in files]}


# ======================== WEBSOCKET PARA SIMULACIÓN ========================

@app.websocket("/ws/simulation-scheduling")
async def websocketSimulationScheduling(websocket: WebSocket):
    """WebSocket para simulación de calendarización"""
    await websocket.accept()
    try:
        configData = await websocket.receive_text()
        config = json.loads(configData)

        process = await asyncio.create_subprocess_exec(
            "../backend/bin/simulator",
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE,
        )

        while True:
            line = await process.stdout.readline()
            if not line:
                break

            decodedLine = line.decode().strip()
            try:
                eventData = json.loads(decodedLine)
                if websocket.client_state.value == 1:
                    await websocket.send_text(json.dumps(eventData))
            except json.JSONDecodeError:
                continue

        if websocket.client_state.value == 1:
            await websocket.send_text(json.dumps({"event": "SIMULATION_END"}))

    except WebSocketDisconnect:
        print("Cliente WebSocket desconectado.")
    except Exception as e:
        print(f"Error en WebSocket: {e}")
        if websocket.client_state.value == 1:
            await websocket.send_text(json.dumps({"event": "ERROR", "message": str(e)}))
    finally:
        try:
            if websocket.client_state.value == 1:
                await websocket.close()
        except RuntimeError:
            pass


@app.websocket("/ws/simulation-synchronization")
async def websocketSimulationSynchronization(websocket: WebSocket):
    """WebSocket para simulación de sincronización (futuro binario)"""
    await websocket.accept()
    try:
        configData = await websocket.receive_text()
        config = json.loads(configData)

        process = await asyncio.create_subprocess_exec(
            "../backend/bin/synchronization_simulator",
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE,
        )

        while True:
            line = await process.stdout.readline()
            if not line:
                break

            decodedLine = line.decode().strip()
            try:
                eventData = json.loads(decodedLine)
                if websocket.client_state.value == 1:
                    await websocket.send_text(json.dumps(eventData))
            except json.JSONDecodeError:
                continue

        if websocket.client_state.value == 1:
            await websocket.send_text(json.dumps({"event": "SIMULATION_END"}))

    except WebSocketDisconnect:
        print("Cliente WebSocket desconectado.")
    except Exception as e:
        print(f"Error en WebSocket: {e}")
        if websocket.client_state.value == 1:
            await websocket.send_text(json.dumps({"event": "ERROR", "message": str(e)}))
    finally:
        try:
            if websocket.client_state.value == 1:
                await websocket.close()
        except RuntimeError:
            pass