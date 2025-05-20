from fastapi import FastAPI, Request, UploadFile, File, Cookie, WebSocket, WebSocketDisconnect
from fastapi.responses import HTMLResponse, JSONResponse, RedirectResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
import asyncio
import os
import shutil
import json
from dotenv import load_dotenv

# Carga variables de entorno desde un archivo .env
load_dotenv()

app = FastAPI()

# Monta /static para archivos CSS, JS, imágenes, etc.
app.mount("/static", StaticFiles(directory="assets"), name="static")

# Plantillas HTML en /templates
templates = Jinja2Templates(directory="templates")

# Directorios para archivos
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
    """Página para subir procesos (calendarización)"""
    return templates.TemplateResponse("upload-scheduling.html", {"request": request})


@app.get("/upload-synchronization", response_class=HTMLResponse)
async def uploadSynchronizationPage(request: Request):
    """Página para subir procesos, recursos y acciones (sincronización)"""
    return templates.TemplateResponse("upload-synchronization.html", {"request": request})


@app.get("/config", response_class=HTMLResponse)
async def configPage(request: Request, allowRouteJump: str = Cookie(default="false")):
    """Página de configuración (requiere acceso autorizado)"""
    if ENVIRONMENT == "prod" and allowRouteJump != "true":
        return RedirectResponse(url="/")
    return templates.TemplateResponse("config.html", {"request": request})


@app.get("/simulation", response_class=HTMLResponse)
async def simulationPage(request: Request):
    """Página que muestra la simulación"""
    return templates.TemplateResponse("simulation.html", {"request": request})


# ======================== API FUNCIONAL ========================

@app.get("/environment/")
async def getEnvironment():
    """Devuelve el entorno actual de ejecución"""
    return {"environment": ENVIRONMENT}


@app.get("/listFiles/")
async def listFiles():
    """Lista los archivos .txt disponibles y una vista previa de las primeras líneas"""
    filesInfo = []
    os.makedirs(DATA_INPUT_DIR, exist_ok=True)
    for filename in os.listdir(DATA_INPUT_DIR):
        if filename.endswith(".txt"):
            filePath = os.path.join(DATA_INPUT_DIR, filename)
            with open(filePath, "r") as f:
                preview = "".join([next(f, "") for _ in range(2)]).strip()
            filesInfo.append({"name": filename, "preview": preview})
    return JSONResponse(content=filesInfo)


@app.post("/uploadFiles/")
async def uploadFiles(files: list[UploadFile] = File(...)):
    """Recibe uno o más archivos .txt y los guarda en el directorio de entrada"""
    os.makedirs(DATA_INPUT_DIR, exist_ok=True)
    for file in files:
        filePath = os.path.join(DATA_INPUT_DIR, file.filename)
        with open(filePath, "wb") as buffer:
            shutil.copyfileobj(file.file, buffer)
    return {"status": "Archivos subidos exitosamente", "archivos": [file.filename for file in files]}


# ======================== WEBSOCKET PARA SIMULACIÓN ========================

@app.websocket("/ws/simulation")
async def websocketSimulation(websocket: WebSocket):
    """
    WebSocket que ejecuta el simulador en tiempo real.
    Recibe la configuración, ejecuta el simulador y envía eventos JSON al cliente.
    """
    await websocket.accept()
    try:
        config_data = await websocket.receive_text()
        config = json.loads(config_data)

        process = await asyncio.create_subprocess_exec(
            "../backend/bin/simulator",
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE,
        )

        while True:
            line = await process.stdout.readline()
            if not line:
                break

            decoded_line = line.decode().strip()
            try:
                event_data = json.loads(decoded_line)
                if websocket.client_state.value == 1:
                    await websocket.send_text(json.dumps(event_data))
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
