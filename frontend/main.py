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

# Monta la carpeta de archivos estáticos (CSS, JS, imágenes) en /static
app.mount("/static", StaticFiles(directory="assets"), name="static")

# Configura el motor de plantillas Jinja2 para renderizar HTML
templates = Jinja2Templates(directory="templates")

# Directorios para datos de entrada y salida
DATA_INPUT_DIR = "../data/input/"
DATA_OUTPUT_DIR = "../data/output/"
ENVIRONMENT = os.getenv("ENVIRONMENT", "prod")


@app.get("/environment/")
async def get_environment():
    """Devuelve el entorno actual de ejecución."""
    return {"environment": ENVIRONMENT}


@app.get("/", response_class=HTMLResponse)
async def home(request: Request):
    """Página principal."""
    return templates.TemplateResponse("index.html", {"request": request})


@app.get("/config", response_class=HTMLResponse)
async def configPage(request: Request, allowRouteJump: str = Cookie(default="false")):
    """
    Página de configuración.
    Si el entorno es producción y no se permite saltar ruta, redirige a /.
    """
    if ENVIRONMENT == "prod" and allowRouteJump != "true":
        return RedirectResponse(url="/")
    return templates.TemplateResponse("config.html", {"request": request})


@app.get("/listFiles/")
async def listFiles():
    """
    Lista los archivos .txt disponibles en el directorio de entrada,
    mostrando su nombre y las dos primeras líneas como preview.
    """
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
    """
    Recibe uno o más archivos subidos y los guarda en el directorio de entrada.
    """
    os.makedirs(DATA_INPUT_DIR, exist_ok=True)
    for file in files:
        filePath = os.path.join(DATA_INPUT_DIR, file.filename)
        with open(filePath, "wb") as buffer:
            shutil.copyfileobj(file.file, buffer)
    return {"status": "Archivos subidos exitosamente", "archivos": [file.filename for file in files]}


@app.get("/simulation", response_class=HTMLResponse)
async def simulation_page(request: Request):
    """Página que muestra la simulación."""
    return templates.TemplateResponse("simulation.html", {"request": request})


@app.websocket("/ws/simulation")
async def websocketSimulation(websocket: WebSocket):
    """
    WebSocket que ejecuta la simulación en tiempo real.

    Recibe la configuración por el WebSocket, ejecuta el proceso externo
    y envía los eventos JSON línea por línea al cliente.

    Al finalizar, envía evento SIMULATION_END o ERROR según corresponda.
    """
    await websocket.accept()
    try:
        config_data = await websocket.receive_text()
        config = json.loads(config_data)

        # Ejecuta el simulador como proceso asíncrono
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
                # Envía al cliente solo si está conectado
                if websocket.client_state.value == 1:  # WebSocket.CONNECTED = 1
                    await websocket.send_text(json.dumps(event_data))
                else:
                    print("Cliente ya desconectado, no se envía más data.")
                    break
            except json.JSONDecodeError:
                # Ignora líneas que no sean JSON válidos
                continue

        # Envía evento de finalización si el cliente sigue conectado
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
            # Si ya está cerrado no hace nada
            pass
