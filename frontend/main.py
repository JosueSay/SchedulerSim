from fastapi import FastAPI, Request, UploadFile, File, Cookie
from fastapi.responses import HTMLResponse, JSONResponse, RedirectResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
import subprocess
import json
import os
import shutil
from dotenv import load_dotenv


load_dotenv()
app = FastAPI()

app.mount("/static", StaticFiles(directory="assets"), name="static")
templates = Jinja2Templates(directory="templates")


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
    os.makedirs(DATA_OUTPUT_DIR, exist_ok=True)
    for filename in os.listdir(DATA_OUTPUT_DIR):
        filePath = os.path.join(DATA_OUTPUT_DIR, filename)
        if filename.endswith(".txt"):
            with open(filePath, "r") as f:
                preview = "".join([next(f, "") for _ in range(2)]).strip()
            filesInfo.append({"name": filename, "preview": preview})
    return JSONResponse(content=filesInfo)

@app.post("/uploadFiles/")
async def uploadFiles(files: list[UploadFile] = File(...)):
    os.makedirs(DATA_OUTPUT_DIR, exist_ok=True)
    for file in files:
        filePath = os.path.join(DATA_OUTPUT_DIR, file.filename)
        with open(filePath, "wb") as buffer:
            shutil.copyfileobj(file.file, buffer)
    return {"status": "Archivos subidos exitosamente", "archivos": [file.filename for file in files]}

@app.post("/simulate/")
async def simulate_simulation(config: dict):
    # Ejecutar el simulador en C
    try:
        subprocess.run(["../backend/bin/simulator"], check=True)
    except subprocess.CalledProcessError as e:
        return JSONResponse(content={"error": "Error ejecutando la simulación"}, status_code=500)

    # Leer Timeline
    timeline_path = os.path.join(DATA_OUTPUT_DIR, "timeline.txt")
    timeline = []
    if os.path.exists(timeline_path):
        with open(timeline_path, "r") as f:
            for line in f:
                parts = [x.strip() for x in line.strip().split(",")]
                if len(parts) == 4:
                    timeline.append({
                        "pid": parts[0],
                        "startCycle": int(parts[1]),
                        "endCycle": int(parts[2]),
                        "state": parts[3]
                    })

    # Leer Métricas
    metrics_path = os.path.join(DATA_OUTPUT_DIR, "metrics.txt")
    metrics = {}
    if os.path.exists(metrics_path):
        with open(metrics_path, "r") as f:
            for line in f:
                key, value = line.strip().split(":")
                metrics[key.strip()] = float(value.strip())

    return JSONResponse(content={
        "timeline": timeline,
        "metrics": metrics,
        "status": "Simulation Completed"
    })

@app.get("/simulation", response_class=HTMLResponse)
async def simulation_page(request: Request):
    return templates.TemplateResponse("simulation.html", {"request": request})
