from fastapi import FastAPI, Request, UploadFile, File, Cookie, WebSocket, WebSocketDisconnect
from fastapi.responses import HTMLResponse, JSONResponse, RedirectResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
import asyncio
import subprocess
import json
import os
import shutil
from dotenv import load_dotenv

load_dotenv()
app = FastAPI()

app.mount("/static", StaticFiles(directory="assets"), name="static")
templates = Jinja2Templates(directory="templates")

DATA_INPUT_DIR = "../data/input/"
DATA_OUTPUT_DIR = "../data/output/"
ENVIRONMENT = os.getenv("ENVIRONMENT", "prod")


@app.get("/environment/")
async def get_environment():
    return {"environment": ENVIRONMENT}


@app.get("/", response_class=HTMLResponse)
async def home(request: Request):
    return templates.TemplateResponse("index.html", {"request": request})


@app.get("/config", response_class=HTMLResponse)
async def configPage(request: Request, allowRouteJump: str = Cookie(default="false")):
    if ENVIRONMENT == "prod" and allowRouteJump != "true":
        return RedirectResponse(url="/")
    return templates.TemplateResponse("config.html", {"request": request})


@app.get("/listFiles/")
async def listFiles():
    filesInfo = []
    os.makedirs(DATA_INPUT_DIR, exist_ok=True)
    for filename in os.listdir(DATA_INPUT_DIR):
        filePath = os.path.join(DATA_INPUT_DIR, filename)
        if filename.endswith(".txt"):
            with open(filePath, "r") as f:
                preview = "".join([next(f, "") for _ in range(2)]).strip()
            filesInfo.append({"name": filename, "preview": preview})
    return JSONResponse(content=filesInfo)


@app.post("/uploadFiles/")
async def uploadFiles(files: list[UploadFile] = File(...)):
    os.makedirs(DATA_INPUT_DIR, exist_ok=True)
    for file in files:
        filePath = os.path.join(DATA_INPUT_DIR, file.filename)
        with open(filePath, "wb") as buffer:
            shutil.copyfileobj(file.file, buffer)
    return {"status": "Archivos subidos exitosamente", "archivos": [file.filename for file in files]}


@app.get("/simulation", response_class=HTMLResponse)
async def simulation_page(request: Request):
    return templates.TemplateResponse("simulation.html", {"request": request})


@app.websocket("/ws/simulation")
async def websocket_simulation(websocket: WebSocket):
    await websocket.accept()
    try:
        # Recibir la configuración inicial desde el cliente
        config_data = await websocket.receive_text()
        config = json.loads(config_data)

        # Ejecutar el simulador con la configuración recibida
        process = await asyncio.create_subprocess_exec(
            "../backend/bin/simulator",
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE
        )

        # Leer salida del simulador línea por línea y enviarla por WebSocket
        while True:
            line = await process.stdout.readline()
            if not line:
                break

            decoded_line = line.decode().strip()
            try:
                event_data = json.loads(decoded_line)
                await websocket.send_text(json.dumps(event_data))
            except json.JSONDecodeError:
                continue  # Ignora líneas no JSON

        # Enviar mensaje de finalización
        await websocket.send_text(json.dumps({"event": "SIMULATION_END"}))

    except WebSocketDisconnect:
        print("Cliente WebSocket desconectado.")
    except Exception as e:
        print(f"Error en WebSocket: {e}")
        await websocket.send_text(json.dumps({"event": "ERROR", "message": str(e)}))
    finally:
        await websocket.close()
